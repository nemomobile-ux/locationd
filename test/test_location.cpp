#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QDBusObjectPath>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QDBusInterface master("org.freedesktop.Geoclue.Master", "/org/freedesktop/Geoclue/Master",
                          "org.freedesktop.Geoclue.Master", QDBusConnection::sessionBus());
    QDBusReply<QDBusObjectPath> reply = master.call("Create");
    if (!reply.isValid()) {
        qCritical() << "Create failed:" << reply.error().message();
        return 1;
    }
    QString path = reply.value().path();
    if (path.isEmpty() || path.contains(QStringLiteral("denied"))) {
        qDebug() << "DENIED";
        return 1;
    }
    qDebug() << "ALLOWED:" << path;
    return 0;
}