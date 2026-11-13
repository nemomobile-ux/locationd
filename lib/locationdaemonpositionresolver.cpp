/*
 * Copyright (C) 2026 Chupligin Sergey <neochapay@gmail.com>
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

#include "locationdaemonpositionresolver.h"

#include <QDateTime>

LocationDaemonPositionResolver::LocationDaemonPositionResolver(PluginManager* manager, const QString& objectPath, QObject* parent)
    : QObject { parent }
    , m_pluginManager(manager)
    , m_objectPath(objectPath.isEmpty() ? QStringLiteral("/org/freedesktop/Geoclue/Master/client0") : objectPath)
    , m_refCount(0)
{
    connect(m_pluginManager, &PluginManager::providerPositionUpdated, this, &LocationDaemonPositionResolver::onProviderPositionUpdated);

    connect(m_pluginManager, &PluginManager::bestProviderChanged, this, &LocationDaemonPositionResolver::onBestProviderChanged);

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, &LocationDaemonPositionResolver::onProviderPositionUpdated);
}

void LocationDaemonPositionResolver::AddReference()
{
    m_refCount++;
    onProviderPositionUpdated();
}

void LocationDaemonPositionResolver::RemoveReference()
{
    m_refCount--;
}

QString LocationDaemonPositionResolver::GetProviderInfo(QString& Description)
{
    Description = "Nemomobile location provider";
    return QStringLiteral("locationd");
}

int LocationDaemonPositionResolver::GetStatus()
{
    return 3; // GEOCLUE_STATUS_AVAILABLE
}

void LocationDaemonPositionResolver::SetOptions(const QVariantMap& options)
{
    if (options != m_option) {
        m_option = options;
        if (m_option.contains("UpdateInterval")) {
            m_updateTimer->start(m_option["UpdateInterval"].toInt());
        }
        onBestProviderChanged();
    }
}

void LocationDaemonPositionResolver::AddressStart()
{
    m_pluginManager->setActive(true);
}

QString LocationDaemonPositionResolver::GetAddressProvider(QString& description, QString& service, QString& path)
{
    return "";
}

QString LocationDaemonPositionResolver::GetPositionProvider(QString& description, QString& service, QString& path)
{
    QString name = "NemoMobile location daemon";
    description = "NemoMobile Location provider";
    service = "org.freedesktop.Geoclue.Master";
    path = m_objectPath;

    emit PositionProviderChanged(name, description, service, path);
    return name;
}

void LocationDaemonPositionResolver::PositionStart()
{
    m_pluginManager->setActive(true);

    QString name = "NemoMobile location daemon";
    QString description = "NemoMobile Location provider";
    QString service = "org.freedesktop.Geoclue.Master";
    QString path = m_objectPath;

    emit PositionProviderChanged(name, description, service, path);
    onProviderPositionUpdated();
}

void LocationDaemonPositionResolver::SetRequirements(int accuracy_level, int time, bool require_updates, int allowed_resources)
{
    qDebug() << "SetRequirements called:"
             << "accuracy_level=" << accuracy_level
             << "time=" << time
             << "require_updates=" << require_updates
             << "allowed_resources=" << allowed_resources;

    if (allowed_resources & GEOCLUE_RESOURCE_GPS) {
        qDebug() << "GPS required";
    }

    if (allowed_resources & GEOCLUE_RESOURCE_WIFI) {
        qDebug() << "Wi-Fi required";
    }

    if (allowed_resources & GEOCLUE_RESOURCE_CELL) {
        qDebug() << "Cell tower required";
    }

    if (allowed_resources & GEOCLUE_RESOURCE_OTHER) {
        qDebug() << "Other required";
    }
}

int LocationDaemonPositionResolver::GetPosition(int& timestamp, double& latitude, double& longitude, double& altitude, Accuracy& accuracy)
{
    if (!m_pluginManager->bestProvider()) {
        qDebug() << "Without provider!";
        return 0;
    }

    if (!m_lastCoord.isValid()) {
        m_pluginManager->bestProvider()->requestLocationUpdate();
        return 0;
    }

    timestamp = m_lastUpdate.currentSecsSinceEpoch();
    latitude = m_lastCoord.latitude();
    longitude = m_lastCoord.longitude();
    altitude = m_lastCoord.altitude();
    accuracy = m_lastAccuracy;
    return Latitude | Longitude | Altitude;
}

int LocationDaemonPositionResolver::GetVelocity(int& timestamp, double& speed, double& direction, double& climb)
{
    if (!m_pluginManager->bestProvider()) {
        return 0;
    }

    if (!m_lastCoord.isValid()) {
        return 0;
    }

    timestamp = m_lastUpdate.currentSecsSinceEpoch();
    speed = m_pluginManager->bestProvider()->speed();
    direction = m_pluginManager->bestProvider()->direction();
    climb = 0.0;
    return 1 | 2; // speed | direction
}

int LocationDaemonPositionResolver::GetAddress(QVariantMap& address, Accuracy& accuracy)
{
    QVariantMap addressMap;
    addressMap["Country"] = "USSR";
    address = addressMap;
    accuracy = Accuracy();
    return 1;
}

int LocationDaemonPositionResolver::GetLastSatellite(int& satellite_used, int& satellite_visible, QList<int>& used_prn, QList<QGeoSatelliteInfo>& sat_info)
{
    if (!m_pluginManager->bestProvider()) {
        return 0;
    }
    if (m_pluginManager->bestProvider()->satellites().satellitesView.count() < 1) {
        return 0;
    }

    int timestamp = m_lastUpdate.currentSecsSinceEpoch();
    satellite_visible = m_pluginManager->bestProvider()->satellites().satellitesView.count();
    satellite_used = m_pluginManager->bestProvider()->satellites().satellitesUsed.count();
    sat_info = m_pluginManager->bestProvider()->satellites().satellitesView;

    used_prn.clear();
    foreach (QGeoSatelliteInfo s, m_pluginManager->bestProvider()->satellites().satellitesUsed) {
        used_prn.push_back(s.satelliteIdentifier());
    }
    return timestamp;
}

int LocationDaemonPositionResolver::GetSatellite(int& satellite_used, int& satellite_visible, QList<int>& used_prn, QList<QGeoSatelliteInfo>& sat_info)
{
    return GetLastSatellite(satellite_used, satellite_visible, used_prn, sat_info);
}

void LocationDaemonPositionResolver::onProviderPositionUpdated()
{
    if (!m_pluginManager->bestProvider()) {
        return;
    }

    QGeoCoordinate coord = m_pluginManager->bestProvider()->coordinate();
    if (coord.isValid() && coord != m_lastCoord) {
        m_lastAccuracy = m_pluginManager->bestProvider()->accuracy();
        m_lastCoord = coord;
        m_lastUpdate = m_pluginManager->bestProvider()->lastUpdate();

               // TODO fix me!
        int feelds = Latitude | Longitude | Altitude;
        emit PositionChanged(feelds, m_lastUpdate.currentSecsSinceEpoch(), coord.latitude(), coord.longitude(), coord.altitude(), m_lastAccuracy);

        int vfields = 1 | 2; // speed | direction
        emit VelocityChanged(vfields, m_lastUpdate.currentSecsSinceEpoch(), m_pluginManager->bestProvider()->speed(), m_pluginManager->bestProvider()->direction(), 0.0);
    }

           //TODO need check for update
    if(m_pluginManager->bestProvider()->satellites().satellitesView.count() > 1) {

        int satellite_visible = m_pluginManager->bestProvider()->satellites().satellitesView.count();
        int satellite_used = m_pluginManager->bestProvider()->satellites().satellitesUsed.count();
        QList<QGeoSatelliteInfo> sat_info = m_pluginManager->bestProvider()->satellites().satellitesView;
        QList<int> used_prn;

        foreach (QGeoSatelliteInfo s, m_pluginManager->bestProvider()->satellites().satellitesUsed) {
            used_prn.push_back(s.satelliteIdentifier());
        }

        emit SatelliteChanged(m_lastUpdate.currentSecsSinceEpoch(), satellite_used, satellite_visible, used_prn, sat_info);
    }
}

void LocationDaemonPositionResolver::onBestProviderChanged()
{
    QString name = "NemoMobile location daemon";
    QString description = "NemoMobile Location provider";
    QString service = "org.freedesktop.Geoclue.Master";
    QString path = m_objectPath;

    emit PositionProviderChanged(name, description, service, path);
    onProviderPositionUpdated();
}
