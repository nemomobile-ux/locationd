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

#ifndef GEOCLUE1SERVICE_H
#define GEOCLUE1SERVICE_H

#include "pluginmanager.h"
#include "ilocationprovider.h"
#include <QObject>

class GeoClue1Service : public QObject {
    Q_OBJECT
public:
    enum class PositionState {
        Stopped,
        Running
    };

    explicit GeoClue1Service(QObject* parent = nullptr);

public slots:
    int GetPosition(int& timestamp, double& latitude, double& longitude, double& altitude, Accuracy& accuracy);    
    QVector<SatInfoFull> GetSatellites();
    int GetLastSatellite(int &satellite_used, int &satellite_visible, UsedPRN &used_prn, UsedSat &sat_info);
    int GetSatellite(int &satellite_used, int &satellite_visible, UsedPRN &used_prn, UsedSat &sat_info);

    int GetVelocity(int& timestamp, double& speed, double& direction, double& climb);
    int GetAddress(QVariantMap& address, Accuracy& accuracy);
    void AddReference();
    QString GetProviderInfo(QString& Description);
    int GetStatus();
    void RemoveReference();
    void SetOptions(const QVariantMap &options);
    int AddressToPosition(const QVariantMap &address, double &latitude, double &longitude, double &altitude, Accuracy &accuracy);
    int FreeformAddressToPosition(const QString &address, double &latitude, double &longitude, double &altitude, Accuracy &accuracy);
    QDBusObjectPath Create();

    void AddressStart();
    QString GetAddressProvider(QString &description, QString &service, QString &path);
    QString GetPositionProvider(QString &description, QString &service, QString &path);
    void PositionStart();
    void SetRequirements(int accuracy_level, int time, bool require_updates, int allowed_resources);

    QVariantMap PositionToAddress(double latitude, double longitude, Accuracy position_accuracy, Accuracy &address_accuracy);

private slots:
    void onProviderUpdated();

signals:
    void PositionChanged(int timestamp,
        double latitude,
        double longitude,
        double altitude,
        Accuracy accuracy);
    void PositionProviderChanged(QString name,
        QString description,
        QString service,
        QString path);

private:
    PluginManager* m_pluginManager;
    ILocationProvider* m_provider;
    QList<QGeoSatelliteInfo> m_lastSatellites;
    PositionState m_positionState;
    int m_refCount;
    int m_clientCount;

    int m_lastTimestamp = 0;
    double m_lastLatitude = 0.0;
    double m_lastLongitude = 0.0;
    double m_lastAltitude = 0.0;
    Accuracy m_lastAccuracy;
};

#endif // GEOCLUE1SERVICE_H
