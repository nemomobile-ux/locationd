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
    m_updateTimer.setInterval(30000);
    m_updateTimer.setSingleShot(true);
    connect(&m_updateTimer, &QTimer::timeout, this, &CellProvider::queryMLS);
}

CellProvider::~CellProvider()
{
}

void CellProvider::requestLocationUpdate()
{
    fetchCellData();
    m_updateTimer.start();
}

void CellProvider::fetchCellData()
{
    // TODO: fetch real data from ofono
    m_cells.clear();
    m_cells.append({ 12345, 101, 310, 260, -70 });
    m_cells.append({ 12346, 101, 310, 260, -85 });
}

void CellProvider::queryMLS()
{
    if (m_cells.isEmpty())
        return;

    QJsonObject root;
    QJsonArray cells;
    for (const CellTower& t : m_cells) {
        QJsonObject c;
        c["cellId"] = t.cellId;
        c["locationAreaCode"] = t.locationAreaCode;
        c["mobileCountryCode"] = t.mobileCountryCode;
        c["mobileNetworkCode"] = t.mobileNetworkCode;
        c["signalStrength"] = t.signalStrength;
        c["radioType"] = "gsm";
        cells.append(c);
    }
    root["cellTowers"] = cells;

    QNetworkRequest req(QUrl("https://location.services.mozilla.com/v1/geolocate?key=test"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkReply* reply = manager->post(req, QJsonDocument(root).toJson());

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "MLS request failed:" << reply->errorString();
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

void CellProvider::handleMLSResponse(double lat, double lon, int accuracy)
{
    m_latitude = lat;
    m_longitude = lon;
    m_altitude = 0.0;
    m_accuracy.horizontal = accuracy;
    m_available = true;

    emit updated();
    qInfo() << "[CellProvider] Location updated via MLS:" << lat << lon << "accuracy:" << accuracy;
}
