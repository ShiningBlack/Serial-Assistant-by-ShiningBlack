#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setWindowTitle("串口助手 By ShiningBlack"); // set window title
    this->setWindowIcon(QIcon(":/new/prefix1/USB.png")); // set window icon

    // 默认为ASCII被点击
    ui->radioButton_ASCII->setChecked(true);

    // 连接按钮槽函数
    QObject::connect(ui->pushButton_ConnectPort, SIGNAL(clicked()), this, SLOT(connect_SerialPort()));

    /* 获取串口列表 */ /* 这里应该开一个线程 */
    QList serial_List = QSerialPortInfo::availablePorts();
    for (auto i = serial_List.begin(); i != serial_List.end(); ++i){
        ui->comboBox_Port->addItem(i->portName());
    }

    /* 设置波特率列表 */
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud115200));
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud57600));
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud38400));
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud19200));
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud9600));
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud4800));
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud2400));
    ui->comboBox_BaudRate->addItem(QString::number(QSerialPort::Baud1200));

    /* 设置数据位 */
    ui->comboBox_DataBit->addItem(QString::number(QSerialPort::Data8));
    ui->comboBox_DataBit->addItem(QString::number(QSerialPort::Data7));
    ui->comboBox_DataBit->addItem(QString::number(QSerialPort::Data6));
    ui->comboBox_DataBit->addItem(QString::number(QSerialPort::Data5));

    /* 设置停止位 */
    ui->comboBox_StopBit->addItem(QString::number(QSerialPort::OneStop));
    ui->comboBox_StopBit->addItem(QString::number(QSerialPort::TwoStop));

    /* 设置校验位 */
    ui->comboBox_CheckBit->addItem("无");
    ui->comboBox_CheckBit->addItem("奇");
    ui->comboBox_CheckBit->addItem("偶");

    /* 设置聊天框只读 */
    ui->textEdit_Msg->setReadOnly(true);

    /* 连接串口后，把串口发送到IO线程 */
    connect(this, SIGNAL(quitReadThread()), &readSerialData, SLOT(quitReadThread()));

    connect(&readSerialData, SIGNAL(readedData(QByteArray)), this, SLOT(print_readData(QByteArray)));

    connect(this, SIGNAL(sendSerialPort(QSerialPort*)), &readSerialData, SLOT(GetSerialPort(QSerialPort*)), Qt::DirectConnection);
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::connect_SerialPort()
{
    qDebug() << "connect Slot";
    qDebug() << ui->pushButton_ConnectPort->text();
    if (ui->pushButton_ConnectPort->text() == "Connect")
    {
        qDebug() << "open Serial Port";
        serialPort.setPortName(ui->comboBox_Port->currentText());
        serialPort.setBaudRate(ui->comboBox_BaudRate->currentText().toInt());
        serialPort.setStopBits(QSerialPort::StopBits(ui->comboBox_StopBit->currentText().toInt()));
        // 设置奇偶校验位
        if (ui->comboBox_CheckBit->currentText() == "无"){
            serialPort.setParity(QSerialPort::NoParity);
        }else if (ui->comboBox_CheckBit->currentText() == "奇"){
            serialPort.setParity(QSerialPort::OddParity);
        }else if (ui->comboBox_CheckBit->currentText() == "偶"){
            serialPort.setParity(QSerialPort::EvenParity);
        }else{
            serialPort.setParity(QSerialPort::NoParity);
        }
        // 设置为没有控制流
        serialPort.setFlowControl(QSerialPort::NoFlowControl);

        // 打开串口
        if (!serialPort.open(QIODevice::ReadWrite)){
            qDebug() << "Failed open SerialPort";
            QMessageBox::critical(this, "Error", "Open SerialPort Failed");
            return false;
        }else{
            // 打开串口成功
            qDebug() << "send serial ";
            ui->comboBox_Port->setEnabled(false);
            ui->comboBox_BaudRate->setEnabled(false);
            ui->comboBox_DataBit->setEnabled(false);
            ui->comboBox_StopBit->setEnabled(false);
            ui->comboBox_CheckBit->setEnabled(false);
            ui->pushButton_ConnectPort->setText("Disconnect");

            emit sendSerialPort(&serialPort);
            readSerialData.moveToThread(&(readSerialData.thread));
            connect(&(readSerialData.thread), &QThread::started, &readSerialData, &IOSerialThread::readSerialPort);

            readSerialData.thread.start();

            QMessageBox::information(this, "Infomation", "Open SerialPort Success!");
        }
    }else{
        qDebug() << "close Serial Port";
        emit quitReadThread();
        readSerialData.thread.quit();
        qDebug() << "thread.quit";
//        readSerialData.thread.exit();
//        qDebug() << "thread.exit";
        readSerialData.thread.wait();
        qDebug() << "thread.wait";

        serialPort.clear();
        serialPort.close();
        ui->pushButton_ConnectPort->setText("Connect");
        ui->comboBox_Port->setEnabled(true);
        ui->comboBox_BaudRate->setEnabled(true);
        ui->comboBox_DataBit->setEnabled(true);
        ui->comboBox_StopBit->setEnabled(true);
        ui->comboBox_CheckBit->setEnabled(true);     
    }
    return true;
}

void Widget::print_readData(QByteArray serialData)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentTimeString = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");

    ui->textEdit_Msg->append(ui->comboBox_Port->currentText() + QString(" ") +currentTimeString);

    ui->textEdit_Msg->append(QString(serialData));
    //ui->textEdit_Msg->append(QString("\n"));
}


