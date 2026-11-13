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

#include "locationdaemonclientresolver.h"
#include "addressadaptor.h"
#include "geoclueadaptor.h"
#include "masterclientdaptor.h"
#include "positionadaptor.h"
#include "satelliteadaptor.h"
#include "velocityadaptor.h"

LocationDaemonClientResolver::LocationDaemonClientResolver(QObject* parent)
    : QObject { parent }
    , m_clientCount(-1)
{
    m_pluginManager = new PluginManager();
    QString pluginPath = qEnvironmentVariable("LOCATIOND_PLUGIN_PATH", "/usr/lib/locationd/plugins/");
    m_pluginManager->loadPlugins(pluginPath);
}

QDBusObjectPath LocationDaemonClientResolver::Create()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QString path = QString("/org/freedesktop/Geoclue/Master/client%1").arg(++m_clientCount);

    LocationDaemonPositionResolver* resolver = new LocationDaemonPositionResolver(m_pluginManager, path, this);

    GeoclueAdaptor* geoclueMain = new GeoclueAdaptor(resolver);
    MasterClientAdaptor* masterClient = new MasterClientAdaptor(resolver);
    PositionAdaptor* positon = new PositionAdaptor(resolver);
    AddressAdaptor* address = new AddressAdaptor(resolver);
    SatelliteAdaptor* satelittes = new SatelliteAdaptor(resolver);
    VelocityAdaptor* velocity = new VelocityAdaptor(resolver);

    if (!bus.registerObject(
            path,
            resolver,
            QDBusConnection::ExportAdaptors)) {
        qWarning() << "Failed to register client object:" << bus.lastError().message();
    }

    return QDBusObjectPath(path);
}
