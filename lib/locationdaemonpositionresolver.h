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

#ifndef LOCATIONDAEMONPOSITIONRESOLVER_H
#define LOCATIONDAEMONPOSITIONRESOLVER_H

#include "geocluetypes.h"
#include "pluginmanager.h"
#include <QObject>
#include <QTimer>

#define GEOCLUE_RESOURCE_GPS (1 << 0) // 0001
#define GEOCLUE_RESOURCE_WIFI (1 << 1) // 0010
#define GEOCLUE_RESOURCE_CELL (1 << 2) // 0100
#define GEOCLUE_RESOURCE_OTHER (1 << 3) // 1000

class LocationDaemonPositionResolver : public QObject {
    Q_OBJECT
public:
    enum PositionField {
        NoPositionFields = 0,
        Latitude = 1 << 0, // 1
        Longitude = 1 << 1, // 2
        Altitude = 1 << 2 // 4
    };

    explicit LocationDaemonPositionResolver(PluginManager* manager, QObject* parent = nullptr);

public slots:
    // org.freedesktop.Geoclue
    void AddReference();
    void RemoveReference();
    QString GetProviderInfo(QString& Description);
    int GetStatus();
    void SetOptions(const QVariantMap& options);

    // org.freedesktop.Geoclue.MasterClient
    void AddressStart();
    QString GetAddressProvider(QString& description, QString& service, QString& path);
    QString GetPositionProvider(QString& description, QString& service, QString& path);

    void PositionStart();
    void SetRequirements(int accuracy_level, int time, bool require_updates, int allowed_resources);

    // org.freedesktop.Geoclue.Position
    int GetPosition(int& timestamp, double& latitude, double& longitude, double& altitude, Accuracy& accuracy);

    // org.freedesktop.Geoclue.Address
    int GetAddress(QVariantMap& address, Accuracy& accuracy);

private slots:
    void onProviderPositionUpdated();
    void onBestProviderChanged();

Q_SIGNALS:
    void PositionProviderChanged(const QString& name, const QString& description, const QString& service, const QString& path);

    void PositionChanged(int fields, int timestamp, double latitude, double longitude, double altitude, Accuracy accuracy);

private:
    int m_refCount;
    QDateTime m_lastUpdate;
    QTimer* m_updateTimer;
    PluginManager* m_pluginManager;
    QGeoCoordinate m_lastCoord;
    Accuracy m_lastAccuracy;
    QVariantMap m_option;
};

#endif // LOCATIONDAEMONPOSITIONRESOLVER_H
