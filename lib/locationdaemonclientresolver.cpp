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
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "geoclueadaptor.h"
#include "masterclientdaptor.h"
#include "positionadaptor.h"
#include "satelliteadaptor.h"
#include "velocityadaptor.h"

LocationDaemonClientResolver::LocationDaemonClientResolver(PluginManager* pluginManager, QObject* parent)
    : QObject { parent }
    , m_clientCount(-1)
    , m_pluginManager(pluginManager)
{
}

QDBusObjectPath LocationDaemonClientResolver::Create()
{
    // Authorization check
    if (calledFromDBus()) {
        QStringList allowed;

        // Main allowlist file
        QString allowFile = qEnvironmentVariable("LOCATIOND_ALLOWLIST", "/etc/locationd/allowlist");
        QFile f(allowFile);
        if (f.exists() && f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&f);
            while (!in.atEnd()) {
                QString line = in.readLine().trimmed();
                if (line.isEmpty() || line.startsWith('#'))
                    continue;
                allowed << line;
            }
            f.close();
        }

        // Drop-in directory: /etc/locationd/allowlist.d/*.conf
        QString allowDir = qEnvironmentVariable("LOCATIOND_ALLOWLIST_DIR", "/etc/locationd/allowlist.d");
        QDir dir(allowDir);
        if (dir.exists()) {
            const QStringList files = dir.entryList(QStringList() << "*.conf", QDir::Files, QDir::Name);
            for (const QString& file : files) {
                QString fullPath = dir.absoluteFilePath(file);
                QFile cf(fullPath);
                bool hasEntries = false;
                if (cf.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&cf);
                    while (!in.atEnd()) {
                        QString line = in.readLine().trimmed();
                        if (line.isEmpty() || line.startsWith('#'))
                            continue;
                        allowed << line;
                        hasEntries = true;
                    }
                    cf.close();
                }
                // If the conf file is empty, treat its basename (without .conf) as allowed
                if (!hasEntries) {
                    QString base = QFileInfo(file).baseName();
                    if (!base.isEmpty())
                        allowed << base;
                }
            }
        }

        if (!allowed.isEmpty()) {
            QString caller = message().service();
            uint pid = connection().interface()->servicePid(caller);
            QString procPath = QFileInfo(QString("/proc/%1/exe").arg(pid)).symLinkTarget();
            QString procName = QFileInfo(procPath).fileName();

            bool ok = false;
            for (const QString& entry : allowed) {
                if (entry == caller || entry == procName || entry == QString::number(pid)) {
                    ok = true;
                    break;
                }
            }
            if (!ok) {
                qWarning() << "Location access denied for" << caller << procName;
                return QDBusObjectPath();
            }
        }
    }

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
