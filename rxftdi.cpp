#include "rxftdi.h"

RxFTDI::RxFTDI() { // Constructor
    // you could copy data from constructor arguments to internal variables here.
}

RxFTDI::~RxFTDI() { // Destructor
    // free resources
}

void RxFTDI::process() {

    qDebug() << "RxFTDI hello!" << QThread::currentThreadId();

    while(true)
    {
        FT_GetStatus(ftHandle,&RxBytes,&TxBytes,&EventDWord);
//        FT_GetQueueStatus(ftHandle, &RxBytes);
        if ((ftStatus == FT_OK) && (RxBytes > 0))
        {
//            qDebug() << "rx " << RxBytes;
            FT_Read(ftHandle, &RxBuffer, RxBytes, &BytesReceived);

            if (ftStatus == FT_OK) {
                // FT_Read OK
            }
            else {
                // FT_Read Failed
            }

        }

    }
}

void RxFTDI::stopProcessing() {
    emit finished();
}
