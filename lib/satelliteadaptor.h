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

#ifndef SATELLITE_ADAPTOR_H
#define SATELLITE_ADAPTOR_H

#include "geoclue1service.h"
#include "satinfofull.h"
#include <QtCore/QObject>
#include <QtDBus/QtDBus>

using UsedPRN = QList<int>;
using UsedSat = QList<SatInfoFull>;

class SatelliteAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Geoclue.Satellite")
    Q_CLASSINFO("D-Bus Introspection", ""
                                       "  <interface name=\"org.freedesktop.Geoclue.Satellite\">\n"
                                       "    <method name=\"GetSatellite\">\n"
                                       "      <arg direction=\"out\" type=\"i\" name=\"timestamp\"/>\n"
                                       "      <arg direction=\"out\" type=\"i\" name=\"satellite_used\"/>\n"
                                       "      <arg direction=\"out\" type=\"i\" name=\"satellite_visible\"/>\n"
                                       "      <arg direction=\"out\" type=\"ai\" name=\"used_prn\"/>\n"
                                       "      <arg direction=\"out\" type=\"a(iiii)\" name=\"sat_info\"/>\n"
                                       "    </method>\n"
                                       "    <method name=\"GetLastSatellite\">\n"
                                       "      <arg direction=\"out\" type=\"i\" name=\"timestamp\"/>\n"
                                       "      <arg direction=\"out\" type=\"i\" name=\"satellite_used\"/>\n"
                                       "      <arg direction=\"out\" type=\"i\" name=\"satellite_visible\"/>\n"
                                       "      <arg direction=\"out\" type=\"ai\" name=\"used_prn\"/>\n"
                                       "      <arg direction=\"out\" type=\"a(iiii)\" name=\"sat_info\"/>\n"
                                       "    </method>\n"
                                       "    <signal name=\"SatelliteChanged\">\n"
                                       "      <arg type=\"i\" name=\"timestamp\"/>\n"
                                       "      <arg type=\"i\" name=\"satellite_used\"/>\n"
                                       "      <arg type=\"i\" name=\"satellite_visible\"/>\n"
                                       "      <arg type=\"ai\" name=\"used_prn\"/>\n"
                                       "      <arg type=\"a(iiii)\" name=\"sat_info\"/>\n"
                                       "    </signal>\n"
                                       "  </interface>\n"
                                       "")
public:
    explicit SatelliteAdaptor(GeoClue1Service* service);

public slots:
    int GetSatellite(int& timestamp, int& satellite_used, int& satellite_visible,
        UsedPRN& used_prn, UsedSat& sat_info);
    int GetLastSatellite(int& timestamp, int& satellite_used, int& satellite_visible,
        UsedPRN& used_prn, UsedSat& sat_info);

Q_SIGNALS:
    void SatelliteChanged(int timestamp, int satellite_used, int satellite_visible,
        const UsedPRN& used_prn, const UsedSat& sat_info);

private:
    GeoClue1Service* m_service;
};

#endif
