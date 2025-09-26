#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Source/manager.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Load main QML
    const QUrl mainQmlUrl(QStringLiteral("qrc:/main.qml"));
    engine.load(mainQmlUrl);

    if (engine.rootObjects().isEmpty())
        return -1;

    QObject *rootObj = engine.rootObjects().first();

    Manager manager(&engine, rootObj);

    return app.exec();
}
