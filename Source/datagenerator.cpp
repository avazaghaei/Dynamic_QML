#include "datagenerator.h"
#include <QDebug>

DataGenerator::DataGenerator(const QString &id, int msec, int min, int max, QObject *parent)
    : QObject(parent), m_id(id), m_msec(msec), m_min(min), m_max(max)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(msec);
    connect(m_timer, &QTimer::timeout, this, &DataGenerator::generateValue);
}

DataGenerator::~DataGenerator()
{
    stop();
}

void DataGenerator::start()
{
    if (!m_timer->isActive())
    {
        m_timer->start();
        qDebug() << "Generator" << m_id << "started";
    }
}

void DataGenerator::stop()
{
    if (m_timer->isActive())
    {
        m_timer->stop();
        qDebug() << "Generator" << m_id << "stopped";
    }
}

bool DataGenerator::isRunning() const
{
    return m_timer->isActive();
}

void DataGenerator::generateValue()
{
    int newValue = QRandomGenerator::global()->bounded(m_min, m_max + 1);
    qDebug() << m_id << "value changed to" << newValue;
    emit signalValueChanged(newValue);
}
