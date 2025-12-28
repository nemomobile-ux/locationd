/*
 * Copyright (C) 2025 Chupligin Sergey <neochapay@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "wifiplugin.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QtDBus/QtDBus>

// Симуляция получения точек через ConnMan
#include <QDBusInterface>
#include <QDBusReply>

WifiProvider::WifiProvider(QObject* parent)
    : ILocationProvider(parent)
{
    m_updateTimer.setSingleShot(true);
    m_updateTimer.setInterval(30000);
    connect(&m_updateTimer, &QTimer::timeout, this, &WifiProvider::queryMLS);
}

WifiProvider::~WifiProvider() { }

void WifiProvider::requestLocationUpdate()
{
    fetchWifiData();
    if (!m_accessPoints.isEmpty())
        m_updateTimer.start();
}

void WifiProvider::fetchWifiData()
{
    m_accessPoints.clear();

    QDBusInterface manager("net.connman", "/", "net.connman.Manager", QDBusConnection::systemBus());
    if (!manager.isValid()) {
        qWarning() << "ConnMan Manager not available";
        return;
    }

    QDBusReply<QList<QDBusObjectPath>> servicesReply = manager.call("GetServices");
    if (!servicesReply.isValid()) {
        qWarning() << "Failed to get services from ConnMan";
        return;
    }

    for (const QDBusObjectPath& path : servicesReply.value()) {
        QDBusInterface service("net.connman", path.path(), "net.connman.Service", QDBusConnection::systemBus());
        if (!service.isValid())
            continue;

        QDBusReply<QVariantMap> propsReply = service.call("GetProperties");
        if (!propsReply.isValid())
            continue;

        QVariantMap props = propsReply.value();
        if (!props.contains("Type") || props["Type"].toString() != "wifi")
            continue;

        if (props.contains("Strength") && props.contains("Name")) {
            WifiAccessPoint ap;
            ap.macAddress = props["Name"].toString(); // SSID как пример
            ap.signalStrength = props["Strength"].toInt();
            ap.channel = 0;
            m_accessPoints.append(ap);
        }
    }

    qInfo() << "[WifiProvider] Found" << m_accessPoints.size() << "Wi-Fi networks";
}

void WifiProvider::queryMLS()
{
    if (m_accessPoints.isEmpty())
        return;

    QJsonObject root;
    QJsonArray wifis;
    for (const WifiAccessPoint& ap : m_accessPoints) {
        QJsonObject obj;
        obj["macAddress"] = ap.macAddress;
        obj["signalStrength"] = ap.signalStrength;
        obj["channel"] = ap.channel;
        wifis.append(obj);
    }
    root["wifiAccessPoints"] = wifis;

    QNetworkRequest req(QUrl("https://location.services.mozilla.com/v1/geolocate?key=test"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->post(req, QJsonDocument(root).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "MLS Wi-Fi request failed:" << reply->errorString();
        } else {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject obj = doc.object();
            if (obj.contains("location")) {
                QJsonObject loc = obj["location"].toObject();
                double lat = loc["lat"].toDouble();
                double lon = loc["lng"].toDouble();
                int acc = obj["accuracy"].toInt();
                handleMLSResponse(lat, lon, acc);
            }
        }
        reply->deleteLater();
        reply->manager()->deleteLater();
    });
}

void WifiProvider::handleMLSResponse(double lat, double lon, int accuracy)
{
    m_latitude = lat;
    m_longitude = lon;
    m_altitude = 0.0;
    m_accuracy.horizontal = accuracy;
    m_available = true;

    emit updated();
    qInfo() << "[WifiProvider] Location updated via Wi-Fi MLS:" << lat << lon << "accuracy:" << accuracy;
}
