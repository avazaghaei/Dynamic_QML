#include "manager.h"


Manager::Manager(QQmlEngine *engine, QObject *rootObject, QObject *parent)
    : QObject(parent), qmlEngine(engine), rootObject(rootObject)
{
    getInputFileAddress();
    checkInputFileAddress();

    if(!loadBackendConfig())    // Step 1: Create data generators
        return;

    if(!loadFrontendConfig())   // Step 2: Set frontend config
        return;

    if(!initUiComponents())     // Step 3: Create QML components
        return;

}

Manager::~Manager()
{
    // Clean up listGenerators
    for (auto generator : listGenerators)
    {
        if (generator) {
            generator->stop();
            generator->deleteLater();
        }
    }
    listGenerators.clear();
}

void Manager::getInputFileAddress()
{
    // Use relative paths and check if files exist
    backendConfig   = QDir::currentPath() + "/configs/backend_5.json";
    frontendConfig  = QDir::currentPath() + "/configs/frontend_5.json";
}

void Manager::checkInputFileAddress()
{
    // Check if files exist
    if (!QFile::exists(backendConfig))
        qWarning() << "Backend config file not found:" << backendConfig;

    if (!QFile::exists(frontendConfig))
        qWarning() << "Frontend config file not found:" << frontendConfig;
}

bool Manager::loadBackendConfig()
{
    QJsonDocument doc = checkFile(backendConfig);
    if(doc.isNull())
        return false;

    for (const auto &v : doc.array())
    {
        if (!v.isObject())
            continue;

        QJsonObject obj = v.toObject();

        QString id  = obj.value("id").toString();
        int msec    = obj.value("msec").toInt(1000);
        int min     = obj.value("min").toInt(0);
        int max     = obj.value("max").toInt(100);

        if (id.isEmpty())
            continue;

        DataGenerator* newGenerator = new DataGenerator(id, msec, min, max, this);
        newGenerator->start();
        listGenerators.insert(id, newGenerator);

        qDebug() << "Created generator:" << id << "interval:" << msec << "min:" << min << "max:" << max;
    }

    qInfo() << "Loaded backend listGenerators:" << listGenerators.keys();
    return true;
}

QJsonDocument Manager::checkFile(const QString &path)
{
    if (path.isEmpty())
    {
        qWarning() << "No frontend config path set";
        return {};
    }

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open backend config:" << path;
        return {};
    }

    QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isArray())
    {
        qWarning() << "config should be a JSON array";
        return {};
    }
    return doc;
}

bool Manager::loadFrontendConfig()
{
    frontendConfigPath = frontendConfig;
    qDebug() << "Frontend config path set to:" << frontendConfig;
    return true;
}

bool Manager::initUiComponents()
{ 
    QJsonDocument doc = checkFile(frontendConfig);
    if(doc.isNull())
        return false;

    parentForItems = findRootQmlItem();
    if(parentForItems == nullptr)
        return false;

    component = initQmlComponent();
    if(component == nullptr)
        return false;

    context = initQmlContext();

    readFrontendJson(doc);

    return true;
}

QObject* Manager::findRootQmlItem()
{
    // Find the container for items
    QObject *parentForItems = rootObject->findChild<QObject*>("rootItem");
    if (!parentForItems)
    {
        qWarning() << "Could not find rootItem";
        return nullptr;
    }
    return parentForItems;
}

QQmlComponent *Manager::initQmlComponent()
{
    // Create component
    QQmlComponent* component = new QQmlComponent(qmlEngine, QUrl(QStringLiteral("qrc:/MovableBox.qml"))); //class definition  //A template or blueprint of MovableBox.qml
    if (component->isError())
    {
        qWarning() << "Failed to load MovableBox.qml:" << component->errors();
        return nullptr;
    }
    return component;
}

QQmlContext *Manager::initQmlContext()
{
    // Create object with parent context
    QQmlContext *context = new QQmlContext(qmlEngine->rootContext());

    return context;
}

void Manager::readFrontendJson(QJsonDocument doc)
{
    for (const auto &v : doc.array())
    {
        if (!v.isObject())
            continue;

        QJsonObject obj = v.toObject();

        QString id          = obj.value("id").toString();
        double x            = obj.value("x").toDouble(0);
        double y            = obj.value("y").toDouble(0);
        QString colorHex    = obj.value("color-hex").toString("#4ab471");
        QString dataSource  = obj.value("dataSource").toString();

        if (id.isEmpty())
        {
            qWarning() << "Skipping item with empty id";
            continue;
        }

        QObject *objInstance = makeQmlObject(id, x, y, colorHex, dataSource);   //An actual instance of MovableBox
        if(objInstance == nullptr)
            continue;

        qDebug() << "Creating box:" << id << "at (" << x << "," << y << ")";

        trackQmlObkect(objInstance, id, dataSource);
    }

    qInfo() << "Instantiated" << items.size() << "UI components";

}

QObject* Manager::makeQmlObject(QString id, double x, double y, QString colorHex, QString dataSource)
{
    QObject *objInstance = component->create(context);  // Creates QML instance from template

    if (!objInstance)
    {
        qWarning() << "Failed to create MovableBox instance for" << id;
        delete context;
        return nullptr;
    }

    // CRITICAL: Set the visual parent using QML engine method
    objInstance->setProperty("parent", QVariant::fromValue(parentForItems));

    // Set properties
    objInstance->setProperty("objectId", id);
    objInstance->setProperty("x", x);
    objInstance->setProperty("y", y);
    objInstance->setProperty("colorHex", colorHex);
    objInstance->setProperty("dataSource", dataSource);
    objInstance->setProperty("displayText", "0");

    // Connects QML signal to C++ slot
    connect(objInstance, SIGNAL(xChanged()), this, SLOT(slotHandleItemXChanged()));

    return objInstance;
}

void Manager::trackQmlObkect(QObject* objInstance, QString id, QString dataSource)
{
    // Track the item
    ItemDesc desc;
    desc.qmlItem = objInstance;
    desc.id = id;
    desc.dataSource = dataSource;
    items.append(desc);

    // Connect backend generator to QML
    if (listGenerators.contains(dataSource))
    {
        DataGenerator *gen = listGenerators.value(dataSource);
        connect(gen, &DataGenerator::signalValueChanged, this,
                [objInstance, id](int newVal){
                    objInstance->setProperty("displayText", QString::number(newVal));
                    qDebug() << id << "UI updated to" << newVal;
                });
    }
    else
    {
        qWarning() << "No backend generator for dataSource" << dataSource;
    }
}

void Manager::slotHandleItemXChanged()
{
    QObject *item = sender();   // Gets which QML item triggered the signal
    if (!item)
        return;

    double x = item->property("x").toDouble();

    double rootWidth = rootObject->property("width").toDouble();
    double halfWidth = rootWidth / 2.0;

    // Find the item description
    ItemDesc *foundDesc = nullptr;
    for (auto &desc : items)
    {
        if (desc.qmlItem == item)
        {
            foundDesc = &desc;
            break;
        }
    }

    if (!foundDesc || !listGenerators.contains(foundDesc->dataSource))
    {
        return;
    }

    DataGenerator *gen = listGenerators.value(foundDesc->dataSource);
    if (x >= halfWidth)
    {
        if (gen->isRunning())
        {
            gen->stop();
            qInfo() << "Paused generator" << foundDesc->dataSource << "for item" << foundDesc->id << "(x=" << x << ")";
        }
    }
    else
    {
        if (!gen->isRunning())
        {
            gen->start();
            qInfo() << "Resumed generator" << foundDesc->dataSource << "for item" << foundDesc->id << "(x=" << x << ")";
        }
    }
}
