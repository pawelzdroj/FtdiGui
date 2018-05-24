#include "ftdi.h"

Ftdi::Ftdi(QWidget *parent) : QWidget(parent)
{


}


Ftdi::~Ftdi()
{
    FT_Close(ftHandle);
}


bool Ftdi::ftdiOpen()
{
    ftStatus = FT_OpenEx(serialNumber, FT_OPEN_BY_SERIAL_NUMBER, &ftHandle);
    ftStatus |= FT_ResetDevice(ftHandle); 	//Reset USB device

    // set interface into FT245 Synchronous FIFO mode
    Mode = 0x00; //reset mode
    ftStatus |= FT_SetBitMode(ftHandle, Mask, Mode);
    Sleep(1000);
    Mode = 0x40; //Sync FIFO mode
    ftStatus |= FT_SetBitMode(ftHandle, Mask, Mode);

    ftStatus |= FT_SetLatencyTimer(ftHandle, 2);
    ftStatus |= FT_SetUSBParameters(ftHandle,0x10000, 0x10000);
    ftStatus |= FT_SetTimeouts(ftHandle, 5000, 5000);
    ftStatus |= FT_SetFlowControl(ftHandle, FT_FLOW_RTS_CTS, 0x0, 0x0);
    ftStatus |= FT_Purge(ftHandle, FT_PURGE_RX + FT_PURGE_TX);

    Tx->ftHandle = ftHandle;
    Rx->ftHandle = ftHandle;

    qDebug() << serialNumber;

    if (ftStatus != FT_OK) {
        qDebug() << "FTDI open and configuration failed";
        return false;
    } else {
        qDebug() << "FTDI open and configuration success";
        if(ftHandle != 0) this->ftOpened = true;
        return true;
    }
}

bool Ftdi::createRxTxThreads ()
{
    RxThread = new QThread;
    Rx = new RxFTDI();
    Rx->moveToThread(RxThread);
    connect(RxThread, &QThread::started, Rx, &RxFTDI::process);
    connect(Rx, &RxFTDI::finished, RxThread, &QThread::quit);
    connect(Rx, &RxFTDI::finished, Rx, &RxFTDI::deleteLater);
    connect(RxThread, &QThread::finished, RxThread, &QThread::deleteLater);
    RxThread->start();


    TxThread = new QThread;
    Tx = new TxFTDI();
    Tx->moveToThread(TxThread);
    connect(TxThread, &QThread::started, Tx, &TxFTDI::process);
    connect(Tx, &TxFTDI::finished, TxThread, &QThread::quit);
    connect(Tx, &TxFTDI::finished, Tx, &TxFTDI::deleteLater);
    connect(TxThread, &QThread::finished, TxThread, &QThread::deleteLater);
    TxThread->start();

    return true;
}

bool Ftdi::quitRxTxThreads ()
{
    emit Tx->finished();
    emit Rx->finished();

    delete Tx;
    delete Rx;

    RxThread->terminate();
    TxThread->terminate();

    delete TxThread;
    delete RxThread;

    return true;
}

