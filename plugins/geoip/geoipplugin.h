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

#ifndef GEOIPPLUGIN_H
#define GEOIPPLUGIN_H

#include <QNetworkAccessManager>
#include <QObject>

#include "ilocationprovider.h"

class GeoIPProvider : public ILocationProvider {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.nemomobile.locationd.ILocationProvider")
    Q_INTERFACES(ILocationProvider)

public:
    explicit GeoIPProvider(QObject* parent = nullptr);
    ~GeoIPProvider() override = default;

    void setActive(bool active) override;
    bool isAvailable() const override { return true; }
    int priority() const override { return 10; }

    QGeoCoordinate coordinate() const override { return m_coordinate; }

    double speed() const override { return m_speed; }
    double direction() const override { return m_direction; }

    Accuracy accuracy() const override { return m_accuracy; }

    SatellitesInfo satellites() const override { return SatellitesInfo(); }
    QDateTime lastUpdate() const override { return m_lastUpdate; }

public slots:
    void requestLocationUpdate() override;

private slots:
    void queryGeoIP();
    void parseGeoIPJson();

private:
    QNetworkAccessManager* m_manager;

    QGeoCoordinate m_coordinate = QGeoCoordinate();

    double m_speed = 0.0;
    double m_direction = 0.0;

    QDateTime m_lastUpdate;

    Accuracy m_accuracy;
};

#endif // GEOIPPLUGIN_H
