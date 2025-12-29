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

#include "ilocationprovider.h"
#include <QObject>

class GeoClue1Service : public QObject {
    Q_OBJECT
public:
    explicit GeoClue1Service(ILocationProvider* provider, QObject* parent = nullptr);
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

private:
    ILocationProvider* m_provider;
    QList<QGeoSatelliteInfo> m_lastSatellites;
};

#endif // GEOCLUE1SERVICE_H
