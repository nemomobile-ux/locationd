/*
 * Copyright (C) 2026 Chupligin Sergey <neochapay@gmail.com>
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
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

#include "authorizationagent.h"

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("locationd-agent");

    AuthorizationAgent agent;
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.registerObject(QStringLiteral("/org/nemomobile/location/agent"),
                            QStringLiteral("org.freedesktop.Geoclue.Master.AuthorizationAgent"),
                            &agent, QDBusConnection::ExportAllSlots)) {
        qCritical() << "Failed to register agent object:" << bus.lastError().message();
        return 1;
    }
    if (!bus.registerService(QStringLiteral("org.nemomobile.location.agent"))) {
        qCritical() << "Failed to register agent service:" << bus.lastError().message();
        return 1;
    }

    // Register with master
    QDBusInterface master(QStringLiteral("org.freedesktop.Geoclue.Master"),
                          QStringLiteral("/org/freedesktop/Geoclue/Master"),
                          QStringLiteral("org.freedesktop.Geoclue.Master"), bus);
    QDBusReply<void> reply = master.call(QStringLiteral("RegisterAuthorizationAgent"),
                                         QStringLiteral("org.nemomobile.location.agent"),
                                         QStringLiteral("/org/nemomobile/location/agent"));
    if (!reply.isValid()) {
        qWarning() << "Failed to register with master:" << reply.error().message();
    } else {
        qInfo() << "Registered with location master";
    }

    return app.exec();
}