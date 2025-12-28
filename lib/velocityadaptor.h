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

#ifndef VELOCITY_ADAPTOR_H
#define VELOCITY_ADAPTOR_H

#include "geoclue1service.h"
#include <QtCore/QObject>
#include <QtDBus/QtDBus>

class VelocityAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Geoclue.Velocity")
    Q_CLASSINFO("D-Bus Introspection", ""
                                       "  <interface name=\"org.freedesktop.Geoclue.Velocity\">\n"
                                       "    <method name=\"GetVelocity\">\n"
                                       "      <arg direction=\"out\" type=\"i\" name=\"fields\"/>\n"
                                       "      <arg direction=\"out\" type=\"i\" name=\"timestamp\"/>\n"
                                       "      <arg direction=\"out\" type=\"d\" name=\"speed\"/>\n"
                                       "      <arg direction=\"out\" type=\"d\" name=\"direction\"/>\n"
                                       "      <arg direction=\"out\" type=\"d\" name=\"climb\"/>\n"
                                       "    </method>\n"
                                       "    <signal name=\"VelocityChanged\">\n"
                                       "      <arg type=\"i\" name=\"fields\"/>\n"
                                       "      <arg type=\"i\" name=\"timestamp\"/>\n"
                                       "      <arg type=\"d\" name=\"speed\"/>\n"
                                       "      <arg type=\"d\" name=\"direction\"/>\n"
                                       "      <arg type=\"d\" name=\"climb\"/>\n"
                                       "    </signal>\n"
                                       "  </interface>\n"
                                       "")
public:
    explicit VelocityAdaptor(GeoClue1Service* service);

public Q_SLOTS:
    int GetVelocity(int& fields, int& timestamp, double& speed, double& direction, double& climb);

Q_SIGNALS:
    void VelocityChanged(int fields, int timestamp, double speed, double direction, double climb);

private:
    GeoClue1Service* m_service;
};

#endif
