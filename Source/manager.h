#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QString>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>

class DataGenerator;

class Manager : public QObject
{
    Q_OBJECT

private:
    struct ItemDesc
    {
        QObject *qmlItem = nullptr;
        QString id;
        QString dataSource;
    };

    QQmlEngine *qmlEngine;
    QObject *rootObject;
    QMap<QString, DataGenerator*> generators;
    QList<ItemDesc> items;
    QString frontendConfigPath;

public:
    explicit Manager(QQmlEngine *engine, QObject *rootObject, QObject *parent = nullptr);
    ~Manager();

    bool loadBackendConfig(const QString &path);
    bool loadFrontendConfig(const QString &path);
    void instantiateUIComponents();

private slots:
    void slotHandleItemXChanged();
};

#endif // MANAGER_H
