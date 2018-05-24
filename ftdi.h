#ifndef FTDI_H
#define FTDI_H

#include <QWidget>
#include <QDebug>

#include "ftd2xx.h"

#include "rxftdi.h"
#include "txftdi.h"

class Ftdi : public QWidget
{
    Q_OBJECT
public:
    explicit Ftdi(QWidget *parent = nullptr);
    ~Ftdi();

    //device info fields:
    DWORD ftFlags;
    DWORD ftType;
    DWORD deviceID;
    DWORD ftLocId;
    char serialNumber[16];
    char description[64];
    FT_HANDLE ftHandle;

    bool ftOpened;

    QThread* RxThread;
    QThread* TxThread;
    RxFTDI* Rx;
    TxFTDI* Tx;

    bool createRxTxThreads();



private:

    FT_DEVICE ftDevice;
    FT_STATUS ftStatus;

    UCHAR Mask = 0xff;
    UCHAR Mode;

signals:

public slots:

    bool ftdiOpen();
    bool quitRxTxThreads();

};

#endif // FTDI_H
