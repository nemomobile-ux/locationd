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

#include "nmeaparser.h"
#include <QDebug>
#include <QStringList>

bool NMEAParser::parseNMEA(const QByteArray& line)
{
    if (line.isEmpty() || line[0] != '$')
        return false;

    QString str(line);
    str = str.trimmed();
    QStringList parts = str.split(',');

    if (parts.isEmpty())
        return false;

    QString type = parts[0];
    if (type == "$GPRMC") {
        if (parts.size() < 10)
            return false;

        if (parts[2] != "A") // Validity
            return false;

        // Latitude
        double latDeg = parts[3].left(2).toDouble();
        double latMin = parts[3].mid(2).toDouble();
        m_coordinate.setLatitude(latDeg + latMin / 60.0);
        if (parts[4] == "S")
            m_coordinate.setLatitude(-m_coordinate.altitude());

        // Longitude
        double lonDeg = parts[5].left(3).toDouble();
        double lonMin = parts[5].mid(3).toDouble();
        m_coordinate.setLongitude(lonDeg + lonMin / 60.0);
        if (parts[6] == "W")
            m_coordinate.setLongitude(-m_coordinate.longitude());

        // Speed and course
        m_speed = parts[7].toDouble() * 0.514444; // knots -> m/s
        m_dir = parts[8].toDouble();

        m_accuracy.setLevel(Accuracy::Level::Detailed);
        m_accuracy.setHorizontal(5.0);
        m_accuracy.setVertical(10.0);

        return true;
    } else if (type == "$GPGGA") {
        if (parts.size() < 10)
            return false;

        m_coordinate.setAltitude(parts[9].toDouble()); // Altitude in meters

        return true;
    } else if (type == "$GPGSV") {
        // Satellites in view
        // FORMAT: $GPGSV,total_msgs,msg_num,num_sv,PRN1,ele1,az1,SNR1,...*hh
        int num_sv = parts[3].toInt();

        int startIndex = 4;
        for (int i = 0; i < num_sv; ++i) {
            if (startIndex + 3 >= parts.size())
                break;

            QGeoSatelliteInfo sat;
            // FIX ME!
            /*sat.prn = parts[startIndex].toInt();
            sat.elevation = parts[startIndex + 1].toInt();
            sat.azimuth = parts[startIndex + 2].toInt();
            sat.snr = parts[startIndex + 3].toInt();
            sat.used = (sat.snr > 0);*/

            m_sats.append(sat);

            startIndex += 4;
        }
        return true;
    }

    return false;
}
