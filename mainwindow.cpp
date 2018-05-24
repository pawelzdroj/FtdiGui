#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <initguid.h>
#include <ntddstor.h>
#include <winbase.h>
#include <windows.h>
#include <dbt.h>
#include <Usbiodef.h>
#include "guid.h"


QString ACCEPTED_SERIALNUMBERS[] = {"FT1A2ZE5", "FT1773SJ"};



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{



    // Register for device connect notification
    DEV_BROADCAST_DEVICEINTERFACE devInt;
    ZeroMemory( &devInt, sizeof(devInt) );
    devInt.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    devInt.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    devInt.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE_FTDI;
    devInt.dbcc_name[0] = '\0';

    HWND MW_HWND = (HWND) this->winId();
    HDEVNOTIFY NotificationHandle = RegisterDeviceNotification( MW_HWND, &devInt, DEVICE_NOTIFY_WINDOW_HANDLE);

    if(NotificationHandle == NULL)
    {
        qDebug() << "Failed to register device notification";
    }



    ui->setupUi(this);

     qDebug() << "MAIN" << QThread::currentThreadId();

     ftdiChA = new Ftdi;
     ftdiChB = new Ftdi;
     ftdiChA->ftOpened = false;
     ftdiChB->ftOpened = false;
     ftdiChA->ftHandle = 0;
     ftdiChB->ftHandle = 0;

//    QObject::connect(ui->sendButton, &QPushButton::released, Tx, &TxFTDI::sendData);



}

MainWindow::~MainWindow()
{
    delete ui;
}


QString MainWindow::errorString(QString) const
{
    return errString;
}

bool MainWindow::nativeEvent(const QByteArray& eventType, void* message,
                             long* result)
{
    Q_UNUSED( result );
    Q_UNUSED( eventType );

    MSG* msg = reinterpret_cast<MSG*>(message);

    if(msg->message == WM_DEVICECHANGE)
    {
        switch(msg->wParam)
        {
        case DBT_DEVICEARRIVAL:
        {
            DEV_BROADCAST_HDR* devBcHdr = reinterpret_cast<DEV_BROADCAST_HDR*>(msg->lParam);
            if(devBcHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
            {
                DEV_BROADCAST_DEVICEINTERFACE* devBcDevInt = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE*>(msg->lParam);
                if(matchVidPid(QString::fromWCharArray(devBcDevInt->dbcc_name)))
                {
                    detectDevices();
                    emit ftdiArrived();
                }
            }


//            emit signal_FtdiConnected();
            break;
        }

        case DBT_DEVICEREMOVECOMPLETE:
        {
            DEV_BROADCAST_HDR* devBcHdr = reinterpret_cast<DEV_BROADCAST_HDR*>(msg->lParam);
            if(devBcHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
            {
                DEV_BROADCAST_DEVICEINTERFACE* devBcDevInt = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE*>(msg->lParam);
                if(matchVidPid(QString::fromWCharArray(devBcDevInt->dbcc_name)))
                {
                    qDebug() << "ftdiChA fthandle: " << ftdiChA->ftHandle;
                    qDebug() << "ftdiChA ftopened: " << ftdiChA->ftOpened;
                    qDebug() << "ftdiChB fthandle: " << ftdiChB->ftHandle;
                    qDebug() << "ftdiChB ftopened: " << ftdiChB->ftOpened;

                    FT_STATUS ftStatus;

                    if (ftdiChA->ftHandle != 0) {
                        emit ftdiDisconnected();

                        ftStatus = FT_Close(ftdiChA->ftHandle);
                        if (ftStatus == FT_OK) {
                            ftdiChA->ftOpened = false;
                            ftdiChA->ftHandle = 0;
                        }

                    }
                    else {
                        qDebug() << "A fthandle = 0";
                    }

                    if (ftdiChB->ftHandle != 0) {
                        ftStatus = FT_Close(ftdiChB->ftHandle);
                        if (ftStatus == FT_OK) {
                            ftdiChB->ftOpened = false;
                            ftdiChB->ftHandle = 0;
                        }

                    }
                    else {
                        qDebug() << "B fthandle = 0";
                    }
                }
            }
            break;
        }
        }
    }

    return false;
}


bool MainWindow::matchVidPid(QString dbcc_name)
{
    QString devicePid;
    QString deviceVid;

    int indx = dbcc_name.indexOf("#");
    int indx2 = dbcc_name.indexOf("&");
    deviceVid = dbcc_name.mid(indx  + 1, PID_VID_LENGTH);
    devicePid = dbcc_name.mid(indx2 + 1, PID_VID_LENGTH);


    if (deviceVid == VID_STRING && devicePid == PID_STRING) {
        return true;
    }

    return false;
}


bool MainWindow::detectDevices()
{
    FT_STATUS ftStatus;
    FT_DEVICE_LIST_INFO_NODE *devInfo;
    DWORD numDevs;

    ftStatus = FT_CreateDeviceInfoList(&numDevs);
    if (ftStatus == FT_OK) {
//        qDebug() << "Number of devices is" << numDevs;
        if(numDevs < 2) return false;
    }
    else {
       qDebug() << "FT_CreateDeviceInfoList FAILED!";
       return false;
    }


    devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);

    ftStatus = FT_GetDeviceInfoList(devInfo,&numDevs);
    if (ftStatus == FT_OK) {
        for (uint i = 0; i < numDevs; i++) {
            switch(supportedDevice(devInfo[i].SerialNumber))
            {
            case 'A':
            {
//               ftdiChA = new Ftdi;
               ftdiChA->ftFlags = devInfo[i].Flags;
               ftdiChA->ftType = devInfo[i].Type;
               ftdiChA->deviceID = devInfo[i].ID;
               ftdiChA->ftLocId = devInfo[i].LocId;
               strcpy(ftdiChA->serialNumber, devInfo[i].SerialNumber);
               strcpy(ftdiChA->description, devInfo[i].Description);
               ftdiChA->ftHandle = devInfo[i].ftHandle;

               ftdiChA->createRxTxThreads();

               if(ftdiChA->ftHandle == 0) ftdiChA->ftOpened = false;

               QObject::connect(this, SIGNAL (ftdiArrived()), ftdiChA, SLOT (ftdiOpen()));
               QObject::connect(this, SIGNAL (ftdiDisconnected()), ftdiChA, SLOT (quitRxTxThreads()));

//               QObject::connect(ui->sendButton, &QPushButton::released, ftdiChA->Tx, &TxFTDI::sendData(ui->textEdit));
               QObject::connect(ui->sendButton, &QPushButton::released, this, &MainWindow::handleSendButton);
               QObject::connect(this, &MainWindow::sendData, ftdiChA->Tx, &TxFTDI::sendData);


               break;
            }
            case 'B':
            {
//               ftdiChB = new Ftdi;
               ftdiChB->ftFlags = devInfo[i].Flags;
               ftdiChB->ftType = devInfo[i].Type;
               ftdiChB->deviceID = devInfo[i].ID;
               ftdiChB->ftLocId = devInfo[i].LocId;
               strcpy(ftdiChB->serialNumber, devInfo[i].SerialNumber);
               strcpy(ftdiChB->description, devInfo[i].Description);
               ftdiChB->ftHandle = devInfo[i].ftHandle;

               if(ftdiChB->ftHandle == 0) ftdiChB->ftOpened = false;

//               QObject::connect(this, SIGNAL (ftdiArrived()), ftdiChB, SLOT (ftdiOpen()));

               break;
            }
            }
       }
    }

    return true;
}


char MainWindow::supportedDevice(char serialNumber[16])
{
    for(int i = 0; i < NUM_OF_SUPPORTED_DEVICES; i++)
    {
        if(serialNumber == ACCEPTED_SERIALNUMBERS[i]+'A')
        {
            qDebug() << "A:" << serialNumber;
            return 'A';
        }
        else if(serialNumber == ACCEPTED_SERIALNUMBERS[i]+'B')
        {
            qDebug() << "B:" << serialNumber;
            return 'B';
        }
        return '0';
    }
    return '0';
}

void MainWindow::handleSendButton()
{
//    bool bStatus = false;
//    dataToSend[0] = ui->textDataToSend->toPlainText().toUInt(&bStatus,16);

//    qDebug() << dataToSend[0];

//    emit sendData(dataToSend,1);


    QString textEditString = ui->textDataToSend->toPlainText();

    QByteArray bArray = textEditString.toLatin1();
    //First get the byte array from the string
    bArray = QByteArray::fromHex(bArray);

    //Get the size of the string in bytes
    int length = bArray.size();

    //Get the string buffer
    char *tBuffer = bArray.data();

    qDebug() << length;
    qDebug() << tBuffer;

    emit sendData(tBuffer, length);



}
