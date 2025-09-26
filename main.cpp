#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Source/manager.h"
#include <QDir>

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

    Manager manager(&engine, rootObj, nullptr);

    // Use relative paths and check if files exist
    QString backendConfig = "configs/backend_5.json";
    QString frontendConfig = "configs/frontend_5.json";

    // Check if files exist
    if (!QFile::exists(backendConfig)) {
        qWarning() << "Backend config file not found:" << backendConfig;
        // Try absolute path
        backendConfig = QDir::currentPath() + "/configs/backend_3.json";
    }

    if (!QFile::exists(frontendConfig)) {
        qWarning() << "Frontend config file not found:" << frontendConfig;
        frontendConfig = QDir::currentPath() + "/configs/frontend_3.json";
    }

    qDebug() << "Using backend config:" << backendConfig;
    qDebug() << "Using frontend config:" << frontendConfig;

    if (!manager.loadBackendConfig(backendConfig)) {
        qWarning() << "Failed to load backend config";
        return -1;
    }

    if (!manager.loadFrontendConfig(frontendConfig)) {
        qWarning() << "Failed to load frontend config";
        return -1;
    }

    manager.instantiateUIComponents();

    return app.exec();
}
