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

    if (!checkNMEAChecksum(line))
        return false;

    QString str = QString::fromLatin1(line).trimmed();
    int asterisk = str.indexOf('*');
    if (asterisk > 0)
        str = str.left(asterisk);

    QStringList parts = str.split(',');
    if (parts.isEmpty())
        return false;

    QString type = sentenceType(parts[0]);

    if (type == "GSV") {
        parseGSV(parts);
        return true;
    }

    if (type == "GSA") {
        parseGSA(parts);
        return true;
    }

    /* ======================= RMC ======================= */
    if (type == "RMC") {
        if (parts.size() < 9 || parts[2] != "A")
            return false;

        if (parts[3].isEmpty() || parts[5].isEmpty())
            return false;

               // Latitude
        double latDeg = parts[3].left(2).toDouble();
        double latMin = parts[3].mid(2).toDouble();
        double lat = latDeg + latMin / 60.0;
        if (parts[4] == "S")
            lat = -lat;
        m_coordinate.setLatitude(lat);

               // Longitude
        double lonDeg = parts[5].left(3).toDouble();
        double lonMin = parts[5].mid(3).toDouble();
        double lon = lonDeg + lonMin / 60.0;
        if (parts[6] == "W")
            lon = -lon;
        m_coordinate.setLongitude(lon);

        m_speed = parts[7].toDouble() * 0.514444; // knots → m/s
        m_dir   = parts[8].toDouble();

        m_accuracy.setLevel(Accuracy::Level::Detailed);
        return true;
    }

    /* ======================= GGA ======================= */
    if (type == "GGA") {
        if (parts.size() >= 10 && !parts[9].isEmpty())
            m_coordinate.setAltitude(parts[9].toDouble());

        return true;
    }

    /* ======================= GSV ======================= */
    if (type == "GSV") {
        parseGSV(parts);
        return true;
    }

    return false;
}

bool NMEAParser::checkNMEAChecksum(const QByteArray &line)
{
    int asterisk = line.indexOf('*');
    if (asterisk < 0)
        return false;

    QByteArray data = line.mid(1, asterisk - 1);
    QByteArray checksumStr = line.mid(asterisk + 1, 2);

    bool ok = false;
    int expected = checksumStr.toInt(&ok, 16);
    if (!ok)
        return false;

    quint8 crc = 0;
    for (char c : data)
        crc ^= static_cast<quint8>(c);

    return crc == expected;
}

void NMEAParser::parseGSV(const QStringList& parts)
{
    if (parts.size() < 4)
        return;

    int msgNum = parts[2].toInt();

    if (msgNum == 1)
        m_satsInView.clear();

    int satsInMsg = (parts.size() - 4) / 4;
    int index = 4;

    for (int i = 0; i < satsInMsg; ++i) {
        if (index + 3 >= parts.size())
            break;

        if (parts[index].isEmpty())
            break;

        QGeoSatelliteInfo sat;
        int prn = parts[index].toInt();
        sat.setSatelliteIdentifier(prn);

        sat.setAttribute(QGeoSatelliteInfo::Elevation,
            parts[index + 1].toInt());
        sat.setAttribute(QGeoSatelliteInfo::Azimuth,
            parts[index + 2].toInt());

        if (!parts[index + 3].isEmpty()) {
            sat.setSignalStrength(parts[index + 3].toInt());
        }

        m_satsInView.append(sat);
        index += 4;
    }

    updateSatUseFlags();
}

void NMEAParser::parseGSA(const QStringList &parts)
{
    if (parts.size() < 15)
        return;

    m_usedPrns.clear();

    for (int i = 3; i <= 14 && i < parts.size(); ++i) {
        if (!parts[i].isEmpty())
            m_usedPrns.insert(parts[i].toInt());
    }

    updateSatUseFlags();
}

void NMEAParser::updateSatUseFlags()
{
    m_satsInUse.clear();

    for (QGeoSatelliteInfo sat : std::as_const(m_satsInView)) {
        int prn = sat.satelliteIdentifier();
        if (m_usedPrns.contains(prn))
            m_satsInUse.append(sat);
    }
}


QString NMEAParser::sentenceType(const QString &header)
{
    // "$GPGSV" → "GSV"
    if (header.length() >= 6 && header.startsWith('$'))
        return header.mid(3, 3);
    return QString();
}
