#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Source/manager.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);    // Creates Qt application instance

    QQmlApplicationEngine engine;       // Creates QML engine to run QML files

    // Load main QML
    const QUrl mainQmlUrl(QStringLiteral("qrc:/main.qml"));  // Load main QML file from resources
    engine.load(mainQmlUrl);           // Loads and displays the QML interface

    if (engine.rootObjects().isEmpty()) // Checks if QML loaded successfully
        return -1;

    QObject *rootObj = engine.rootObjects().first();    // Gets the root QML object

    Manager manager(&engine, rootObj);                  // Creates manager to handle C++-QML integration

    return app.exec();
}
