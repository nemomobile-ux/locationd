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

#ifndef SATINFOFULL_H
#define SATINFOFULL_H

#include <QtDBus/QtDBus>

struct SatInfoFull {
    int prn;
    int azimuth;
    int elevation;
    int snr;
    bool used;

    bool operator==(const SatInfoFull& other) const
    {
        return prn == other.prn && azimuth == other.azimuth && elevation == other.elevation && snr == other.snr;
    }

    bool operator!=(const SatInfoFull& other) const
    {
        return !(*this == other);
    }
};

Q_DECLARE_METATYPE(SatInfoFull)

QDBusArgument& operator<<(QDBusArgument& arg, const SatInfoFull& s);
const QDBusArgument& operator>>(const QDBusArgument& arg, SatInfoFull& s);

typedef QVector<int> UsedPRN;
Q_DECLARE_METATYPE(UsedPRN)

typedef QVector<SatInfoFull> UsedSat;
Q_DECLARE_METATYPE(UsedSat)

#endif // SATINFOFULL_H
