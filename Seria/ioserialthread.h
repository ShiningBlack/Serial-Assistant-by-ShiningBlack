#ifndef IOSERIALTHREAD_H
#define IOSERIALTHREAD_H

#include <QObject>
#include <QDebug>
#include <QSerialPort>
#include <QByteArray>
#include <QThread>
#include <QCoreApplication>

class IOSerialThread : public QObject
{
    Q_OBJECT
public:
    QThread thread;

    explicit IOSerialThread(QObject *parent = nullptr);
    ~IOSerialThread(){
        thread.quit();
        thread.wait();
    }
    void readSerialPort();

signals:
    void readedData(QByteArray);

public slots:
    void GetSerialPort(QSerialPort *serialPort);
    void quitReadThread();

private:
    bool cond = false;
    QSerialPort *serialPort_ = nullptr;
};

#endif // IOSERIALTHREAD_H
