/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *X_upButton;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QPushButton *Y_upButton;
    QPushButton *M_upButton;
    QPushButton *Z_upButton;
    QPushButton *X_downButton;
    QPushButton *Y_downButton;
    QPushButton *M_downButton;
    QPushButton *Z_downButton;
    QComboBox *serialBox;
    QLabel *label_6;
    QPushButton *openButton;
    QTextBrowser *textBrowser;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *Xspeed_label;
    QLabel *Yspeed_label;
    QLabel *Mspeed_label;
    QLabel *Zspeed_label;
    QPushButton *speedResetButton;
    QComboBox *sonicBox;
    QLabel *label_9;
    QPushButton *openSonicButton;
    QLabel *label_10;
    QTextEdit *sonicSendEdit;
    QPushButton *sonicSendButton;
    QLabel *label_11;
    QTextBrowser *sonicRecvtextBrowser;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QTextEdit *vlcSendtextEdit;
    QPushButton *vlcSendButton;
    QTextBrowser *vlcRecvtextBrowser;
    QLabel *label_15;
    QLabel *sonicOrderlabel;
    QPushButton *clearSonicRecvButton;
    QPushButton *clearVLCrecvButton;
    QPushButton *ROVclearButton;
    QPushButton *AButton;
    QPushButton *DButton;
    QPushButton *MButton;
    QPushButton *EButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(797, 782);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        X_upButton = new QPushButton(centralWidget);
        X_upButton->setObjectName(QStringLiteral("X_upButton"));
        X_upButton->setGeometry(QRect(40, 100, 51, 21));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 70, 101, 23));
        QFont font;
        font.setPointSize(10);
        label->setFont(font);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(10, 100, 16, 23));
        label_2->setFont(font);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(10, 130, 16, 23));
        label_3->setFont(font);
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(10, 160, 16, 23));
        label_4->setFont(font);
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(10, 190, 16, 23));
        label_5->setFont(font);
        Y_upButton = new QPushButton(centralWidget);
        Y_upButton->setObjectName(QStringLiteral("Y_upButton"));
        Y_upButton->setGeometry(QRect(40, 130, 51, 21));
        M_upButton = new QPushButton(centralWidget);
        M_upButton->setObjectName(QStringLiteral("M_upButton"));
        M_upButton->setGeometry(QRect(40, 160, 51, 21));
        Z_upButton = new QPushButton(centralWidget);
        Z_upButton->setObjectName(QStringLiteral("Z_upButton"));
        Z_upButton->setGeometry(QRect(40, 190, 51, 21));
        X_downButton = new QPushButton(centralWidget);
        X_downButton->setObjectName(QStringLiteral("X_downButton"));
        X_downButton->setGeometry(QRect(100, 100, 51, 21));
        Y_downButton = new QPushButton(centralWidget);
        Y_downButton->setObjectName(QStringLiteral("Y_downButton"));
        Y_downButton->setGeometry(QRect(100, 130, 51, 21));
        M_downButton = new QPushButton(centralWidget);
        M_downButton->setObjectName(QStringLiteral("M_downButton"));
        M_downButton->setGeometry(QRect(100, 160, 51, 21));
        Z_downButton = new QPushButton(centralWidget);
        Z_downButton->setObjectName(QStringLiteral("Z_downButton"));
        Z_downButton->setGeometry(QRect(100, 190, 51, 21));
        serialBox = new QComboBox(centralWidget);
        serialBox->setObjectName(QStringLiteral("serialBox"));
        serialBox->setGeometry(QRect(90, 0, 61, 21));
        serialBox->setFont(font);
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(10, 0, 81, 23));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(50);
        label_6->setFont(font1);
        openButton = new QPushButton(centralWidget);
        openButton->setObjectName(QStringLiteral("openButton"));
        openButton->setGeometry(QRect(90, 30, 61, 21));
        QFont font2;
        font2.setPointSize(10);
        font2.setItalic(false);
        openButton->setFont(font2);
        textBrowser = new QTextBrowser(centralWidget);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        textBrowser->setGeometry(QRect(220, 30, 361, 221));
        QFont font3;
        font3.setPointSize(8);
        textBrowser->setFont(font3);
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(220, 0, 161, 23));
        label_7->setFont(font1);
        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(150, 70, 71, 23));
        label_8->setFont(font);
        Xspeed_label = new QLabel(centralWidget);
        Xspeed_label->setObjectName(QStringLiteral("Xspeed_label"));
        Xspeed_label->setGeometry(QRect(170, 100, 21, 23));
        Xspeed_label->setFont(font);
        Yspeed_label = new QLabel(centralWidget);
        Yspeed_label->setObjectName(QStringLiteral("Yspeed_label"));
        Yspeed_label->setGeometry(QRect(170, 130, 21, 23));
        Yspeed_label->setFont(font);
        Mspeed_label = new QLabel(centralWidget);
        Mspeed_label->setObjectName(QStringLiteral("Mspeed_label"));
        Mspeed_label->setGeometry(QRect(170, 160, 21, 23));
        Mspeed_label->setFont(font);
        Zspeed_label = new QLabel(centralWidget);
        Zspeed_label->setObjectName(QStringLiteral("Zspeed_label"));
        Zspeed_label->setGeometry(QRect(170, 190, 21, 23));
        Zspeed_label->setFont(font);
        speedResetButton = new QPushButton(centralWidget);
        speedResetButton->setObjectName(QStringLiteral("speedResetButton"));
        speedResetButton->setGeometry(QRect(50, 220, 91, 31));
        speedResetButton->setFont(font);
        sonicBox = new QComboBox(centralWidget);
        sonicBox->setObjectName(QStringLiteral("sonicBox"));
        sonicBox->setGeometry(QRect(80, 290, 61, 21));
        sonicBox->setFont(font);
        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(10, 290, 81, 23));
        label_9->setFont(font);
        openSonicButton = new QPushButton(centralWidget);
        openSonicButton->setObjectName(QStringLiteral("openSonicButton"));
        openSonicButton->setGeometry(QRect(80, 320, 61, 21));
        openSonicButton->setFont(font);
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(220, 280, 171, 23));
        QFont font4;
        font4.setPointSize(10);
        font4.setBold(false);
        font4.setWeight(50);
        label_10->setFont(font4);
        sonicSendEdit = new QTextEdit(centralWidget);
        sonicSendEdit->setObjectName(QStringLiteral("sonicSendEdit"));
        sonicSendEdit->setGeometry(QRect(220, 300, 251, 141));
        sonicSendEdit->setFont(font);
        sonicSendButton = new QPushButton(centralWidget);
        sonicSendButton->setObjectName(QStringLiteral("sonicSendButton"));
        sonicSendButton->setGeometry(QRect(220, 450, 51, 21));
        sonicSendButton->setFont(font);
        label_11 = new QLabel(centralWidget);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(490, 280, 171, 23));
        label_11->setFont(font);
        sonicRecvtextBrowser = new QTextBrowser(centralWidget);
        sonicRecvtextBrowser->setObjectName(QStringLiteral("sonicRecvtextBrowser"));
        sonicRecvtextBrowser->setGeometry(QRect(490, 300, 291, 151));
        sonicRecvtextBrowser->setFont(font3);
        label_12 = new QLabel(centralWidget);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(10, 490, 101, 23));
        label_12->setFont(font);
        label_13 = new QLabel(centralWidget);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(220, 490, 171, 23));
        label_13->setFont(font);
        label_14 = new QLabel(centralWidget);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(500, 490, 171, 23));
        label_14->setFont(font);
        vlcSendtextEdit = new QTextEdit(centralWidget);
        vlcSendtextEdit->setObjectName(QStringLiteral("vlcSendtextEdit"));
        vlcSendtextEdit->setGeometry(QRect(220, 510, 251, 151));
        vlcSendtextEdit->setFont(font);
        vlcSendButton = new QPushButton(centralWidget);
        vlcSendButton->setObjectName(QStringLiteral("vlcSendButton"));
        vlcSendButton->setGeometry(QRect(300, 660, 71, 21));
        vlcSendButton->setFont(font);
        vlcRecvtextBrowser = new QTextBrowser(centralWidget);
        vlcRecvtextBrowser->setObjectName(QStringLiteral("vlcRecvtextBrowser"));
        vlcRecvtextBrowser->setGeometry(QRect(500, 510, 281, 151));
        vlcRecvtextBrowser->setFont(font);
        label_15 = new QLabel(centralWidget);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(10, 370, 151, 23));
        label_15->setFont(font);
        sonicOrderlabel = new QLabel(centralWidget);
        sonicOrderlabel->setObjectName(QStringLiteral("sonicOrderlabel"));
        sonicOrderlabel->setGeometry(QRect(140, 370, 73, 23));
        QFont font5;
        font5.setPointSize(10);
        font5.setBold(true);
        font5.setWeight(75);
        sonicOrderlabel->setFont(font5);
        clearSonicRecvButton = new QPushButton(centralWidget);
        clearSonicRecvButton->setObjectName(QStringLiteral("clearSonicRecvButton"));
        clearSonicRecvButton->setGeometry(QRect(610, 450, 71, 21));
        clearSonicRecvButton->setFont(font);
        clearVLCrecvButton = new QPushButton(centralWidget);
        clearVLCrecvButton->setObjectName(QStringLiteral("clearVLCrecvButton"));
        clearVLCrecvButton->setGeometry(QRect(590, 660, 81, 21));
        clearVLCrecvButton->setFont(font);
        ROVclearButton = new QPushButton(centralWidget);
        ROVclearButton->setObjectName(QStringLiteral("ROVclearButton"));
        ROVclearButton->setGeometry(QRect(370, 260, 61, 21));
        ROVclearButton->setFont(font);
        AButton = new QPushButton(centralWidget);
        AButton->setObjectName(QStringLiteral("AButton"));
        AButton->setGeometry(QRect(310, 450, 31, 21));
        AButton->setFont(font);
        DButton = new QPushButton(centralWidget);
        DButton->setObjectName(QStringLiteral("DButton"));
        DButton->setGeometry(QRect(350, 450, 31, 21));
        DButton->setFont(font);
        MButton = new QPushButton(centralWidget);
        MButton->setObjectName(QStringLiteral("MButton"));
        MButton->setGeometry(QRect(390, 450, 31, 21));
        MButton->setFont(font);
        EButton = new QPushButton(centralWidget);
        EButton->setObjectName(QStringLiteral("EButton"));
        EButton->setGeometry(QRect(430, 450, 31, 21));
        EButton->setFont(font);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 797, 29));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        X_upButton->setText(QApplication::translate("MainWindow", "+", nullptr));
        label->setText(QApplication::translate("MainWindow", "ROV\346\216\247\345\210\266\345\217\260\357\274\232", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "X", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "Y", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "M", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "Z", nullptr));
        Y_upButton->setText(QApplication::translate("MainWindow", "+", nullptr));
        M_upButton->setText(QApplication::translate("MainWindow", "+", nullptr));
        Z_upButton->setText(QApplication::translate("MainWindow", "+", nullptr));
        X_downButton->setText(QApplication::translate("MainWindow", "-", nullptr));
        Y_downButton->setText(QApplication::translate("MainWindow", "-", nullptr));
        M_downButton->setText(QApplication::translate("MainWindow", "-", nullptr));
        Z_downButton->setText(QApplication::translate("MainWindow", "-", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "ROV\344\270\262\345\217\243\357\274\232", nullptr));
        openButton->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "ROV\347\212\266\346\200\201", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "\345\275\223\345\211\215\346\241\243\344\275\215", nullptr));
        Xspeed_label->setText(QApplication::translate("MainWindow", "0", nullptr));
        Yspeed_label->setText(QApplication::translate("MainWindow", "0", nullptr));
        Mspeed_label->setText(QApplication::translate("MainWindow", "0", nullptr));
        Zspeed_label->setText(QApplication::translate("MainWindow", "0", nullptr));
        speedResetButton->setText(QApplication::translate("MainWindow", "\346\241\243\344\275\215\351\207\215\347\275\256", nullptr));
        label_9->setText(QApplication::translate("MainWindow", "\346\260\264\345\243\260\344\270\262\345\217\243\357\274\232", nullptr));
        openSonicButton->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\344\270\262\345\217\243", nullptr));
        label_10->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201\347\252\227\345\217\243\357\274\232", nullptr));
        sonicSendButton->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        label_11->setText(QApplication::translate("MainWindow", "\346\216\245\346\224\266\347\252\227\345\217\243\357\274\232", nullptr));
        label_12->setText(QApplication::translate("MainWindow", "\345\205\211\351\200\232\344\277\241\346\224\266\345\217\221\357\274\232", nullptr));
        label_13->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201\347\252\227\345\217\243\357\274\232", nullptr));
        label_14->setText(QApplication::translate("MainWindow", "\346\216\245\346\224\266\347\252\227\345\217\243\357\274\232", nullptr));
        vlcSendButton->setText(QApplication::translate("MainWindow", "\345\217\221\351\200\201", nullptr));
        label_15->setText(QApplication::translate("MainWindow", "\346\260\264\345\243\260\346\216\247\345\210\266\346\214\207\344\273\244\350\247\243\346\236\220\357\274\232", nullptr));
        sonicOrderlabel->setText(QApplication::translate("MainWindow", "null", nullptr));
        clearSonicRecvButton->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244\346\216\245\346\224\266", nullptr));
        clearVLCrecvButton->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244\346\216\245\346\224\266", nullptr));
        ROVclearButton->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244\346\216\245\346\224\266", nullptr));
        AButton->setText(QApplication::translate("MainWindow", "A", nullptr));
        DButton->setText(QApplication::translate("MainWindow", "D", nullptr));
        MButton->setText(QApplication::translate("MainWindow", "M", nullptr));
        EButton->setText(QApplication::translate("MainWindow", "E", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
