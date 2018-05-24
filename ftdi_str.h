#ifndef FTDI_STR_H
#define FTDI_STR_H

#include "ftd2xx.h"

struct ftdiInfo {
    DWORD ftFlags;
    DWORD ftType;
    DWORD deviceID;
    DWORD ftLocId;
    char serialNumber[16];
    char description[64];
    FT_HANDLE ftHandle;
};


#endif // FTDI_STR_H
