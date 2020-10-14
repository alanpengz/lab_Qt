//mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <thread>
#include <threads.h>
#include <unistd.h>

std::map<QString, QString>
orderMap{{"-7","1"}, {"-6","2"}, {"-5","3"}, {"-4","4"},{"-3","5"},{"-2","6"},{"-1","7"},{"0","8"},{"1","9"},{"2","a"},{"3","b"},{"4","c"},{"5","d"},{"6","e"},{"7","f"}};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("UWOC Comm");

    spi_init();
    //连接serialROV serialSonic信号和槽
    QObject::connect(&serialROV, &QSerialPort::readyRead, this, &MainWindow::serialROV_readyRead);
    QObject::connect(&serialSonic, &QSerialPort::readyRead, this, &MainWindow::serialSonic_readyRead);


    std::thread t1(&MainWindow::serialport_refresh, this);
    t1.detach();

    ui->X_upButton->setEnabled(false);
    ui->X_downButton->setEnabled(false);
    ui->Y_upButton->setEnabled(false);
    ui->Y_downButton->setEnabled(false);
    ui->M_upButton->setEnabled(false);
    ui->M_downButton->setEnabled(false);
    ui->Z_upButton->setEnabled(false);
    ui->Z_downButton->setEnabled(false);
    ui->speedResetButton->setEnabled(false);
    ui->Xspeed_label->setText(QString::number(X));
    ui->Yspeed_label->setText(QString::number(Y));
    ui->Mspeed_label->setText(QString::number(M));
    ui->Zspeed_label->setText(QString::number(Z));
    ui->sonicSendButton->setEnabled(false);
    ui->AButton->setEnabled(false);
    ui->DButton->setEnabled(false);
    ui->MButton->setEnabled(false);
    ui->EButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::serialport_refresh(){
    //初始
    ui->serialBox->clear();
    ui->sonicBox->clear();
    ports = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo &info, ports){
        ui->serialBox->addItem(info.portName());
        ui->sonicBox->addItem(info.portName());
    }
    //刷新
    while(true){
        if(QSerialPortInfo::availablePorts().size()!=ports.size()){
            ui->serialBox->clear();
            ui->sonicBox->clear();
            foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
                ui->serialBox->addItem(info.portName());
                ui->sonicBox->addItem(info.portName());
            }
            ports = QSerialPortInfo::availablePorts();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void MainWindow::spi_recv(){
    pinMode(6, INPUT);
    while(true){
        if(digitalRead(6)){
            unsigned char vlcrecv[239];
            wiringPiSPIDataRW(1, vlcrecv, 239);
            char* tmp = (char*)vlcrecv;

            QString spirecv = QString(tmp);
            ui->vlcRecvtextBrowser->append(spirecv);
            memset(vlcrecv, 0, 0);
        }
    }
}

void MainWindow::spi_init(){
    //初始化SPI
    if(wiringPiSetup()<0){
      QMessageBox::about(NULL, "提示", "wiringPi初始化失败！");
     }
    if(spiFd0 = wiringPiSPISetup(0,976000)==-1){
       QMessageBox::about(NULL, "提示", "SPI0初始化失败！");
     }
    if(spiFd1 = wiringPiSPISetup(1,976000)==-1){
       QMessageBox::about(NULL, "提示", "SPI1初始化失败！");
     }

    std::thread t(&MainWindow::spi_recv, this);
    t.detach();
}


void MainWindow::serialROV_readyRead()
{
    //从接收缓冲区中读取数据
    QByteArray buffer = serialROV.readAll();
    //从界面中读取以前收到的数据
    QString recv = ui->textBrowser->toPlainText();
    recv += QString(buffer);
    //清空以前的显示
    ui->textBrowser->clear();
    //重新显示
    ui->textBrowser->append(recv);
}

void MainWindow::serialSonic_readyRead()
{
    // 接收显示
    QByteArray buffer = serialSonic.readAll();
    QString show = ui->sonicRecvtextBrowser->toPlainText();
    show += QString(buffer);
    ui->sonicRecvtextBrowser->clear();
    ui->sonicRecvtextBrowser->append(show);

    // 水声控制指令解析
    QString recv = QString(buffer);
    QString orderKey = "Received String: ";
    if(int index = recv.indexOf(orderKey)){
        QString orderVal = recv.mid(index+17,6); //解析出的指令
        if(orderVal.startsWith("#") && orderVal.endsWith("$")){
            if(!serialROV.open(QIODevice::ReadWrite)){
                QMessageBox::about(NULL, "提示", "ROV串口未打开！");
                return;
            }
            else{
                QByteArray order_send = orderVal.toUtf8();
                serialROV.write(order_send);
                ui->sonicOrderlabel->setText(orderVal);
                QString QX, QY, QM, QZ;
                for(std::map<QString,QString>::iterator it = orderMap.begin();it!=orderMap.end();it++) {
                    if(it->second==QString(orderVal[1])) QX = it->first;
                    if(it->second==QString(orderVal[2])) QY = it->first;
                    if(it->second==QString(orderVal[3])) QM = it->first;
                    if(it->second==QString(orderVal[4])) QZ = it->first;
                }
                // 更新全局速度
                X = QX.toInt();
                Y = QY.toInt();
                M = QM.toInt();
                Z = QZ.toInt();
                ui->Xspeed_label->setText(QX);
                ui->Yspeed_label->setText(QY);
                ui->Mspeed_label->setText(QM);
                ui->Zspeed_label->setText(QZ);
            }
        }
    }
    else{
        QString orderNotFound = "NotFound";
        ui->sonicOrderlabel->setText(orderNotFound);
    }
    serialSonic.clear();
    buffer.clear();
}

//ROV
void MainWindow::on_openButton_clicked()
{
    if(ui->openButton->text()==QString("打开串口"))
    {
        serialROV.setPortName(ui->serialBox->currentText());
        serialROV.setBaudRate(9600);
        serialROV.setDataBits(QSerialPort::Data8);
        serialROV.setParity(QSerialPort::NoParity);
        serialROV.setStopBits(QSerialPort::OneStop);
        serialROV.setFlowControl(QSerialPort::NoFlowControl);
        //打开串口
        if(!serialROV.open(QIODevice::ReadWrite))
        {
            QMessageBox::about(NULL, "提示", "无法打开串口！");
            return;
        }
        //下拉失能
        ui->serialBox->setEnabled(false);
        ui->openButton->setText(QString("关闭串口"));
        //使能
        ui->X_upButton->setEnabled(true);
        ui->X_downButton->setEnabled(true);
        ui->Y_upButton->setEnabled(true);
        ui->Y_downButton->setEnabled(true);
        ui->M_upButton->setEnabled(true);
        ui->M_downButton->setEnabled(true);
        ui->Z_upButton->setEnabled(true);
        ui->Z_downButton->setEnabled(true);
        ui->speedResetButton->setEnabled(true);
    }
    else
    {
        //关闭串口
        serialROV.close();
        //下拉使能
        ui->serialBox->setEnabled(true);
        ui->openButton->setText(QString("打开串口"));
        //按键失能
        ui->X_upButton->setEnabled(false);
        ui->X_downButton->setEnabled(false);
        ui->Y_upButton->setEnabled(false);
        ui->Y_downButton->setEnabled(false);
        ui->M_upButton->setEnabled(false);
        ui->M_downButton->setEnabled(false);
        ui->Z_upButton->setEnabled(false);
        ui->Z_downButton->setEnabled(false);
        ui->speedResetButton->setEnabled(false);
    }
}

void MainWindow::on_ROVclearButton_clicked(){
    ui->textBrowser->clear();
}

void MainWindow::on_X_upButton_clicked()
{
    if(X<7) X += 1;
    else X = 7;
    ui->Xspeed_label->setText(QString::number(X));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_X_downButton_clicked()
{
    if(X>-7) X -= 1;
    else X = -7;
    ui->Xspeed_label->setText(QString::number(X));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_Y_upButton_clicked()
{
    if(Y<7) Y += 1;
    else Y = 7;
    ui->Yspeed_label->setText(QString::number(Y));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_Y_downButton_clicked()
{
    if(Y>-7) Y -= 1;
    else Y = -7;
    ui->Yspeed_label->setText(QString::number(Y));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_M_upButton_clicked()
{
    if(M<7) M += 1;
    else M = 7;
    ui->Mspeed_label->setText(QString::number(M));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_M_downButton_clicked()
{
    if(M>-7) M -= 1;
    else M = -7;
    ui->Mspeed_label->setText(QString::number(M));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_Z_upButton_clicked()
{
    if(Z<7) Z += 1;
    else Z = 7;
    ui->Zspeed_label->setText(QString::number(Z));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_Z_downButton_clicked()
{
    if(Z>-7) Z -= 1;
    else Z = -7;
    ui->Zspeed_label->setText(QString::number(Z));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_speedResetButton_clicked()
{
    X=0;Y=0;M=0;Z=0;
    ui->Xspeed_label->setText(QString::number(X));
    ui->Yspeed_label->setText(QString::number(Y));
    ui->Mspeed_label->setText(QString::number(M));
    ui->Zspeed_label->setText(QString::number(Z));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}


//Sonic
void MainWindow::on_openSonicButton_clicked()
{
    if(ui->openSonicButton->text()==QString("打开串口"))
    {
        serialSonic.setPortName(ui->sonicBox->currentText());
        serialSonic.setBaudRate(460800);
        serialSonic.setDataBits(QSerialPort::Data8);
        serialSonic.setParity(QSerialPort::NoParity);
        serialSonic.setStopBits(QSerialPort::OneStop);
        serialSonic.setFlowControl(QSerialPort::NoFlowControl);
        //打开串口
        if(!serialSonic.open(QIODevice::ReadWrite))
        {
            QMessageBox::about(NULL, "提示", "无法打开串口！");
            return;
        }
        //下拉失能
        ui->sonicBox->setEnabled(false);
        ui->openSonicButton->setText(QString("关闭串口"));
        ui->sonicSendButton->setEnabled(true);
        ui->AButton->setEnabled(true);
        ui->DButton->setEnabled(true);
        ui->MButton->setEnabled(true);
        ui->EButton->setEnabled(true);
    }
    else
    {
        //关闭串口
        serialSonic.close();
        //下拉使能
        ui->sonicBox->setEnabled(true);
        ui->openSonicButton->setText(QString("打开串口"));
        ui->sonicSendButton->setEnabled(false);
        ui->AButton->setEnabled(false);
        ui->DButton->setEnabled(false);
        ui->MButton->setEnabled(false);
        ui->EButton->setEnabled(false);
    }
}

void MainWindow::on_sonicSendButton_clicked(){
    QString sonicSend = ui->sonicSendEdit->toPlainText();
    QByteArray sonicSendBytes = sonicSend.toUtf8();
    serialSonic.write(sonicSendBytes);
}

void MainWindow::on_AButton_clicked(){
    QString opt = "A";
    QByteArray optBytes = opt.toUtf8();
    serialSonic.write(optBytes);
}

void MainWindow::on_DButton_clicked(){
    QString opt = "D";
    QByteArray optBytes = opt.toUtf8();
    serialSonic.write(optBytes);
}

void MainWindow::on_MButton_clicked(){
    QString opt = "M";
    QByteArray optBytes = opt.toUtf8();
    serialSonic.write(optBytes);
}

void MainWindow::on_EButton_clicked(){
    QString opt = "E";
    QByteArray optBytes = opt.toUtf8();
    serialSonic.write(optBytes);
}

void MainWindow::on_clearSonicRecvButton_clicked(){
    ui->sonicRecvtextBrowser->clear();
}

void MainWindow::on_vlcSendButton_clicked(){
        char vlcsend[239];
        QString spiSend = ui->vlcSendtextEdit->toPlainText();
        QByteArray spiSendBytes = spiSend.toUtf8();
        char* tmp = spiSendBytes.data();

        strcpy(vlcsend, tmp);
        wiringPiSPIDataRW(0, (unsigned char*)vlcsend, 239);
}

void MainWindow::on_clearVLCrecvButton_clicked(){
    ui->vlcRecvtextBrowser->clear();
}
