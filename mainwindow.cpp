//mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "crc.h"
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
    this->setWindowTitle("UWOC Comm(v0.1)");

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
    ui->sonicSendClearButton->setEnabled(false);
    ui->looptimeEdit->setText("1000");
    ui->wumalvTimeEdit->setText("1000");
    ui->label_wumalv->setText("0.0000");
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
    while(true){
        if(digitalRead(6)){
            unsigned char vlcrecv[239]={0};
            unsigned char* tmp = vlcrecv;
            wiringPiSPIDataRW(1, vlcrecv, 239);

            // CRC
            if(ui->CRCrecvcheckBox->isChecked()){
                char* crc_recv = new char[4];
                for(int i=0; i<4; i++){
                    crc_recv[i] = vlcrecv[i];
                }
                // data
                for(int i=0; i<4; i++){
                    tmp++;
                }
                uint32_t crc_code = getCRC((char*)tmp, strlen((char*)tmp));
                char crc_cal[4];
                crc_cal[0] = crc_code >> 24;
                crc_cal[1] = crc_code >> 16;
                crc_cal[2] = crc_code >> 8;
                crc_cal[3] = crc_code;

                // 显示CRC
                QString CRC = QByteArray(crc_cal).toHex().data();
                ui->label_crcRecv->setText(CRC.mid(0,2)+" "+CRC.mid(2,2)+" "+CRC.mid(4,2)+" "+CRC.mid(6,2));
                if(strcmp(crc_recv, crc_cal)){
                    crc_check_right = true;
                }
                else crc_check_right = false;
                delete crc_recv;
            }
            else{crc_check_right = false;}

            QString spirecv = QString((char*)tmp);
            if(ui->CRCyescheckBox->isChecked()){
                if(crc_check_right){
                    ui->vlcRecvtextBrowser->append(spirecv);
                }
            }
            else ui->vlcRecvtextBrowser->append(spirecv);

            //统计误码率
            if(ui->wumalvRecvcheckBox->isChecked()){
                QString words = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
                for(int i=0; i<min(words.size(), spirecv.size());i++){
                    if(spirecv[i]==words[i]) Ycounts++;
                    Acounts++;
                }
                wumalv = 1-(Ycounts / Acounts);
                ui->label_wumalv->setText(QString::number(wumalv, 10, 6));
            }
            memset(vlcrecv, 0, 0);
         }
    }
}

void MainWindow::spi_init(){
    //初始化SPI
    if(wiringPiSetup()<0){
      QMessageBox::about(NULL, "提示", "wiringPi初始化失败！");
     }
    if(spiFd0 = wiringPiSPISetup(0,6250000)==-1){
       QMessageBox::about(NULL, "提示", "SPI0初始化失败！");
     }
    if(spiFd1 = wiringPiSPISetup(1,6250000)==-1){
       QMessageBox::about(NULL, "提示", "SPI1初始化失败！");
     }

    pinMode(6, INPUT);
    pinMode(24, OUTPUT); // 收发互斥
    pinMode(25,OUTPUT); // 复位
    digitalWrite(24, 0);
    digitalWrite(25, 0);
    sleep(1);
    digitalWrite(25, 1); // 复位脚置高

    std::thread t(&MainWindow::spi_recv, this);
    t.detach();
}

void MainWindow::serialROV_readyRead(){
    //从接收缓冲区中读取数据
    QByteArray buffer = serialROV.readAll();
    if(buffer.size()){
        //从界面中读取以前收到的数据
        QString recv = ui->textBrowser->toPlainText();
        recv += QString(buffer);
        //清空以前的显示
        ui->textBrowser->clear();
        //重新显示
        ui->textBrowser->append(recv);
    }
}

void MainWindow::serialSonic_readyRead(){
    // 接收显示
    QByteArray buffer = serialSonic.readAll();
    if(buffer.size()){
        QString show = ui->sonicRecvtextBrowser->toPlainText();
        show += QString(buffer);
        ui->sonicRecvtextBrowser->clear();
        ui->sonicRecvtextBrowser->append(show);

        // 水声控制指令解析
        QString recv = QString(buffer);
        QString orderKey = "Received String: ";
        int index = recv.indexOf(orderKey);
        if(index>=0){
            QString orderVal = recv.mid(index+17,6); //解析出的指令
            if(orderVal.startsWith("#") && orderVal.endsWith("$")){
                if(!serialROV.open(QIODevice::ReadWrite)){
                    QMessageBox::about(NULL, "提示", "指令已识别,ROV串口未打开！");
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
    }
}



//ROV
void MainWindow::on_openButton_clicked(){
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

void MainWindow::on_X_upButton_clicked(){
    if(X<7) X += 1;
    else X = 7;
    ui->Xspeed_label->setText(QString::number(X));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_X_downButton_clicked(){
    if(X>-7) X -= 1;
    else X = -7;
    ui->Xspeed_label->setText(QString::number(X));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_Y_upButton_clicked(){
    if(Y<7) Y += 1;
    else Y = 7;
    ui->Yspeed_label->setText(QString::number(Y));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_Y_downButton_clicked(){
    if(Y>-7) Y -= 1;
    else Y = -7;
    ui->Yspeed_label->setText(QString::number(Y));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_M_upButton_clicked(){
    if(M<7) M += 1;
    else M = 7;
    ui->Mspeed_label->setText(QString::number(M));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_M_downButton_clicked(){
    if(M>-7) M -= 1;
    else M = -7;
    ui->Mspeed_label->setText(QString::number(M));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_Z_upButton_clicked(){
    if(Z<7) Z += 1;
    else Z = 7;
    ui->Zspeed_label->setText(QString::number(Z));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_Z_downButton_clicked(){
    if(Z>-7) Z -= 1;
    else Z = -7;
    ui->Zspeed_label->setText(QString::number(Z));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::on_speedResetButton_clicked(){
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
void MainWindow::on_openSonicButton_clicked(){
    if(ui->openSonicButton->text()==QString("打开串口"))
    {
        serialSonic.setPortName(ui->sonicBox->currentText());
        serialSonic.setBaudRate(115200);
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
        ui->sonicSendClearButton->setEnabled(true);
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
        ui->sonicSendClearButton->setEnabled(false);
    }
}

void MainWindow::on_sonicSendButton_clicked(){
    QString sonicSend = ui->sonicSendEdit->toPlainText() + "\r\n";
    QByteArray sonicSendBytes = sonicSend.toUtf8();
    serialSonic.write(sonicSendBytes);
}

void MainWindow::on_AButton_clicked(){
    QString opt = "A\r\n";
    QByteArray optBytes = opt.toUtf8();
    serialSonic.write(optBytes);
}

void MainWindow::on_DButton_clicked(){
    QString opt = "D\r\n";
    QByteArray optBytes = opt.toUtf8();
    serialSonic.write(optBytes);
}

void MainWindow::on_MButton_clicked(){
    QString opt = "M\r\n";
    QByteArray optBytes = opt.toUtf8();
    serialSonic.write(optBytes);
}

void MainWindow::on_EButton_clicked(){
    QString opt = "E\r\n";
    QByteArray optBytes = opt.toUtf8();
    serialSonic.write(optBytes);
}

void MainWindow::on_clearSonicRecvButton_clicked(){
    ui->sonicRecvtextBrowser->clear();
}

void MainWindow::on_sonicSendClearButton_clicked(){
    ui->sonicSendEdit->clear();
}



//VLC
QString gettime(){
      time_t rawtime;
      struct tm *ptminfo;
      time(&rawtime);
      ptminfo = localtime(&rawtime);

      QString sec;
      if(ptminfo->tm_sec < 10){
          sec = "0" + QString::number(ptminfo->tm_sec);
      }
      else sec = QString::number(ptminfo->tm_sec);

      QString min;
      if(ptminfo->tm_min < 10){
          min = "0" + QString::number(ptminfo->tm_min);
      }
      else min = QString::number(ptminfo->tm_min);

      QString hour;
      if(ptminfo->tm_hour < 10){
          hour = "0" + QString::number(ptminfo->tm_hour);
      }
      else hour = QString::number(ptminfo->tm_hour);

      QString day;
      if(ptminfo->tm_mday < 10){
          day = "0" + QString::number(ptminfo->tm_mday);
      }
      else day = QString::number(ptminfo->tm_mday);

      QString mon;
      if(ptminfo->tm_mon +1 < 10){
          mon = "0" + QString::number(ptminfo->tm_mon+1);
      }
      else mon = QString::number(ptminfo->tm_mon+1);

      QString year = QString::number(ptminfo->tm_year + 1900);
      return year+"-"+mon+"-"+day+" "+hour+":"+min+":"+sec+"\n";
  }

void MainWindow::on_vlcSendButton_clicked(){
    if(ui->ParadoxcheckBox->isChecked()){
        digitalWrite(24, 1);
    }

    char vlcsend[239]={0};
    QString spiSend;
    if(ui->timecheckBox->isChecked()){
        spiSend = gettime() + ui->vlcSendtextEdit->toPlainText();
    }
    else spiSend = ui->vlcSendtextEdit->toPlainText();

    QByteArray spiSendBytes = spiSend.toUtf8();
    char* tmp = spiSendBytes.data();

    // add crc
    if(ui->CRCcheckBox->isChecked()){
        uint32_t crc_code = getCRC(tmp, strlen(tmp));
        vlcsend[0] = crc_code >> 24;
        vlcsend[1] = crc_code >> 16;
        vlcsend[2] = crc_code >> 8;
        vlcsend[3] = crc_code;
        QString CRC = QByteArray(vlcsend).toHex().data();
        ui->label_crcSend->setText(CRC.mid(0,2)+" "+CRC.mid(2,2)+" "+CRC.mid(4,2)+" "+CRC.mid(6,2));

        strcat(vlcsend, tmp);
    }
    else strcpy(vlcsend, tmp);

    wiringPiSPIDataRW(0, (unsigned char*)vlcsend, 239);
    if(ui->ParadoxcheckBox->isChecked()){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        digitalWrite(24, 0);
    }
}

void MainWindow::start_spi_LoopThread(){
    if(!spi_send_loop){
           spi_send_loop = true;
           std::thread th(std::bind(&MainWindow::spi_loopSend,this));
           th.detach();
       }
}

void MainWindow::stop_spi_LoopThread(){
    spi_send_loop = false;
}

void MainWindow::spi_loopSend(){
    if(ui->ParadoxcheckBox->isChecked()){
        digitalWrite(24, 1);
    }
    while(spi_send_loop){
        QString intervaltime = ui->looptimeEdit->text();
        int interval = intervaltime.toInt();

        char vlcsend[239]={0};
        QString spiSend;
        if(ui->timecheckBox->isChecked()){
            spiSend = gettime() + ui->vlcSendtextEdit->toPlainText();
        }
        else spiSend = ui->vlcSendtextEdit->toPlainText();

        QByteArray spiSendBytes = spiSend.toUtf8();
        char* tmp = spiSendBytes.data();

        // CRC
        if(ui->CRCcheckBox->isChecked()){
            uint32_t crc_code = getCRC(tmp, strlen(tmp));
            vlcsend[0] = crc_code >> 24;
            vlcsend[1] = crc_code >> 16;
            vlcsend[2] = crc_code >> 8;
            vlcsend[3] = crc_code;
            QString CRC = QByteArray(vlcsend).toHex().data();
            ui->label_crcSend->setText(CRC.mid(0,2)+" "+CRC.mid(2,2)+" "+CRC.mid(4,2)+" "+CRC.mid(6,2));

            strcat(vlcsend, tmp);
        }
        else strcpy(vlcsend, tmp);

        wiringPiSPIDataRW(0, (unsigned char*)vlcsend, 239);
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
    if(ui->ParadoxcheckBox->isChecked()){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        digitalWrite(24, 0);
    }
}

void MainWindow::on_loopButton_clicked(){
    if(ui->loopButton->text() == "启动定时发送"){
        ui->loopButton->setText(tr("停止定时发送"));
        ui->vlcSendButton->setEnabled(false);
        ui->looptimeEdit->setEnabled(false);
        start_spi_LoopThread();
       }
    else{
        ui->loopButton->setText(tr("启动定时发送"));
        ui->vlcSendButton->setEnabled(true);
        ui->looptimeEdit->setEnabled(true);
        stop_spi_LoopThread();
       }
}

void MainWindow::on_clearVLCrecvButton_clicked(){
    ui->vlcRecvtextBrowser->clear();
    ui->label_crcRecv->clear();
}

void MainWindow::on_vlcRecvtextBrowser_textChanged(){
    ui->vlcRecvtextBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::on_vlcSendClearButton_clicked(){
    ui->vlcSendtextEdit->clear();
    ui->label_crcSend->clear();
}



// VLC误码率
void MainWindow::wumalv_loopSend(){
    if(ui->ParadoxcheckBox->isChecked()){
        digitalWrite(24, 1);
    }
    while(wumalv_send_loop){
        QString wumalvtime = ui->wumalvTimeEdit->text();
        int interval = wumalvtime.toInt();
        char vlcsend[239]={0};
        QString spiSend = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        QByteArray spiSendBytes = spiSend.toUtf8();
        char* tmp = spiSendBytes.data();

        // CRC
        if(ui->CRCcheckBox->isChecked()){
            uint32_t crc_code = getCRC(tmp, strlen(tmp));
            vlcsend[0] = crc_code >> 24;
            vlcsend[1] = crc_code >> 16;
            vlcsend[2] = crc_code >> 8;
            vlcsend[3] = crc_code;
            QString CRC = QByteArray(vlcsend).toHex().data();
            ui->label_crcSend->setText(CRC.mid(0,2)+" "+CRC.mid(2,2)+" "+CRC.mid(4,2)+" "+CRC.mid(6,2));
            strcat(vlcsend, tmp);
        }
        else strcpy(vlcsend, tmp);

        wiringPiSPIDataRW(0, (unsigned char*)vlcsend, 239);
        wumalv_sendnums++;
        ui->label_wumalv_sendnums->setText(QString::number(wumalv_sendnums));
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
    if(ui->ParadoxcheckBox->isChecked()){
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        digitalWrite(24, 0);
    }
}

void MainWindow::on_wumalvButton_clicked(){
    if(ui->wumalvButton->text()==QString("启动误码率探测")){
        ui->wumalvButton->setText(tr("停止误码率探测"));
        ui->clearwumalvSendNumsButton->setEnabled(false);
        ui->wumalvTimeEdit->setEnabled(false);
        ui->clearWumalvButton->setEnabled(false);
        if(!wumalv_send_loop){
               wumalv_send_loop = true;
               std::thread th(std::bind(&MainWindow::wumalv_loopSend,this));
               th.detach();
           }
    }
    else{
        ui->wumalvButton->setText(tr("启动误码率探测"));
        ui->clearwumalvSendNumsButton->setEnabled(true);
        ui->wumalvTimeEdit->setEnabled(true);
        ui->clearWumalvButton->setEnabled(true);
        wumalv_send_loop = false;
    }
}

void MainWindow::on_clearWumalvButton_clicked(){
    wumalv = 0;
    Acounts = 0;
    Ycounts = 0;
    ui->label_wumalv->setText("0.0000");
}

void MainWindow::on_clearwumalvSendNumsButton_clicked(){
    wumalv_sendnums = 0;
    ui->label_wumalv_sendnums->setText(QString::number(wumalv_sendnums));
}




