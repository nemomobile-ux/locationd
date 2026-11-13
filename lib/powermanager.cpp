#include "powermanager.h"

#include "pluginmanager.h"

#include <QDBusConnection>
#include <QDebug>

PowerManager::PowerManager(PluginManager* pluginManager, QObject* parent)
    : QObject(parent)
    , m_pluginManager(pluginManager)
{
    QDBusConnection sysBus = QDBusConnection::systemBus();
    if (!sysBus.isConnected()) {
        qWarning() << "PowerManager: cannot connect to system bus";
        return;
    }

    // MCE display status
    sysBus.connect("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal",
                   "display_status_ind", this, SLOT(onMceDisplayStatus(QString)));

    // DSME device state (try common signal names)
    QStringList dsmeSignals = { "device_state_ind", "sig_device_state_ind", "state_ind" };
    for (const QString& sig : dsmeSignals) {
        sysBus.connect("com.nokia.dsme", "/com/nokia/dsme/signal", "com.nokia.dsme.signal",
                       sig, this, SLOT(onDsmeMessage(QDBusMessage)));
    }

    qInfo() << "PowerManager: MCE/DSME integration enabled";
}

void PowerManager::onMceDisplayStatus(const QString& state)
{
    qDebug() << "MCE display status:" << state;
    bool allowed = (state == QLatin1String("on") || state == QLatin1String("dimmed"));
    if (m_pluginManager) {
        m_pluginManager->setPowerAllowed(allowed);
    }
}

void PowerManager::onDsmeMessage(const QDBusMessage& message)
{
    QString state;
    if (!message.arguments().isEmpty()) {
        state = message.arguments().first().toString().toLower();
    }
    qDebug() << "DSME state:" << state;
    if (state.isEmpty()) {
        return;
    }
    bool allowed = true;
    if (state.contains("suspend") || state.contains("sleep") || state.contains("shutdown")) {
        allowed = false;
    } else if (state.contains("normal") || state.contains("running") || state.contains("on")) {
        allowed = true;
    }
    if (m_pluginManager) {
        m_pluginManager->setPowerAllowed(allowed);
    }
}