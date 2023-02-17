#include <QCoreApplication>

#include "qmultikeyhash_test.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTest::qExec(new QMultiKeyHashTest(1'000ll), argc, argv);
    QMetaObject::invokeMethod(&app, &QCoreApplication::quit, Qt::QueuedConnection);

    return app.exec();
}
