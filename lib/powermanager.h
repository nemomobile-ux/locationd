#ifndef POWERMANAGER_H
#define POWERMANAGER_H

#include <QDBusMessage>
#include <QObject>

class PluginManager;

class PowerManager : public QObject {
    Q_OBJECT
public:
    explicit PowerManager(PluginManager* pluginManager, QObject* parent = nullptr);
private slots:
    void onMceDisplayStatus(const QString& state);
    void onDsmeMessage(const QDBusMessage& message);
private:
    PluginManager* m_pluginManager;
};

#endif // POWERMANAGER_H