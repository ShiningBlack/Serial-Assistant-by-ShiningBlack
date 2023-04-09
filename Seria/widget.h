#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QList>
#include <QString>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QIODevice>
#include <QMessageBox>
#include <QByteArray>
#include <QThread>
#include <ioserialthread.h>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    QSerialPort serialPort;
    IOSerialThread readSerialData;

private slots:
    bool connect_SerialPort(void);
    void print_readData(QByteArray serialData);

signals:
    void sendSerialPort(QSerialPort *serialPort);
    void quitReadThread();
};
#endif // WIDGET_H
