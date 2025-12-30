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

#include "cellplugin.h"

#include <QDBusInterface>
#include <QDBusObjectPath>
#include <QDBusReply>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

CellProvider::CellProvider(QObject* parent)
    : ILocationProvider(parent)
{
}

CellProvider::~CellProvider()
{
}

void CellProvider::setActive(bool active)
{

}

void CellProvider::requestLocationUpdate()
{
    fetchCellData();

    if (m_cells.isEmpty()) {
        qWarning() << "CellProvider: no cell data";
        m_available = false;
        return;
    }

    queryMLS();
}

void CellProvider::fetchCellData()
{
    m_cells.clear();

    QDBusInterface manager(
        "org.ofono",
        "/",
        "org.ofono.Manager",
        QDBusConnection::systemBus());

    QDBusReply<QList<QDBusObjectPath>> modems = manager.call("GetModems");

    if (!modems.isValid())
        return;

    for (const QDBusObjectPath& path : modems.value()) {
        QDBusInterface netreg(
            "org.ofono",
            path.path(),
            "org.ofono.NetworkRegistration",
            QDBusConnection::systemBus());

        if (!netreg.isValid())
            continue;

        QDBusReply<QVariantMap> props = netreg.call("GetProperties");

        if (!props.isValid())
            continue;

        const QVariantMap p = props.value();

        if (!p.contains("CellId"))
            continue;

        CellTower c;
        c.cellId = p["CellId"].toInt();
        c.locationAreaCode = p["LocationAreaCode"].toInt();
        c.mobileCountryCode = p["MobileCountryCode"].toString().toInt();
        c.mobileNetworkCode = p["MobileNetworkCode"].toString().toInt();
        c.signalStrength = p.value("Strength", -80).toInt();

        m_cells.append(c);
    }

    qInfo() << "CellProvider:" << m_cells.size() << "cells collected";
}

void CellProvider::queryMLS()
{
    QJsonArray cells;

    for (const CellTower& t : m_cells) {
        QJsonObject c;
        c["cellId"] = t.cellId;
        c["locationAreaCode"] = t.locationAreaCode;
        c["mobileCountryCode"] = t.mobileCountryCode;
        c["mobileNetworkCode"] = t.mobileNetworkCode;
        c["signalStrength"] = t.signalStrength;
        c["radioType"] = "lte"; // лучше, чем gsm по умолчанию
        cells.append(c);
    }

    QJsonObject root;
    root["cellTowers"] = cells;

    QNetworkRequest req(QUrl("https://location.services.mozilla.com/v1/geolocate?key=test"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    auto* nam = new QNetworkAccessManager(this);
    QNetworkReply* reply = nam->post(req, QJsonDocument(root).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "MLS error:" << reply->errorString();
            return;
        }

        const QJsonObject obj = QJsonDocument::fromJson(reply->readAll()).object();

        const QJsonObject loc = obj["location"].toObject();
        handleMLSResponse(
            loc["lat"].toDouble(),
            loc["lng"].toDouble(),
            obj["accuracy"].toInt());
    });
}

void CellProvider::handleMLSResponse(double lat, double lon, int accuracy)
{
    m_latitude = lat;
    m_longitude = lon;
    m_altitude = 0.0;

    m_accuracy.level = static_cast<int>(AccuracyLevel::STREET);
    m_accuracy.horizontal = accuracy;

    m_available = true;

    emit updated();
    qInfo() << "[CellProvider] Location updated via MLS:" << lat << lon << "accuracy:" << accuracy;
}
