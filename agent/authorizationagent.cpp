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

#include "authorizationagent.h"

#include <QEventLoop>

#include <notification.h>

AuthorizationAgent::AuthorizationAgent(QObject* parent)
    : QObject(parent)
{
}

int AuthorizationAgent::RequestAuthorization(const QString& app, uint pid, const QString& exe)
{
    Q_UNUSED(pid)
    Q_UNUSED(exe)

    Notification n;
    n.setSummary(tr("Location request"));
    n.setBody(app + tr(" wants to access your location"));
    n.setAppName(tr("locationd"));
    n.setUrgency(Notification::Critical);

    QVariantList actions;
    actions << Notification::remoteAction(QStringLiteral("allow_once"), tr("Allow once"))
            << Notification::remoteAction(QStringLiteral("allow_always"), tr("Allow always"))
            << Notification::remoteAction(QStringLiteral("disallow"), tr("Disallow"));
    n.setRemoteActions(actions);

    int choice = 2; // default disallow
    bool responded = false;
    QEventLoop loop;

    QObject::connect(&n, &Notification::actionInvoked, this, [&](const QString& name) {
        if (name == QStringLiteral("allow_once")) choice = 0;
        else if (name == QStringLiteral("allow_always")) choice = 1;
        else if (name == QStringLiteral("disallow")) choice = 2;
        responded = true;
        loop.quit();
    });
    QObject::connect(&n, &Notification::closed, this, [&](uint) {
        if (!responded) {
            choice = 2;
            loop.quit();
        }
    });

    n.publish();
    loop.exec();
    n.close();

    return choice;
}