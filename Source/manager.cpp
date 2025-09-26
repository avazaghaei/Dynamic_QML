#include "manager.h"


Manager::Manager(QQmlEngine *engine, QObject *rootObject, QObject *parent)
    : QObject(parent), qmlEngine(engine), rootObject(rootObject)
{
    getInputFileAddress();
    checkInputFileAddress();

    if(!loadBackendConfig())
        return;

    if(!loadFrontendConfig())
        return;

    if(!initUiComponents())
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

    QObject *parentForItems = findRootQmlItem();
    if(parentForItems == nullptr)
        return false;

    QQmlComponent* component = initQmlComponent();
    if(component == nullptr)
        return false;

    QQmlContext *context = initQmlContext();

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

        qDebug() << "Creating box:" << id << "at (" << x << "," << y << ")";




        QObject *objInstance = component->create(context);

        if (!objInstance)
        {
            qWarning() << "Failed to create MovableBox instance for" << id;
            delete context;
            continue;
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

        // Verify properties were set
        qDebug() << "Box created - x:" << objInstance->property("x").toDouble()
                 << "y:" << objInstance->property("y").toDouble()
                 << "visible:" << objInstance->property("visible").toBool()
                 << "color:" << objInstance->property("colorHex").toString();

        // Track the item
        ItemDesc desc;
        desc.qmlItem = objInstance;
        desc.id = id;
        desc.dataSource = dataSource;
        items.append(desc);

        // Connect signals
        bool ok = connect(objInstance, SIGNAL(xChanged()), this, SLOT(slotHandleItemXChanged()));
        if (!ok) {
            qWarning() << "Failed to connect xChanged for" << id;
        }

        // Connect backend generator
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

    qInfo() << "Instantiated" << items.size() << "UI components";

    // Debug: Check what's in the rootItem
    QObjectList children = parentForItems->children();
    qDebug() << "rootItem has" << children.size() << "children";
    for (QObject *child : children) {
        qDebug() << "Child:" << child << "type:" << child->metaObject()->className()
        << "visible:" << child->property("visible").toBool();
    }

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
    QQmlComponent* component = new QQmlComponent(qmlEngine, QUrl(QStringLiteral("qrc:/MovableBox.qml")));
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

void Manager::slotHandleItemXChanged()
{
    QObject *item = sender();
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
