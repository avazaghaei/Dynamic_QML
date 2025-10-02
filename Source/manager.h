#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QDir>
#include "datagenerator.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QDebug>
#include <QMetaObject>

class Manager : public QObject
{
    Q_OBJECT

private:
    QString backendConfig;  // Path to backend JSON config
    QString frontendConfig; // Path to frontend JSON config

    // Structure to track each QML item
    struct ItemDesc
    {
        QObject *qmlItem = nullptr; // Pointer to QML object
        QString id;                 // Unique identifier
        QString dataSource;         // Which generator provides data
    };

    QQmlEngine *qmlEngine;                          // Pointer to QML engine
    QObject *rootObject;                            // Pointer to root QML object
    QMap<QString, DataGenerator*> listGenerators;   // Map of data generators
    QList<ItemDesc> items;                          // List of created QML items
    QString frontendConfigPath;                     // Stores frontend config path

     // QML creation components
    QObject *parentForItems;    // Container for QML items
    QQmlComponent *component;   // Template for creating MovableBox
    QQmlContext *context;       // Execution context for QML


private:
    void getInputFileAddress();                     // Sets config file paths
    void checkInputFileAddress();                   // Validates if files exist
    bool loadBackendConfig();                       // Reads backend JSON, creates generators
    QJsonDocument checkFile(const QString &path);   // Validates and reads JSON files
    bool loadFrontendConfig();                      // Sets frontend config path
    bool initUiComponents();                        // Main initialization method

    // QML component methods
    QObject* findRootQmlItem();                     // Finds container for dynamic items
    QQmlComponent* initQmlComponent();              // Creates QML component template
    QQmlContext* initQmlContext();                  // Creates execution context
    void readFrontendJson(QJsonDocument doc);       // Processes frontend config
    QObject* makeQmlObject(QString id, double x, double y, QString colorHex, QString dataSource);   // Creates QML instances
    void trackQmlObkect(QObject* objInstance, QString id, QString dataSource);                      // Tracks and connects QML objects


public:
    explicit Manager(QQmlEngine *engine, QObject *rootObject, QObject *parent = nullptr);
    ~Manager();

private slots:
    void slotHandleItemXChanged();  // Called when QML item moves horizontally
};

#endif // MANAGER_H
