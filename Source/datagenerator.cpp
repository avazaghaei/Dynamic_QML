#include "datagenerator.h"
#include <QDebug>

DataGenerator::DataGenerator(const QString &id, int intervalMs, int minVal, int maxVal, QObject *parent)
    : QObject(parent), m_id(id), m_minVal(minVal), m_maxVal(maxVal), m_currentValue(minVal)
{
    m_timer = new QTimer(this);
    m_timer->setInterval(intervalMs);
    connect(m_timer, &QTimer::timeout, this, &DataGenerator::generateValue);
}

DataGenerator::~DataGenerator()
{
    stop();
}

void DataGenerator::start()
{
    if (!m_timer->isActive()) {
        m_timer->start();
        qDebug() << "Generator" << m_id << "started";
    }
}

void DataGenerator::stop()
{
    if (m_timer->isActive()) {
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
    m_currentValue = QRandomGenerator::global()->bounded(m_minVal, m_maxVal + 1);
    qDebug() << m_id << "value changed to" << m_currentValue;
    emit signalValueChanged(m_currentValue);
}
