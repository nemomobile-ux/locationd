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

#include "gpsplugin.h"
#include <QDebug>
#include <QSettings>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

GPSProvider::GPSProvider(QObject* parent)
    : ILocationProvider(parent)
{
    loadConfig();
    int flags = O_NONBLOCK;
    if (!m_wakeupCommand.isEmpty() || !m_sleepCommand.isEmpty()) {
        flags |= O_RDWR;
    } else {
        flags |= O_RDONLY;
    }
    m_fd = ::open(m_devicePath.toLocal8Bit().constData(), flags);
    if (m_fd < 0) {
        qWarning() << "GPSProvider: cannot open" << m_devicePath;
        return;
    }
}

GPSProvider::~GPSProvider()
{
    if (m_fd >= 0) {
        if (m_active && !m_sleepCommand.isEmpty()) {
            sendCommand(m_sleepCommand);
        }
        ::close(m_fd);
    }
}

void GPSProvider::loadConfig()
{
    QString configPath = qEnvironmentVariable("LOCATIOND_GPS_CONFIG", "/etc/locationd/gps.conf");
    QSettings settings(configPath, QSettings::IniFormat);
    m_devicePath = settings.value("device", m_devicePath).toString();
    m_wakeupCommand = settings.value("wakeupCommand", m_wakeupCommand).toString();
    m_sleepCommand = settings.value("sleepCommand", m_sleepCommand).toString();
}

void GPSProvider::sendCommand(const QString& cmd)
{
    if (m_fd < 0 || cmd.isEmpty())
        return;
    QByteArray data = cmd.toUtf8();
    data.replace("\\r", "\r");
    data.replace("\\n", "\n");
    ::write(m_fd, data.constData(), data.size());
}

void GPSProvider::setActive(bool active)
{
    if (m_active == active)
        return;
    m_active = active;
    if (active) {
        sendCommand(m_wakeupCommand);
        readData();
    } else {
        sendCommand(m_sleepCommand);
    }
}

void GPSProvider::requestLocationUpdate()
{
    readData();
}

void GPSProvider::readData()
{
    if (m_fd < 0)
        return;

    char buf[1024];
    while (true) {
        ssize_t n = ::read(m_fd, buf, sizeof(buf));
        if (n <= 0)
            break;

        m_buffer.append(buf, static_cast<int>(n));

        int index;
        while ((index = m_buffer.indexOf('\n')) >= 0) {
            QByteArray line = m_buffer.left(index).trimmed();
            m_buffer.remove(0, index + 1);

            if (!line.isEmpty()) {
                if (m_parser.parseNMEA(line)) {
                    QGeoCoordinate newCoordinate = m_parser.coordinate();
                    double newSpeed = m_parser.speed();
                    double newDir = m_parser.direction();
                    Accuracy newAcc = m_parser.accuracy();
                    QList<QGeoSatelliteInfo> newSats = m_parser.satellitesInView();

                    bool changed = (newCoordinate != m_coordinate)
                        || (newSpeed != m_speed)
                        || (newDir != m_direction)
                        || (newAcc.horizontal() != m_accuracy.horizontal())
                        || (newAcc.vertical() != m_accuracy.vertical());

                    if (changed) {
                        m_coordinate = newCoordinate;
                        m_speed = newSpeed;
                        m_direction = newDir;
                        m_accuracy = newAcc;

                        qDebug() << "[ GPS ] Updated coordinates:" << m_coordinate
                                 << "Accuracy:" << m_accuracy.horizontal();

                        emit positionUpdated();
                    }

                    m_satellites.satellitesView = m_parser.satellitesInView();
                    m_satellites.satellitesUsed = m_parser.satellitesInUse();
                }
            }
        }
    }

    if (m_active) {
        QTimer::singleShot(50, this, &GPSProvider::readData);
    }
}
