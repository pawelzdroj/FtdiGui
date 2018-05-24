#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QDebug>
#include <QThread>



#include "ftd2xx.h"
#include "ftdi.h"
#include "txftdi.h"
#include "ftdi_str.h"



#define PID_VID_LENGTH 8
#define VID_STRING "VID_0403"
#define PID_STRING "PID_6010"

#define NUM_OF_SUPPORTED_DEVICES 2
extern QString ACCEPTED_SERIALNUMBERS[];

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool nativeEvent(const QByteArray& eventType, void* message, long* result) Q_DECL_OVERRIDE;
    bool matchVidPid(QString dbcc_name);
    bool detectDevices();
    char supportedDevice(char serialNumber[16]);


    Ftdi* ftdiChA;
    Ftdi* ftdiChB;

    char dataToSend[2];


private:
    Ui::MainWindow *ui;

    QString errString = "";



public slots:
    QString errorString(QString) const;
    void handleSendButton();

private slots:


signals:
    void ftdiArrived();
    void ftdiDisconnected();
    void sendData(char*, unsigned long);



};

#endif // MAINWINDOW_H
