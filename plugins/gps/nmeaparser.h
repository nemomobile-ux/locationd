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

#ifndef NMEA_PARSER_H
#define NMEA_PARSER_H

#include <QGeoCoordinate>
#include <QVector>
#include <geocluetypes.h>

class NMEAParser {
public:
    bool parseNMEA(const QByteArray& line);

    QGeoCoordinate coordinate() const { return m_coordinate; }
    double speed() const { return m_speed; }
    double direction() const { return m_dir; }

    Accuracy accuracy() const { return m_accuracy; }
    QVector<QGeoSatelliteInfo> satellites() const { return m_sats; }

private:
    QGeoCoordinate m_coordinate;
    double m_speed = 0.0;
    double m_dir = 0.0;
    Accuracy m_accuracy;
    QVector<QGeoSatelliteInfo> m_sats;
};

#endif // NMEA_PARSER_H
