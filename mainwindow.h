#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>

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

    void on_vlcSendButton_clicked();
    void on_clearVLCrecvButton_clicked();


private:
    Ui::MainWindow *ui;
    QSerialPort serialROV;
    QSerialPort serialSonic;
    QList<QSerialPortInfo> ports;
    int spiFd0;
    int spiFd1;
    int X=0;
    int Y=0;
    int M=0;
    int Z=0;
};

#endif // MAINWINDOW_H
