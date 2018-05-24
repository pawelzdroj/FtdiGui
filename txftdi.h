#ifndef TXFTDI_H
#define TXFTDI_H

#include <QObject>
#include <QDebug>
#include <QThread>

#include "ftd2xx.h"



class TxFTDI : public QObject
{
    Q_OBJECT

public:
    TxFTDI();
    ~TxFTDI();

    FT_HANDLE ftHandle;
    FT_STATUS ftStatus;
    DWORD EventDWord;
    DWORD TxBytes;
    DWORD RxBytes;
    char RxBuffer[65535];
    DWORD BytesReceived;

    void stopProcessing();

signals:
    void finished();
    void error(QString err);

public slots:
    void process();
    DWORD sendData(char* txBuffer, unsigned long txBufferSize);
};

#endif // TXFTDI_H
