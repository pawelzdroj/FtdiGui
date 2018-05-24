#include "txftdi.h"

TxFTDI::TxFTDI() { // Constructor
    // you could copy data from constructor arguments to internal variables here.
}

TxFTDI::~TxFTDI() { // Destructor
    // free resources
}

void TxFTDI::process() { // Process. Start processing data.
    // allocate resources using new here
    qDebug() << "TxFTDI hello!" << QThread::currentThreadId();
}

void TxFTDI::stopProcessing() {
    emit finished();
}


DWORD TxFTDI::sendData(char* txBuffer, unsigned long txBufferSize) {

    DWORD numOfSentBytes = 0;

    ftStatus = FT_Write(ftHandle, txBuffer, txBufferSize, &numOfSentBytes);
    if (ftStatus != FT_OK) {
        qDebug() << "FT_Write failed";
    } else {
        qDebug() << "FT_Write success:" << numOfSentBytes;
    }

    return numOfSentBytes;
}

