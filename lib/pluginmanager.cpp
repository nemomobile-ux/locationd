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

#include "pluginmanager.h"
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>

PluginManager::PluginManager(QObject* parent)
    : QObject(parent)
{
    connect(QDBusConnection::sessionBus().interface(), &QDBusConnectionInterface::serviceOwnerChanged,
        this, &PluginManager::onNameOwnerChanged);
}

void PluginManager::loadPlugins(const QString& path)
{
    QDir dir(path);
    if (!dir.exists()) {
        qWarning() << "Plugin directory does not exist:" << path;
        return;
    }

    const QStringList files = dir.entryList(QDir::Files);
    for (const QString& file : files) {
        const QString fullPath = dir.absoluteFilePath(file);
        QPluginLoader loader(fullPath);
        QObject* pluginObj = loader.instance();
        if (!pluginObj) {
            qWarning() << "Failed to load plugin:" << fullPath << loader.errorString();
            continue;
        }

        auto provider = qobject_cast<ILocationProvider*>(pluginObj);
        if (!provider) {
            qWarning() << "Plugin is not ILocationProvider:" << fullPath;
            loader.unload();
            continue;
        }

        qInfo() << "Loaded provider:" << provider->metaObject()->className();
        connect(provider, &ILocationProvider::positionUpdated, this, &PluginManager::getProviderPosition);

        m_providers.append(provider);
    }
}

void PluginManager::setActive(bool active)
{
    m_active = active;
    if (active) {
        foreach (ILocationProvider* p, m_providers) {
            p->setActive(true);
        }
        getProviderPosition();
    }
}

void PluginManager::getProviderPosition()
{
    ILocationProvider* best = nullptr;

    for (ILocationProvider* p : m_providers) {
        if (!p->isAvailable()) {
            qDebug() << p->metaObject()->className() << "is not available";
            continue;
        }
        if (!best) {
            best = p;
        } else if (p->priority() > best->priority()
            || (p->priority() == best->priority()
                && p->accuracy().horizontal() < best->accuracy().horizontal())) {
            best = p;
        }
    }

    if (!best) {
        qWarning() << "WITHOUT PROVIDERS!";
        return;
    }

    if (best != m_bestProvider) {
        m_bestProvider = best;
        emit bestProviderChanged();
    }

    if (m_bestProvider->coordinate() != m_lastCoord) {
        m_lastCoord = m_bestProvider->coordinate();
        emit providerPositionUpdated();
    }
}

void PluginManager::onNameOwnerChanged(const QString& name, const QString& oldOwner, const QString& newOwner)
{
    Q_UNUSED(oldOwner);
    if (newOwner.isEmpty()) {
        return;
    }

    uint pid = QDBusConnection::sessionBus().interface()->servicePid(newOwner);
    QString link = QString("/proc/%1/exe").arg(pid);
    qDebug() << "Connecting:" << QFileInfo(link).symLinkTarget();

    if (name.startsWith(":")) {
        if (!newOwner.isEmpty()) {
            m_clients.insert(name);
            activateProviders();
        } else {
            m_clients.remove(name);
            if (m_clients.isEmpty())
                deactivateProviders();
        }
    }
}

void PluginManager::activateProviders()
{
    if (m_active)
        return;

    qInfo() << "Activating providers";
    m_active = true;
}

void PluginManager::deactivateProviders()
{
    if (!m_active)
        return;

    qInfo() << "Deactivating providers";
    m_active = false;

    m_bestProvider = nullptr;
}
