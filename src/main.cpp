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

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

#include "geoclue1service.h"
#include "pluginmanager.h"

#include "positionadaptor.h"
#include "satelliteadaptor.h"
#include "velocityadaptor.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("locationd");
    app.setOrganizationName("nemomobile");

    QDBusConnection bus = QDBusConnection::systemBus();
    if (!bus.isConnected()) {
        qCritical() << "Cannot connect to system D-Bus:"
                    << bus.lastError().message();
        return 1;
    }

    if (!bus.registerService("org.freedesktop.Geoclue")) {
        qCritical() << "Cannot register D-Bus service:"
                    << bus.lastError().message();
        return 1;
    }

    PluginManager pluginManager;
    pluginManager.loadPlugins("/usr/lib/locationd/plugins");

    GeoClue1Service geoclue(pluginManager.bestProvider());

    PositionAdaptor positionAdaptor(&geoclue);
    SatelliteAdaptor satelliteAdaptor(&geoclue);
    VelocityAdaptor velocityAdaptor(&geoclue);

    if (!bus.registerObject("/org/freedesktop/Geoclue",
            &geoclue,
            QDBusConnection::ExportAdaptors)) {
        qCritical() << "Cannot register D-Bus object:"
                    << bus.lastError().message();
        return 1;
    }

    qInfo() << "locationd started (GeoClue v1 compatible)";

    return app.exec();
}
