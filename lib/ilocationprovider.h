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

#ifndef ILOCATIONPROVIDER_H
#define ILOCATIONPROVIDER_H

#include <QDateTime>
#include <QGeoCoordinate>
#include <QGeoSatelliteInfo>
#include <QObject>
#include <QVector>

#include "geocluetypes.h"

class ILocationProvider : public QObject {
    Q_OBJECT
public:
    struct SatellitesInfo
    {
        QList<QGeoSatelliteInfo> satellitesUsed = QList<QGeoSatelliteInfo>();
        QList<QGeoSatelliteInfo> satellitesView = QList<QGeoSatelliteInfo>();
    };

    explicit ILocationProvider(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
    virtual ~ILocationProvider() { }

    virtual void setActive(bool active) = 0;

    virtual bool isAvailable() const = 0;
    virtual int priority() const = 0;

    virtual QGeoCoordinate coordinate() const = 0;

    virtual double speed() const = 0;
    virtual double direction() const = 0;

    virtual Accuracy accuracy() const = 0;

    virtual QDateTime lastUpdate() const = 0;

    virtual SatellitesInfo satellites() const = 0;

public slots:
    virtual void requestLocationUpdate() { }

signals:
    void positionUpdated();
};

#define ILocationProvider_iid "org.nemomobile.locationd.ILocationProvider"
Q_DECLARE_INTERFACE(ILocationProvider, ILocationProvider_iid)

#endif // ILOCATIONPROVIDER_H
