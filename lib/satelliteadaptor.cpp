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

#include "satelliteadaptor.h"

SatelliteAdaptor::SatelliteAdaptor(GeoClue1Service* service)
    : QDBusAbstractAdaptor(service)
    , m_service(service)
{
    setAutoRelaySignals(true);
}

int SatelliteAdaptor::GetSatellite(int& timestamp, int& satellite_used, int& satellite_visible,
    UsedPRN& used_prn, UsedSat& sat_info)
{
    auto sats = m_service->GetSatellites();
    timestamp = QDateTime::currentSecsSinceEpoch();
    satellite_visible = sats.size();
    satellite_used = 0;
    used_prn.clear();
    sat_info.clear();

    for (const auto& sat : sats) {
        used_prn.append(sat.prn);
        sat_info.append({ sat.azimuth, sat.elevation, sat.snr, sat.used ? 1 : 0 });
        if (sat.used)
            satellite_used++;
    }
    return 0;
}

int SatelliteAdaptor::GetLastSatellite(int& timestamp, int& satellite_used, int& satellite_visible,
    UsedPRN& used_prn, UsedSat& sat_info)
{
    auto sats = m_service->GetSatellites();
    if (!sats.isEmpty()) {
        const auto& sat = sats.last();
        timestamp = QDateTime::currentSecsSinceEpoch();
        satellite_visible = 1;
        satellite_used = sat.used ? 1 : 0;
        used_prn = { sat.prn };
        sat_info = { { sat.azimuth, sat.elevation, sat.snr, sat.used ? 1 : 0 } };
    } else {
        timestamp = 0;
        satellite_visible = 0;
        satellite_used = 0;
        used_prn.clear();
        sat_info.clear();
    }
    return 0;
}
