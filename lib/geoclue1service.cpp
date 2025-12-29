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

int GeoClue1Service::GetLastSatellite(int &satellite_used, int &satellite_visible, UsedPRN &used_prn, UsedSat &sat_info)
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
    return -1;
}

int GeoClue1Service::GetSatellite(int &satellite_used, int &satellite_visible, UsedPRN &used_prn, UsedSat &sat_info)
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
    return -1;
}

int GeoClue1Service::GetVelocity(int& timestamp, double& speed, double& direction, double& climb)
{
    if (!m_provider || !m_provider->isAvailable()) {
        timestamp = QDateTime::currentSecsSinceEpoch();
        speed = 0.0;
        direction = 0.0;
        climb = 0.0;
        return -1;
    }

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

int GeoClue1Service::GetAddress(QVariantMap &address, Accuracy &accuracy)
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
    return -1;
}

void GeoClue1Service::AddReference()
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
}

QString GeoClue1Service::GetProviderInfo(QString &Description)
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
    return "";
}

int GeoClue1Service::GetStatus()
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
    return -1;
}

void GeoClue1Service::RemoveReference()
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
}

void GeoClue1Service::SetOptions(const QVariantMap &options)
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
}

int GeoClue1Service::AddressToPosition(const QVariantMap &address, double &latitude, double &longitude, double &altitude, Accuracy &accuracy)
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
    return -1;
}

int GeoClue1Service::FreeformAddressToPosition(const QString &address, double &latitude, double &longitude, double &altitude, Accuracy &accuracy)
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
    return -1;
}

QDBusObjectPath GeoClue1Service::Create()
{
    return QDBusObjectPath();
}

void GeoClue1Service::AddressStart()
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
}

QString GeoClue1Service::GetAddressProvider(QString &description, QString &service, QString &path)
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
    return QString();
}

QString GeoClue1Service::GetPositionProvider(QString &description, QString &service, QString &path)
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
    return QString();
}

void GeoClue1Service::PositionStart()
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
}

void GeoClue1Service::SetRequirements(int accuracy_level, int time, bool require_updates, int allowed_resources)
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
}

QVariantMap GeoClue1Service::PositionToAddress(double latitude, double longitude, Accuracy position_accuracy, Accuracy &address_accuracy)
{
    qDebug() << Q_FUNC_INFO << "Not implemented yeat";
    return QVariantMap();
}


