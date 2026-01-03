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

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

#include "locationdaemonclientresolver.h"
#include "masteradaptor.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("geoclue-master");
    app.setOrganizationName("nemomobile");

    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        qCritical() << "Cannot connect to system D-Bus:" << bus.lastError().message();
        return 1;
    }

    if (!bus.registerService("org.freedesktop.Geoclue.Master")) {
        qCritical() << "Cannot register D-Bus service:" << bus.lastError().message();
        return 1;
    }

    // Master service
    LocationDaemonClientResolver* clientService = new LocationDaemonClientResolver();
    MasterAdaptor masterAdaptor(clientService);

    if (!bus.registerObject(
            QStringLiteral("/org/freedesktop/Geoclue/Master"),
            clientService,
            QDBusConnection::ExportAdaptors)) {
        qCritical() << "Failed to register Master object:" << bus.lastError().message();
        return 1;
    }

    qInfo() << "GeoClue Master service started";

    return app.exec();
}
