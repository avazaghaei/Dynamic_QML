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
    QString backendConfig;
    QString frontendConfig;


    struct ItemDesc
    {
        QObject *qmlItem = nullptr;
        QString id;
        QString dataSource;
    };

    QQmlEngine *qmlEngine;
    QObject *rootObject;
    QMap<QString, DataGenerator*> listGenerators;
    QList<ItemDesc> items;
    QString frontendConfigPath;

private:
    void getInputFileAddress();
    void checkInputFileAddress();
    bool loadBackendConfig();
    QJsonDocument checkFile(const QString &path);
    bool loadFrontendConfig();
    void initUiComponents();

public:
    explicit Manager(QQmlEngine *engine, QObject *rootObject, QObject *parent = nullptr);
    ~Manager();

private slots:
    void slotHandleItemXChanged();
};

#endif // MANAGER_H
