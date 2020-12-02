#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QFile>
#include <QBuffer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void serialport_refresh();
    void spi_init();
    void spi_recv();
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void serialROV_readyRead();
    void serialSonic_readyRead();
    void on_openButton_clicked();
    void on_ROVclearButton_clicked();

    void on_X_upButton_clicked();
    void on_X_downButton_clicked();
    void on_Y_upButton_clicked();
    void on_Y_downButton_clicked();
    void on_M_upButton_clicked();
    void on_M_downButton_clicked();
    void on_Z_upButton_clicked();
    void on_Z_downButton_clicked();
    void on_speedResetButton_clicked();

    void on_openSonicButton_clicked();
    void on_sonicSendButton_clicked();
    void on_AButton_clicked();
    void on_DButton_clicked();
    void on_MButton_clicked();
    void on_EButton_clicked();
    void on_clearSonicRecvButton_clicked();
    void on_sonicSendClearButton_clicked();

    void on_vlcSendButton_clicked();
    void on_loopButton_clicked();
    void spi_loopSend();
    void start_spi_LoopThread();
    void stop_spi_LoopThread();
    void on_clearVLCrecvButton_clicked();
    void on_vlcSendClearButton_clicked();
    void on_vlcRecvtextBrowser_textChanged();

    void on_wumalvButton_clicked();
    void wumalv_loopSend();
    void on_clearWumalvButton_clicked();
    void on_clearwumalvSendNumsButton_clicked();

    void on_startSpiRecvButton_clicked();
    void on_selectFileButton_clicked();
    void on_sendFileButton_clicked();
    void sendFile();
    void updateProgressSend();
//    void updateProgressRecv();

    void on_X_sonic_upButton_clicked();
    void on_X_sonic_downButton_clicked();
    void on_Y_sonic_upButton_clicked();
    void on_Y_sonic_downButton_clicked();
    void on_M_sonic_upButton_clicked();
    void on_M_sonic_downButton_clicked();
    void on_Z_sonic_upButton_clicked();
    void on_Z_sonic_downButton_clicked();
    void on_speedreset_sonicButton_clicked();

    void updateTime();
    void updateTemp();

    void write24zero();
    void vlcsend(char* vlcsend);

private:
    Ui::MainWindow *ui;
    QSerialPort serialROV;
    QSerialPort serialSonic;
    QList<QSerialPortInfo> ports;
    qint32 spiFd0;
    qint32 spiFd1;
    int X=0;
    int Y=0;
    int M=0;
    int Z=0;
    bool spiRecv = false;
    bool crc_check_right = false;
    bool spi_send_loop = false;
    bool wumalv_send_loop = false;
    bool wumalv_recv_on = false;
    double wumalv = 0;
    double Acounts = 0;
    double Ycounts = 0;
    int wumalv_sendnums = 0;

    QFile SendFile;
    QString SendFileName;
    qint64 SendFileSize;
    qint64 sendSize;
    bool file_send_done = false;

    QFile RecvFile;
    QString RecvFileName;
    qint64 RecvFileSize;
    qint64 recvSize;
    bool filehead_done = false;
    bool file_recv_done = false;
    QDataStream* recvStream;
};

#endif // MAINWINDOW_H
