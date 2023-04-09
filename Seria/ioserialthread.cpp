#include "ioserialthread.h"

IOSerialThread::IOSerialThread(QObject *parent)
    : QObject{parent}
{

}

void IOSerialThread::readSerialPort()
{
    qDebug() << "read data thread";
    QByteArray serialPortData;

    while (serialPort_ == nullptr);
    qDebug() << "获得串口";

    while (cond){
        if (serialPort_->waitForReadyRead(100)){
            serialPortData = serialPort_->readAll();
            qDebug() << serialPortData;
            emit readedData(serialPortData);
        }
        QCoreApplication::processEvents();   // 定期检查事件
    }
}

void IOSerialThread::GetSerialPort(QSerialPort *serialPort)
{
    cond = true;
    this->serialPort_ = serialPort;
    qDebug() << "接收到串口";
}

void IOSerialThread::quitReadThread()
{
    cond = false;
}
