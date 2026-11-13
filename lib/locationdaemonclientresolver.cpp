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
#include <QDBusInterface>
#include <QDBusReply>
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

void LocationDaemonClientResolver::RegisterAuthorizationAgent(const QString& service, const QString& path)
{
    m_agentService = service;
    m_agentPath = path;
    qInfo() << "Authorization agent registered:" << service << path;
}

void LocationDaemonClientResolver::UnregisterAuthorizationAgent()
{
    m_agentService.clear();
    m_agentPath.clear();
}

QDBusObjectPath LocationDaemonClientResolver::Create()
{
    // Authorization check
    if (calledFromDBus()) {
        QString caller = message().service();
        uint pid = connection().interface()->servicePid(caller);
        QString procPath = QFileInfo(QString("/proc/%1/exe").arg(pid)).symLinkTarget();
        QString procName = QFileInfo(procPath).fileName();

        QStringList allowed;
        QStringList denied;

        auto loadList = [&](const QString& file, QStringList& target) {
            QFile f(file);
            if (f.exists() && f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&f);
                while (!in.atEnd()) {
                    QString line = in.readLine().trimmed();
                    if (line.isEmpty() || line.startsWith('#'))
                        continue;
                    target << line;
                }
                f.close();
            }
        };
        auto loadDir = [&](const QString& dirPath, QStringList& target, bool allowEmptyBasename) {
            QDir dir(dirPath);
            if (!dir.exists())
                return;
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
                        target << line;
                        hasEntries = true;
                    }
                    cf.close();
                }
                if (allowEmptyBasename && !hasEntries) {
                    QString base = QFileInfo(file).baseName();
                    if (!base.isEmpty())
                        target << base;
                }
            }
        };

        // Allowed
        loadList(qEnvironmentVariable("LOCATIOND_ALLOWLIST", "/etc/locationd/allowlist"), allowed);
        loadDir(qEnvironmentVariable("LOCATIOND_ALLOWLIST_DIR", "/etc/locationd/allowlist.d"), allowed, true);

        // Denied
        loadList(qEnvironmentVariable("LOCATIOND_DENYLIST", "/etc/locationd/denylist"), denied);
        loadDir(qEnvironmentVariable("LOCATIOND_DENYLIST_DIR", "/etc/locationd/denylist.d"), denied, true);

        auto matches = [&](const QStringList& list) {
            for (const QString& entry : list) {
                if (entry == caller || entry == procName || entry == QString::number(pid))
                    return true;
            }
            return false;
        };

        // Explicit deny takes precedence
        if (matches(denied)) {
            qWarning() << "Location access denied for" << caller << procName;
            return QDBusObjectPath("/org/freedesktop/Geoclue/Master/denied");
        }

        // Explicit allow
        if (matches(allowed)) {
            // proceed
        } else if (!allowed.isEmpty() || !denied.isEmpty()) {
            // There are rules but app not explicitly allowed -> consult agent if available
            bool decided = false;
            bool allow = false;
            bool always = false;

            if (!m_agentService.isEmpty() && !m_agentPath.isEmpty()) {
                // Use a separate connection to avoid deadlocking the session bus
                // while handling the outer Create() call.
                QDBusConnection authBus = QDBusConnection::connectToBus(QDBusConnection::SessionBus, "locationd-auth");
                QDBusInterface agent(m_agentService, m_agentPath, "org.freedesktop.Geoclue.Master.AuthorizationAgent", authBus);
                QDBusReply<int> reply = agent.call("RequestAuthorization", procName, pid, procPath);
                if (reply.isValid()) {
                    int choice = reply.value();
                    if (choice == 1) { // Allow always
                        allow = true;
                        always = true;
                    } else if (choice == 0) { // Allow once
                        allow = true;
                        always = false;
                    } else { // Disallow
                        allow = false;
                        always = false;
                    }
                    decided = true;
                } else {
                    qWarning() << "Agent call failed:" << reply.error().message();
                }
                QDBusConnection::disconnectFromBus("locationd-auth");
            }

            if (!decided) {
                // No agent or no response: default allow to preserve functionality
                allow = true;
                always = false;
            }

            if (!allow) {
                // Persist deny if "always" disallow? Store in denylist.d
                if (always == false && false) {
                }
                // For disallow, persist deny rule
                QString denyDir = qEnvironmentVariable("LOCATIOND_DENYLIST_DIR", "/etc/locationd/denylist.d");
                QDir().mkpath(denyDir);
                QFile df(denyDir + "/" + procName + ".conf");
                if (df.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    df.write((procName + "\n").toUtf8());
                    df.close();
                }
                qWarning() << "Location access denied for" << caller << procName;
                return QDBusObjectPath("/org/freedesktop/Geoclue/Master/denied");
            }

            if (always) {
                // Persist allow rule
                QString allowDir2 = qEnvironmentVariable("LOCATIOND_ALLOWLIST_DIR", "/etc/locationd/allowlist.d");
                QDir().mkpath(allowDir2);
                QFile af(allowDir2 + "/" + procName + ".conf");
                if (af.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    af.write((procName + "\n").toUtf8());
                    af.close();
                }
            }
        } else {
            // No rules at all: default allow
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
