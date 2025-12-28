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

#include "gpsplugin.h"
#include <QDebug>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

GPSProvider::GPSProvider(QObject* parent)
    : ILocationProvider(parent)
{
}

GPSProvider::~GPSProvider()
{
    if (m_fd >= 0)
        ::close(m_fd);
}

void GPSProvider::requestLocationUpdate()
{
    m_fd = ::open("/dev/ttyACM0", O_RDONLY | O_NONBLOCK);
    if (m_fd < 0) {
        qWarning() << "GPSProvider: cannot open /dev/ttyACM0";
        return;
    }

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
                    double newLat = m_parser.latitude();
                    double newLon = m_parser.longitude();
                    double newAlt = m_parser.altitude();
                    double newSpeed = m_parser.speed();
                    double newDir = m_parser.direction();
                    Accuracy newAcc = m_parser.accuracy();
                    QVector<SatInfoFull> newSats = m_parser.satellites();

                    bool changed = (newLat != m_latitude) || (newLon != m_longitude) || (newAlt != m_altitude) || (newSpeed != m_speed) || (newDir != m_direction) || (newAcc.horizontal != m_accuracy.horizontal) || (newAcc.vertical != m_accuracy.vertical) || (newSats != m_satellites);

                    if (changed) {
                        m_latitude = newLat;
                        m_longitude = newLon;
                        m_altitude = newAlt;
                        m_speed = newSpeed;
                        m_direction = newDir;
                        m_accuracy = newAcc;
                        m_satellites = newSats;

                        qDebug() << "[ GPS ] Updated Lat:" << m_latitude
                                 << "Lon:" << m_longitude
                                 << "Accuracy:" << m_accuracy.horizontal;

                        emit updated();
                    }
                }
            }
        }
    }

    QTimer::singleShot(50, this, &GPSProvider::readData);
}
