/*
 * Copyright (C) 2025-2026 Chupligin Sergey <neochapay@gmail.com>
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

#ifndef GPSPLUGIN_H
#define GPSPLUGIN_H

#include "ilocationprovider.h"
#include "nmeaparser.h"
#include <QObject>
#include <QTimer>
#include <QVector>

class GPSProvider : public ILocationProvider {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.nemomobile.locationd.ILocationProvider")
    Q_INTERFACES(ILocationProvider)

public:
    explicit GPSProvider(QObject* parent = nullptr);
    ~GPSProvider() override;

    void setActive(bool active) override;
    bool isAvailable() const override { return m_fd >= 0; }
    int priority() const override { return 100; }

    QGeoCoordinate coordinate() const override { return m_coordinate; }

    double speed() const override { return m_speed; }
    double direction() const override { return m_direction; }

    Accuracy accuracy() const override { return m_accuracy; }
    SatellitesInfo satellites() const override { return m_satellites; }
    QDateTime lastUpdate() const override { return m_lastUpdate; }

public slots:
    void requestLocationUpdate() override;

private:
    void readData();

private:
    int m_fd = -1;
    QByteArray m_buffer;

    QGeoCoordinate m_coordinate;
    double m_speed = 0.0;
    double m_direction = 0.0;
    Accuracy m_accuracy;
    SatellitesInfo m_satellites;
    QDateTime m_lastUpdate;

    NMEAParser m_parser;
};

#endif // GPSPLUGIN_H
