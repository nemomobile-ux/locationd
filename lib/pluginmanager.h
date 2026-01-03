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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "ilocationprovider.h"
#include <QList>
#include <QObject>

class PluginManager : public QObject {
    Q_OBJECT
public:
    explicit PluginManager(QObject* parent = nullptr);
    void loadPlugins(const QString& path);
    ILocationProvider* bestProvider() const { return m_bestProvider; }
    void setActive(bool active);

signals:
    void bestProviderChanged();
    void providerPositionUpdated();

private slots:
    void onNameOwnerChanged(const QString& name, const QString& oldOwner, const QString& newOwner);
    void getProviderPosition();

private:
    QList<ILocationProvider*> m_providers;
    ILocationProvider* m_bestProvider = nullptr;

    QGeoCoordinate m_lastCoord;

    QSet<QString> m_clients;
    bool m_active = false;

    void activateProviders();
    void deactivateProviders();
};

#endif // PLUGINMANAGER_H
