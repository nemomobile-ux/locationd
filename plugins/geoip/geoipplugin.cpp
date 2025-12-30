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

#include "geoipplugin.h"
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

GeoIPProvider::GeoIPProvider(QObject* parent)
    : ILocationProvider(parent)
{
    m_manager = new QNetworkAccessManager(this);

    QDBusConnection bus = QDBusConnection::systemBus();
    if (!bus.isConnected()) {
        qWarning() << "GeoIPProvider: Cannot connect to system bus";
        return;
    }

    QDBusInterface connmanManager("net.connman",
        "/",
        "net.connman.Manager",
        bus);
    if (!connmanManager.isValid()) {
        qWarning() << "GeoIPProvider: ConnMan Manager interface not available";
        return;
    }

    bus.connect("net.connman",
        "/",
        "net.connman.Manager",
        "PropertyChanged",
        this,
        SLOT(queryGeoIP()));
}

void GeoIPProvider::setActive(bool active)
{

}

void GeoIPProvider::requestLocationUpdate()
{
    queryGeoIP();
}

void GeoIPProvider::queryGeoIP()
{
    QNetworkRequest request(QUrl("http://ip-api.com/json/"));
    QNetworkReply* reply = m_manager->get(request);
    connect(reply, &QNetworkReply::finished, this, &GeoIPProvider::parseGeoIPJson);
}

void GeoIPProvider::parseGeoIPJson()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();

            m_latitude = obj["lat"].toDouble();
            m_longitude = obj["lon"].toDouble();
            m_altitude = 0.0;
            m_speed = 0.0;
            m_direction = 0.0;

            m_accuracy.level = static_cast<int>(AccuracyLevel::CITY);
            m_accuracy.horizontal = 50000.0;
            m_accuracy.vertical = 0.0;

            emit updated();
            qDebug() << "[GeoIP] Updated Lat:" << m_latitude
                     << "Lon:" << m_longitude
                     << "Accuracy:" << m_accuracy.horizontal << "m";
        }
    } else {
        qWarning() << "GeoIPProvider error:" << reply->errorString();
    }
    reply->deleteLater();
}
