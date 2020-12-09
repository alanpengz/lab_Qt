//mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "crc.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QBuffer>
#include <QDateTime>
#include <QTextStream>
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
    this->setWindowTitle("UWOC Comm(v0.3.0)");

    spi_init();
    //连接serialROV serialSonic信号和槽
    QObject::connect(&serialROV, &QSerialPort::readyRead, this, &MainWindow::serialROV_readyRead);
    QObject::connect(&serialSonic, &QSerialPort::readyRead, this, &MainWindow::serialSonic_readyRead);

    std::thread t1(&MainWindow::serialport_refresh, this);
    t1.detach();

    std::thread t2(&MainWindow::updateTime, this);
    t2.detach();

    std::thread t3(&MainWindow::updateTemp, this);
    t3.detach();

    std::thread t4(&MainWindow::update_VLC_bitrate, this);
    t4.detach();

    ui->X_upButton->setEnabled(false);
    ui->X_downButton->setEnabled(false);
    ui->Y_upButton->setEnabled(false);
    ui->Y_downButton->setEnabled(false);
    ui->M_upButton->setEnabled(false);
    ui->M_downButton->setEnabled(false);
    ui->Z_upButton->setEnabled(false);
    ui->Z_downButton->setEnabled(false);
    ui->speedResetButton->setEnabled(false);
    ui->X_sonic_upButton->setEnabled(false);
    ui->X_sonic_downButton->setEnabled(false);
    ui->Y_sonic_upButton->setEnabled(false);
    ui->Y_sonic_downButton->setEnabled(false);
    ui->M_sonic_upButton->setEnabled(false);
    ui->M_sonic_downButton->setEnabled(false);
    ui->Z_sonic_upButton->setEnabled(false);
    ui->Z_sonic_downButton->setEnabled(false);
    ui->speedreset_sonicButton->setEnabled(false);

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
    ui->label_wumalv->setText("0");
    ui->sendFileButton->setEnabled(false);
    ui->ParadoxcheckBox->setEnabled(false);
    ui->rovRecvcheckBox->setChecked(true);
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

void MainWindow::on_startSpiRecvButton_clicked(){
    if(ui->startSpiRecvButton->text()==QString("开启接收")){
        ui->startSpiRecvButton->setText(QString("关闭接收"));
        if(!spiRecv){
            digitalWrite(24, 0);
            ui->ParadoxcheckBox->setEnabled(true);
        }
    }
    else{
        ui->startSpiRecvButton->setText(QString("开启接收"));
        digitalWrite(24, 1);
        ui->ParadoxcheckBox->setEnabled(false);
    }
}

void MainWindow::spi_recv(){
    while(true){
        if(digitalRead(6)){
            if(ui->modeRecvcomboBox->currentText()=="字符"){
                unsigned char vlcrecv[239]={0};
                unsigned char* tmp = vlcrecv;
                wiringPiSPIDataRW(1, vlcrecv, 239);

                // CRC
                if(ui->CRCrecvcheckBox->isChecked()){
                    char crc_recv[4] = {0};
                    for(int i=0; i<4; i++){
                        crc_recv[i] = vlcrecv[i];
                    }
                    // data
                    for(int i=0; i<4; i++){
                        tmp++;
                    }
                    uint32_t crc_code = getCRC((char*)tmp, strlen((char*)tmp));
                    char crc_cal[4]={0};
                    crc_cal[0] = crc_code >> 24;
                    crc_cal[1] = crc_code >> 16;
                    crc_cal[2] = crc_code >> 8;
                    crc_cal[3] = crc_code;

                    // 显示CRC
                    QString CRC_REC = QByteArray(crc_recv).toHex().data();
                    QString CRC_CAL = QByteArray(crc_cal).toHex().data();
                    ui->label_crcRecv->setText( "crc_recv: "+ CRC_REC.mid(0,2)+" "+CRC_REC.mid(2,2)+" "+CRC_REC.mid(4,2)+" "+CRC_REC.mid(6,2)+", crc_cal: " + CRC_CAL.mid(0,2)+" "+CRC_CAL.mid(2,2)+" "+CRC_CAL.mid(4,2)+" "+CRC_CAL.mid(6,2));
                    if(CRC_REC.mid(0,8) == CRC_CAL.mid(0,8)){
                        crc_check_right = true;
                    }
                    else crc_check_right = false;
                }
                else{
                    crc_check_right = false;
                }

                QString spirecv = QString((char*)tmp);
                if(ui->CRCyescheckBox->isChecked()){
                    if(crc_check_right){
                        ui->vlcRecvtextBrowser->append(spirecv);
                    }
                }
                else ui->vlcRecvtextBrowser->append(spirecv);

                //统计误码率
                if(ui->wumalvRecvcheckBox->isChecked()){
                    if(spirecv.size()){
                        QString words_bits = "0011000100110010001100110011010000110101001101100011011100111000001110010011000001100001011000100110001101100100011001010110011001100111011010000110100101101010011010110110110001101101011011100110111101110000011100010111001001110011011101000111010101110110011101110111100001111001011110100100000101000010010000110100010001000101010001100100011101001000010010010100101001001011010011000100110101001110010011110101000001010001010100100101001101010100010101010101011001010111010110000101100101011010";
                        QString words = "1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
                        QString recv_bits = QString::fromStdString(StrToBitStr(spirecv.toStdString()));
                        for(int i=0; i<min(words.size(), spirecv.size());i++){
                            if(spirecv[i]==words[i]) Ycounts++;
                            Acounts++;
                        }
                        for(int j=0; j<min(words_bits.size(), recv_bits.size());j++){
                            if(words_bits[j]==recv_bits[j]) Ycounts_bits++;
                            Acounts_bits++;
                        }

                        wumalv = 1-(Ycounts / Acounts);
                        ber = 1-(Ycounts_bits / Acounts_bits);
                        ui->label_wumalv->setText(QString::number(wumalv, 10, 6));
                        ui->label_ber->setText(QString::number(ber, 10, 6));
                    }
                }
                if(spirecv.size()>10000){
                    ui->vlcRecvtextBrowser->clear();
                }
            }

            if(ui->modeRecvcomboBox->currentText()=="文件"){
//                unsigned char vlcrecv[239]={0};
                quint8 vlcrecv[239]={0};
                wiringPiSPIDataRW(1, vlcrecv, 239);
                quint8* tmp = vlcrecv;

                // 文件头信息
                if(!filehead_done){
                    RecvFileName = QString((char*)vlcrecv).section("##",0,0);
                    RecvFileSize = QString((char*)vlcrecv).section("##",1,1).toInt();
                    recvSize=0;

                    QString recvFileHeadInfo = "新文件开始接收! 文件名:" + RecvFileName + " 文件大小:" + QString::number(RecvFileSize);
                    ui->vlcRecvtextBrowser->append(recvFileHeadInfo);
                    RecvFile.setFileName("/home/pi/Desktop/" + RecvFileName);
                    if(!RecvFile.open(QIODevice::WriteOnly)){
                        QMessageBox::about(NULL, "提示", "文件写入错误");
                        return;
                    }
                    filehead_done = true;
                    recvStream = new QDataStream(&RecvFile);
                    recvStream->setVersion(QDataStream::Qt_5_7);
                }
                // 文件数据写入
                else{
                    qint64 len;
                    // 最后一块数据
                    if(RecvFileSize-recvSize<239){
                        quint8 zeros = 0;
                        quint8 i = 238;
                        while(vlcrecv[i]==0){
                            i--;
                            zeros++;
                        }
                        len = recvStream->writeRawData((char*)vlcrecv, sizeof(vlcrecv)/sizeof(quint8)-zeros);
                    }
                    else len = recvStream->writeRawData((char*)vlcrecv, sizeof(vlcrecv)/sizeof(quint8));
//                    qint64 len = RecvFile.write((char*)vlcrecv);
                    if(len<0){
                        QMessageBox::about(NULL, "提示", "文件写入失败");
                        return;
                    }
                    else recvSize += len;
                    // 文件接收完成
                    if(recvSize >= RecvFileSize){
                        QString recvFileInfo = "文件接收完成! 文件保存路径:/home/pi/Desktop/ 文件名:" + RecvFileName + " 文件大小:" + QString::number(RecvFileSize);
                        ui->vlcRecvtextBrowser->append(recvFileInfo);
                        file_recv_done = true;
                        RecvFile.close();
                        RecvFileName.clear();
                        filehead_done = false;
                        RecvFileSize=0;
                        recvSize=0;
                        delete recvStream;
                    }
                }
            }
         }
    }
}

void MainWindow::spi_init(){
    //初始化SPI
    if(wiringPiSetup()<0){
      QMessageBox::about(NULL, "提示", "wiringPi初始化失败！");
     }
    QString spiRateSend = ui->spiRateSendcomboBox->currentText();
    QString spiRateRecv = ui->spiRateRecvcomboBox->currentText();
    if(spiFd0 = wiringPiSPISetup(0,spiRateSend.toInt())==-1){
       QMessageBox::about(NULL, "提示", "SPI0初始化失败！");
     }
    if(spiFd1 = wiringPiSPISetup(1,spiRateRecv.toInt())==-1){
       QMessageBox::about(NULL, "提示", "SPI1初始化失败！");
     }

    pinMode(6, INPUT);
    pinMode(24, OUTPUT); // 收发互斥
    pinMode(25,OUTPUT); // 复位
    digitalWrite(24, 1); // 默认不开启接收
    digitalWrite(25, 0);
    sleep(1);
    digitalWrite(25, 1); // 复位脚置高

    // vlc FPGA bitrate, dafault 1.25Mbps, (0,0)
    pinMode(22, OUTPUT);
    pinMode(23, OUTPUT);
    digitalWrite(22, 0);
    digitalWrite(23, 0);

    std::thread t(&MainWindow::spi_recv, this);
    t.detach();
}

void MainWindow::serialROV_readyRead(){
    //从接收缓冲区中读取数据
    QByteArray buffer = serialROV.readAll();
    if(ui->rovRecvcheckBox->isChecked()){
        if(buffer.size()){
            QString text = ui->textBrowser->toPlainText();
            ui->textBrowser->clear();
            QString recv = text + QString(buffer);
            //重新显示
            ui->textBrowser->append(recv);
            if(recv.size()>500){
                ui->textBrowser->clear();
            }
        }
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
        ROV_is_open = true;
        //循环更新rov速度
//        std::thread t5(std::bind(&MainWindow::updateROVSpeed, this));
//        t5.detach();

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
        ROV_is_open = false;
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
        ui->X_sonic_upButton->setEnabled(true);
        ui->X_sonic_downButton->setEnabled(true);
        ui->Y_sonic_upButton->setEnabled(true);
        ui->Y_sonic_downButton->setEnabled(true);
        ui->M_sonic_upButton->setEnabled(true);
        ui->M_sonic_downButton->setEnabled(true);
        ui->Z_sonic_upButton->setEnabled(true);
        ui->Z_sonic_downButton->setEnabled(true);
        ui->speedreset_sonicButton->setEnabled(true);
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
        ui->X_sonic_upButton->setEnabled(false);
        ui->X_sonic_downButton->setEnabled(false);
        ui->Y_sonic_upButton->setEnabled(false);
        ui->Y_sonic_downButton->setEnabled(false);
        ui->M_sonic_upButton->setEnabled(false);
        ui->M_sonic_downButton->setEnabled(false);
        ui->Z_sonic_upButton->setEnabled(false);
        ui->Z_sonic_downButton->setEnabled(false);
        ui->speedreset_sonicButton->setEnabled(false);
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

void MainWindow::on_X_sonic_upButton_clicked(){
    if(X<7) X += 1;
    else X = 7;
    ui->Xspeed_label->setText(QString::number(X));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$" + "\r\n";
    QByteArray order_send = order.toUtf8();
    QString M = "M\r\n";
    QByteArray MBytes = M.toUtf8();
    serialSonic.write(MBytes);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    serialSonic.write(order_send);
}

void MainWindow::on_X_sonic_downButton_clicked(){
    if(X>-7) X -= 1;
    else X = -7;
    ui->Xspeed_label->setText(QString::number(X));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$" + "\r\n";
    QByteArray order_send = order.toUtf8();
    QString M = "M\r\n";
    QByteArray MBytes = M.toUtf8();
    serialSonic.write(MBytes);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    serialSonic.write(order_send);
}

void MainWindow::on_Y_sonic_upButton_clicked(){
    if(Y<7) Y += 1;
    else Y = 7;
    ui->Yspeed_label->setText(QString::number(Y));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$" + "\r\n";
    QByteArray order_send = order.toUtf8();
    QString M = "M\r\n";
    QByteArray MBytes = M.toUtf8();
    serialSonic.write(MBytes);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    serialSonic.write(order_send);
}

void MainWindow::on_Y_sonic_downButton_clicked(){
    if(Y>-7) Y -= 1;
    else Y = -7;
    ui->Yspeed_label->setText(QString::number(Y));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$" + "\r\n";
    QByteArray order_send = order.toUtf8();
    QString M = "M\r\n";
    QByteArray MBytes = M.toUtf8();
    serialSonic.write(MBytes);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    serialSonic.write(order_send);
}

void MainWindow::on_M_sonic_upButton_clicked(){
    if(M<7) M += 1;
    else M = 7;
    ui->Mspeed_label->setText(QString::number(M));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$" + "\r\n";
    QByteArray order_send = order.toUtf8();
    QString M = "M\r\n";
    QByteArray MBytes = M.toUtf8();
    serialSonic.write(MBytes);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    serialSonic.write(order_send);
}

void MainWindow::on_M_sonic_downButton_clicked(){
    if(M>-7) M -= 1;
    else M = -7;
    ui->Mspeed_label->setText(QString::number(M));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$" + "\r\n";
    QByteArray order_send = order.toUtf8();
    QString M = "M\r\n";
    QByteArray MBytes = M.toUtf8();
    serialSonic.write(MBytes);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    serialSonic.write(order_send);
}

void MainWindow::on_Z_sonic_upButton_clicked(){
    if(Z<7) Z += 1;
    else Z = 7;
    ui->Zspeed_label->setText(QString::number(Z));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$" + "\r\n";
    QByteArray order_send = order.toUtf8();
    QString M = "M\r\n";
    QByteArray MBytes = M.toUtf8();
    serialSonic.write(MBytes);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    serialSonic.write(order_send);
}

void MainWindow::on_Z_sonic_downButton_clicked(){
    if(Z>-7) Z -= 1;
    else Z = -7;
    ui->Zspeed_label->setText(QString::number(Z));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$" + "\r\n";
    QByteArray order_send = order.toUtf8();
    QString M = "M\r\n";
    QByteArray MBytes = M.toUtf8();
    serialSonic.write(MBytes);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    serialSonic.write(order_send);
}

void MainWindow::on_speedreset_sonicButton_clicked(){
    X=0;Y=0;M=0;Z=0;
    ui->Xspeed_label->setText(QString::number(X));
    ui->Yspeed_label->setText(QString::number(Y));
    ui->Mspeed_label->setText(QString::number(M));
    ui->Zspeed_label->setText(QString::number(Z));
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$" + "\r\n";
    QByteArray order_send = order.toUtf8();
    QString M = "M\r\n";
    QByteArray MBytes = M.toUtf8();
    serialSonic.write(MBytes);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    serialSonic.write(order_send);
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

void MainWindow::write24zero(){
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    digitalWrite(24, 0);
}

void MainWindow::vlcsend(char* vlcsend){
    wiringPiSPIDataRW(0, (unsigned char*)vlcsend, 239);
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

    std::thread th1(std::bind(&MainWindow::vlcsend,this, vlcsend));
    th1.join();
//    wiringPiSPIDataRW(0, (unsigned char*)vlcsend, 239);

    if(ui->ParadoxcheckBox->isChecked()){
        std::thread th(std::bind(&MainWindow::write24zero,this));
        th.detach();
//        std::this_thread::sleep_for(std::chrono::milliseconds(500));
//        digitalWrite(24, 0);
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
        std::thread th(std::bind(&MainWindow::write24zero,this));
        th.detach();
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
    ui->label_wumalv->setText("0");
    ui->label_ber->setText("0");
    ber = 0;
    Ycounts_bits = 0;
    Ycounts_bits = 0;
    wumalv = 0;
    Acounts = 0;
    Ycounts = 0;
}

void MainWindow::on_clearwumalvSendNumsButton_clicked(){
    wumalv_sendnums = 0;
    ui->label_wumalv_sendnums->setText(QString::number(wumalv_sendnums));
}



// 文件传输
void MainWindow::on_selectFileButton_clicked(){
    QString filePath=QFileDialog::getOpenFileName(this,"选择文件","../");
    if(!filePath.isEmpty()){
        SendFileName.clear();
        SendFileSize=0;
        sendSize=0;
        file_send_done = false;

        //获取文件信息
        QFileInfo info(filePath);
        SendFileName=info.fileName();//获取文件名字
        SendFileSize=info.size();
        sendSize=0;
        //只读方式打开文件
        SendFile.setFileName(filePath);
        bool isOk=SendFile.open(QIODevice::ReadOnly);
        if(!isOk){
            QMessageBox::about(NULL, "提示", "只读方式打开文件失败");
            return;
        }
        QString fileinfo = "文件名:" + SendFileName + " 文件大小:" + QString::number(SendFileSize);
        ui->label_fileinfo->setText(fileinfo);
        ui->sendFileButton->setEnabled(true);
    }
    else{
        QMessageBox::about(NULL, "提示", "选择文件路径出错");
        return;
    }
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(SendFileSize);
    ui->progressBar->setValue(0);
}

void MainWindow::updateProgressSend(){
    while(!file_send_done){
        ui->progressBar->setValue(sendSize);
    }
    ui->progressBar->setValue(SendFileSize);
}

void MainWindow::sendFile(){
    //先发送文件头信息
    QString head=QString("%1##%2").arg(SendFileName).arg(SendFileSize);
    char filehead[239] = {0};
    char* tmphead = head.toUtf8().data();
    strcpy(filehead, tmphead);
    wiringPiSPIDataRW(0, (unsigned char*)filehead, 239);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    QDataStream sendStream(&SendFile);
    sendStream.setVersion(QDataStream::Qt_5_9);
    //发送文件
    while(!file_send_done){
        quint8 filedata[239] = {0};
        qint64 len = sendStream.readRawData((char*)filedata, 239);
//            char filedata[239] = {0};
//            qint64 len = SendFile.read(filedata, 239);
        if(len<0){
            QMessageBox::about(NULL, "提示", "文件读取失败");
            return;
        }
        wiringPiSPIDataRW(0, filedata, 239);
        sendSize += len;

        if(sendSize == SendFileSize) file_send_done = true;
        this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    SendFile.close();
}

void MainWindow::on_sendFileButton_clicked(){
    std::thread th1(std::bind(&MainWindow::sendFile,this));
    th1.detach();

    std::thread th2(std::bind(&MainWindow::updateProgressSend,this));
    th2.detach();
}



// 其他信息
void MainWindow::updateTime(){
    while(true){
        QDateTime dateTime = QDateTime::currentDateTime();
        QLocale locale = QLocale::Chinese;//指定中文显示
        //QLocale locale = QLocale::English;//指定英文显示
        QString strFormat = "当前时间：yyyy-MM-dd hh:mm:ss dddd";
        QString strDateTime = locale.toString(dateTime, strFormat);
        ui->label_currentdatetime->setText(strDateTime);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void MainWindow::updateTemp(){
    while(true){
        QFile* temp = new QFile("/sys/class/thermal/thermal_zone0/temp");
        if (!temp->open(QIODevice::ReadOnly|QIODevice::Text)){
            QMessageBox::about(NULL, "提示", "温度读取失败");
            return;
        }
        else{
            QTextStream stream(temp);
            QString string = stream.readAll();
            temp->close();
            long s = string.toLong() / 1000;
            QString tempinfo = "CPU温度:" + QString::number(s) + "℃";
            ui->label_currentTemp->setText(tempinfo);
        }
        std::this_thread::sleep_for(std::chrono::seconds(3));
        delete temp;
    }
}

void MainWindow::update_VLC_bitrate(){
    QString old_bitrate = ui->vlcBitRatecomboBox->currentText();
    while(true){
        QString bitrate = ui->vlcBitRatecomboBox->currentText();
        if(bitrate!=old_bitrate){
            if(bitrate=="1.25Mbps"){
                digitalWrite(22, 0);
                digitalWrite(23, 0);
            }
            if(bitrate=="2Mbps"){
                digitalWrite(22, 1);
                digitalWrite(23, 0);
            }
            if(bitrate=="3.125Mbps"){
                digitalWrite(22, 0);
                digitalWrite(23, 1);
            }
            if(bitrate=="5Mbps"){
                digitalWrite(22, 1);
                digitalWrite(23, 1);
            }
            old_bitrate = bitrate;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void MainWindow::updateROVSpeed(){
    while(ROV_is_open){
        send_rov_order();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void MainWindow::send_rov_order(){
    QString order =  "#" + orderMap[QString::number(X)] + orderMap[QString::number(Y)]
            + orderMap[QString::number(M)] + orderMap[QString::number(Z)] + "$";
    QByteArray order_send = order.toUtf8();
    serialROV.write(order_send);
}

void MainWindow::autodrive(){
    bool alignment = false;
    while ((!alignment)){
        M = -2;//右转
        ui->Mspeed_label->setText(QString::number(M));
        send_rov_order();

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        QString text = ui->vlcRecvtextBrowser->toPlainText();
        //经历对准时刻
        if(text.contains("0")){
            M = 0;//停转
            ui->Mspeed_label->setText(QString::number(M));
            send_rov_order();

            ui->vlcRecvtextBrowser->clear();//清除接收框内容，等待3s确定是否对准
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            text = ui->vlcRecvtextBrowser->toPlainText();
            //已对准
            if(text.contains("0")){
                alignment = true;
                ui->autodriveButton->setText(tr("开启自动寻找光源"));
            }
            //由于惯性转过了头
            else{
                while((!alignment)){
                    M = 2;//左转
                    ui->Mspeed_label->setText(QString::number(M));
                    send_rov_order();
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    //经历对准时刻
                    text = ui->vlcRecvtextBrowser->toPlainText();
                    if(text.contains("0")){
                        M = 0;//停转
                        ui->Mspeed_label->setText(QString::number(M));
                        send_rov_order();
                        ui->vlcRecvtextBrowser->clear();//清除接收框内容，等待3s确定是否对准
                        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                        text = ui->vlcRecvtextBrowser->toPlainText();
                        if (text.contains("0"))//已对准
                        {
                            alignment = true;
                            ui->autodriveButton->setText(tr("开启自动寻找光源"));
                        }
                        else break;
                    }
               }
           }
       }
   }
}

void MainWindow::on_autodriveButton_clicked(){
    if(ui->autodriveButton->text()==QString("开启自动寻找光源")){
        ui->autodriveButton->setText(tr("关闭自动寻找光源"));
        std::thread t(std::bind(&MainWindow::autodrive,this));
        t.detach();
    }
}
