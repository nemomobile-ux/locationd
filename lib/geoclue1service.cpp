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

#include "geoclue1service.h"

GeoClue1Service::GeoClue1Service(ILocationProvider* provider, QObject* parent)
    : QObject(parent)
    , m_provider(provider)
{
}

int GeoClue1Service::GetPosition(int& timestamp, double& latitude, double& longitude, double& altitude, Accuracy& accuracy)
{
    if (!m_provider || !m_provider->isAvailable()) {
        timestamp = 0;
        latitude = 0.0;
        longitude = 0.0;
        altitude = 0.0;
        accuracy = Accuracy {};
        return -1;
    }

    m_provider->requestLocationUpdate();

    timestamp = QDateTime::currentSecsSinceEpoch();
    latitude = m_provider->latitude();
    longitude = m_provider->longitude();
    altitude = m_provider->altitude();

    Accuracy provAcc = m_provider->accuracy();
    accuracy.level = provAcc.level;
    accuracy.horizontal = provAcc.horizontal;
    accuracy.vertical = provAcc.vertical;

    return 0;
}

QVector<SatInfoFull> GeoClue1Service::GetSatellites()
{
    if (!m_provider)
        return {};
    return m_provider->satellites();
}

int GeoClue1Service::GetVelocity(int& fields, int& timestamp, double& speed, double& direction, double& climb)
{
    if (!m_provider || !m_provider->isAvailable()) {
        fields = 0;
        timestamp = QDateTime::currentSecsSinceEpoch();
        speed = 0.0;
        direction = 0.0;
        climb = 0.0;
        return -1;
    }

    fields = 3; // speed, direction, climb
    timestamp = QDateTime::currentSecsSinceEpoch();
    speed = m_provider->speed(); // m/s
    direction = m_provider->direction(); // degrees

    static double lastAltitude = m_provider->altitude();
    static qint64 lastTime = timestamp;

    double currentAltitude = m_provider->altitude();
    qint64 dt = timestamp - lastTime;
    if (dt > 0)
        climb = (currentAltitude - lastAltitude) / dt;
    else
        climb = 0.0;

    lastAltitude = currentAltitude;
    lastTime = timestamp;

    return 0;
}
