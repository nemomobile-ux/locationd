#include "accuracy.h"

struct AccuracyInitializer {
    AccuracyInitializer()
    {
        qDBusRegisterMetaType<Accuracy>();
    }
};

static AccuracyInitializer _accuracyInitializer;

QDBusArgument& operator<<(QDBusArgument& arg, const Accuracy& acc)
{
    arg.beginStructure();
    arg << acc.level << acc.horizontal << acc.vertical;
    arg.endStructure();
    return arg;
}

const QDBusArgument& operator>>(const QDBusArgument& arg, Accuracy& acc)
{
    arg.beginStructure();
    arg >> acc.level >> acc.horizontal >> acc.vertical;
    arg.endStructure();
    return arg;
}
