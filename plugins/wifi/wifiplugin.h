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

#ifndef WIFIPLUGIN_H
#define WIFIPLUGIN_H

#include "ilocationprovider.h"
#include <QObject>
#include <QTimer>
#include <QVector>

struct WifiAccessPoint {
    QString macAddress;
    int signalStrength; // dBm
    int channel;
};

class WifiProvider : public ILocationProvider {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.nemomobile.locationd.ILocationProvider")
    Q_INTERFACES(ILocationProvider)
public:
    explicit WifiProvider(QObject* parent = nullptr);
    ~WifiProvider() override;

    bool isAvailable() const override { return m_available; }
    int priority() const override { return 60; }

    double latitude() const override { return m_latitude; }
    double longitude() const override { return m_longitude; }
    double altitude() const override { return m_altitude; }

    double speed() const override { return m_speed; }
    double direction() const override { return m_direction; }

    Accuracy accuracy() const override { return m_accuracy; }
    double accuracyValue() const override { return m_accuracy.horizontal; }

    QVector<SatInfoFull> satellites() const override { return {}; }

public slots:
    void requestLocationUpdate() override;

private:
    void fetchWifiData();
    void queryMLS();
    void handleMLSResponse(double lat, double lon, int accuracy);

private:
    bool m_available = false;
    double m_latitude = 0.0;
    double m_longitude = 0.0;
    double m_altitude = 0.0;
    double m_speed = 0.0;
    double m_direction = 0.0;
    Accuracy m_accuracy;

    QVector<WifiAccessPoint> m_accessPoints;
    QTimer m_updateTimer;
};

#endif // WIFIPLUGIN_H
