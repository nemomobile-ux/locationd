#include <QCoreApplication>
#include <QGeoPositionInfoSource>
#include <QGeoPositionInfo>
#include <QDebug>
#include <QTimer>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    QGeoPositionInfoSource* src = QGeoPositionInfoSource::createSource("geoclue", nullptr);
    if (!src) {
        qCritical() << "no source";
        return 1;
    }
    bool got = false;
    QObject::connect(src, &QGeoPositionInfoSource::positionUpdated, [&](const QGeoPositionInfo& info) {
        if (info.coordinate().isValid()) {
            qDebug() << "POSITION:" << info.coordinate();
            got = true;
        }
    });
    src->setUpdateInterval(1000);
    src->startUpdates();
    QTimer::singleShot(8000, &app, &QCoreApplication::quit);
    app.exec();
    return got ? 0 : 1;
}