#include "locationdaemonmasterinterface.h"
#include "masteradaptor.h"

LocationDaemonMasterInterface::LocationDaemonMasterInterface(QObject* parent)
    : QObject { parent }
    , m_clientCount(0)
{
    MasterAdaptor adaptor(this);
    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.registerObject(
            QStringLiteral("/org/freedesktop/Geoclue/Master"),
            "hello",
            this)) {
        qWarning() << "Failed to register Master object:" << bus.lastError().message();
    }
}

QDBusObjectPath LocationDaemonMasterInterface::Create()
{
    qDebug() << Q_FUNC_INFO;
    QString path = QString("/org/freedesktop/Geoclue/Client%1").arg(++m_clientCount);

    if(!QDBusConnection::sessionBus().registerObject(
            path,
            this,
            QDBusConnection::ExportAdaptors
            )) {
        qWarning() << "Failed to register client object:" << QDBusConnection::sessionBus().lastError().message();
    }

/*    m_provider = m_pluginManager->bestProvider();
    if(m_provider) {
        connect(m_provider, &ILocationProvider::updated, this, &GeoClue1Service::onProviderUpdated);
        emit PositionProviderChanged("locationd", "Location daemon", "org.freedesktop.Geoclue.Master", path);
    }*/

    return QDBusObjectPath(path);
}
