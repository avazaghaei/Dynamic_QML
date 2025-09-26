#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QTimer>
#include <QRandomGenerator>

class DataGenerator : public QObject
{
    Q_OBJECT

public:
    explicit DataGenerator(const QString &id, int msec, int min, int max, QObject *parent = nullptr);
    ~DataGenerator();

    void start();
    void stop();
    bool isRunning() const;

signals:
    void signalValueChanged(int newValue);

private slots:
    void generateValue();

private:
    QString m_id;
    QTimer *m_timer;
    int m_min;
    int m_max;
    int m_msec;
};

#endif // DATAGENERATOR_H
