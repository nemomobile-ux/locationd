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

#ifndef CELLPROVIDER_H
#define CELLPROVIDER_H

#include "ilocationprovider.h"
#include <QObject>
#include <QTimer>
#include <QVector>

struct CellTower {
    int cellId;
    int locationAreaCode;
    int mobileCountryCode;
    int mobileNetworkCode;
    int signalStrength;
};

class CellProvider : public ILocationProvider {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.nemomobile.locationd.ILocationProvider")
    Q_INTERFACES(ILocationProvider)
public:
    explicit CellProvider(QObject* parent = nullptr);
    ~CellProvider() override;

    void setActive(bool active) override;

    bool isAvailable() const override { return m_available; }
    int priority() const override { return 50; }

    QGeoCoordinate coordinate() const override { return m_coordinate; }
    double speed() const override { return m_speed; }
    double direction() const override { return m_direction; }

    Accuracy accuracy() const override { return m_accuracy; }

    SatellitesInfo satellites() const override { return SatellitesInfo(); }
    QDateTime lastUpdate() const override { return m_lastUpdate; }

public slots:
    void requestLocationUpdate() override;

private:
    void fetchCellData();
    void queryMLS();
    void handleMLSResponse(double lat, double lon, int accuracy);

private:
    bool m_available = false;
    QGeoCoordinate m_coordinate;
    double m_speed = 0.0;
    double m_direction = 0.0;
    Accuracy m_accuracy;

    QVector<CellTower> m_cells;
    QDateTime m_lastUpdate;
};

#endif // CELLPROVIDER_H
