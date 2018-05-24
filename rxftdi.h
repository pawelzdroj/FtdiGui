#ifndef RXFTDI_H
#define RXFTDI_H

#include <QObject>
#include <QDebug>
#include <QThread>

#include "ftd2xx.h"


class RxFTDI : public QObject
{
    Q_OBJECT

public:
    RxFTDI();
    ~RxFTDI();

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

};

#endif // RXFTDI_H
