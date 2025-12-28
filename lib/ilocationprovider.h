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

#ifndef ILOCATIONPROVIDER_H
#define ILOCATIONPROVIDER_H

#include <QGeoSatelliteInfo>
#include <QObject>
#include <QVector>

#include "accuracy.h"
#include "satinfofull.h"

/*
 * GeoClue v1 accuracy levels
 */
enum class AccuracyLevel : int {
    NONE = 0,
    COUNTRY,
    REGION,
    CITY,
    NEIGHBORHOOD,
    STREET,
    EXACT
};

class ILocationProvider : public QObject {
    Q_OBJECT
public:
    explicit ILocationProvider(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
    virtual ~ILocationProvider() { }

    virtual bool isAvailable() const = 0;
    virtual int priority() const = 0;

    virtual double latitude() const = 0;
    virtual double longitude() const = 0;
    virtual double altitude() const = 0;

    virtual double speed() const = 0;
    virtual double direction() const = 0;

    virtual Accuracy accuracy() const = 0;
    virtual double accuracyValue() const = 0;

    virtual QVector<SatInfoFull> satellites() const = 0;

public slots:
    virtual void requestLocationUpdate() { }

signals:
    void updated();
};

#define ILocationProvider_iid "org.nemomobile.locationd.ILocationProvider"
Q_DECLARE_INTERFACE(ILocationProvider, ILocationProvider_iid)

#endif // ILOCATIONPROVIDER_H
