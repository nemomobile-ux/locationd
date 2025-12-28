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

#ifndef POSITION_ADAPTOR_H
#define POSITION_ADAPTOR_H

#include "accuracy.h"
#include "geoclue1service.h"
#include <QtCore/QObject>
#include <QtDBus/QtDBus>

class PositionAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Geoclue.Position")
    Q_CLASSINFO("D-Bus Introspection", ""
                                       "  <interface name=\"org.freedesktop.Geoclue.Position\">\n"
                                       "    <method name=\"GetPosition\">\n"
                                       "      <arg direction=\"out\" type=\"i\" name=\"fields\"/>\n"
                                       "      <arg direction=\"out\" type=\"i\" name=\"timestamp\"/>\n"
                                       "      <arg direction=\"out\" type=\"d\" name=\"latitude\"/>\n"
                                       "      <arg direction=\"out\" type=\"d\" name=\"longitude\"/>\n"
                                       "      <arg direction=\"out\" type=\"d\" name=\"altitude\"/>\n"
                                       "      <arg direction=\"out\" type=\"(idd)\" name=\"accuracy\"/>\n"
                                       "    </method>\n"
                                       "    <signal name=\"PositionChanged\">\n"
                                       "      <arg type=\"i\" name=\"fields\"/>\n"
                                       "      <arg type=\"i\" name=\"timestamp\"/>\n"
                                       "      <arg type=\"d\" name=\"latitude\"/>\n"
                                       "      <arg type=\"d\" name=\"longitude\"/>\n"
                                       "      <arg type=\"d\" name=\"altitude\"/>\n"
                                       "      <arg type=\"(idd)\" name=\"accuracy\"/>\n"
                                       "    </signal>\n"
                                       "  </interface>\n"
                                       "")
public:
    explicit PositionAdaptor(GeoClue1Service* service);

public slots:
    int GetPosition(int& timestamp, double& latitude, double& longitude, double& altitude, Accuracy& accuracy);

signals:
    void PositionChanged(int fields, int timestamp, double latitude, double longitude, double altitude, Accuracy accuracy);

private:
    GeoClue1Service* m_service;
};
#endif
