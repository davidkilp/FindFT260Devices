// FindFT260Devices.cpp : Find any FT260 USB-to-I2C Bridge devices that have 
//  a PCA9632 LED controllers attached at I2C address 0x60 (Internal), 
// 0x63 (Internal) or 0x62 (External).
//  we have to probe the I2C bus to see if the the RBG strip is internal or external.
//

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>

//------------------------------------------------------------------------------
// include FTDI libraries
//
#include "LibFT260.h"
#include <iostream>

WORD FT260_Vid = 0x0403;
WORD FT260_Pid = 0x6030;

#define I2C_100KHZ  100
#define I2C_400KHZ  400

//#define DEBUG	1
#ifdef DEBUG
#define D(x)   x
#else
#define D(x)
#endif

enum FT260_LOCATION {
    INTERNAL,
    EXTERNAL
};

std::string sFT260Status[] =
{
    "FT260_OK",
    "FT260_INVALID_HANDLE",
    "FT260_DEVICE_NOT_FOUND",
    "FT260_DEVICE_NOT_OPENED",
    "FT260_DEVICE_OPEN_FAIL",
    "FT260_DEVICE_CLOSE_FAIL",
    "FT260_INCORRECT_INTERFACE",
    "FT260_INCORRECT_CHIP_MODE",
    "FT260_DEVICE_MANAGER_ERROR",
    "FT260_IO_ERROR",
    "FT260_INVALID_PARAMETER",
    "FT260_NULL_BUFFER_POINTER",
    "FT260_BUFFER_SIZE_ERROR",
    "FT260_UART_SET_FAIL",
    "FT260_RX_NO_DATA",
    "FT260_GPIO_WRONG_DIRECTION",
    "FT260_INVALID_DEVICE",
    "FT260_INVALID_OPEN_DRAIN_SET",
    "FT260_INVALID_OPEN_DRAIN_RESET",
    "FT260_I2C_READ_FAIL",
    "FT260_OTHER_ERROR"
};



bool IsFT260Dev(WCHAR* devPath)
{
    WCHAR findStr[100];
    swprintf_s(findStr, _countof(findStr), L"vid_%04x&pid_%04x", FT260_Vid, FT260_Pid);

    if (NULL == wcsstr(devPath, findStr))
    {
        return false;
    }
    else
    {
        return true;
    }
}

const char* FT260StatusToString(FT260_STATUS i)
{
    switch (i)
    {
    case  0:
        return sFT260Status[0].c_str();
    case  1:
        return sFT260Status[1].c_str();
    case  2:
        return sFT260Status[2].c_str();
    case  3:
        return sFT260Status[3].c_str();
    case  4:
        return sFT260Status[4].c_str();
    case  5:
        return sFT260Status[5].c_str();
    case  6:
        return sFT260Status[6].c_str();
    case  7:
        return sFT260Status[7].c_str();
    case  8:
        return sFT260Status[8].c_str();
    case  9:
        return sFT260Status[9].c_str();
    case 10:
        return sFT260Status[10].c_str();
    case 11:
        return sFT260Status[11].c_str();
    case 12:
        return sFT260Status[12].c_str();
    case 13:
        return sFT260Status[13].c_str();
    case 14:
        return sFT260Status[14].c_str();
    case 15:
        return sFT260Status[15].c_str();
    case 16:
        return sFT260Status[16].c_str();
    case 17:
        return sFT260Status[17].c_str();
    case 18:
        return sFT260Status[18].c_str();
    case 19:
        return sFT260Status[19].c_str();
    case 20:
        return sFT260Status[20].c_str();
    default:
        return "Not a valid FT260 status";
    }
}

bool isControllerBusy(FT260_HANDLE handle)
{
    FT260_STATUS ftStatus = FT260_OK;
    uint8 I2Cstatus = 0;

    ftStatus = FT260_I2CMaster_GetStatus(handle, &I2Cstatus);
    if (FT260_OK != ftStatus)
    {
        printf("FT260_I2CMaster_GetStatus failed, status: %s\n", FT260StatusToString(ftStatus));
        return true;
    }

    D( printf("isControllerBusy: 0x%02X\n", I2Cstatus); )
    return I2CM_CONTROLLER_BUSY(I2Cstatus);
}

/*
 * waitControllerBusy - check the status of the controller 
 * and if it is busy doing something, check every 100 msec for at most
 * 1 second before giving up.
 */
bool waitControllerBusy(FT260_HANDLE handle)
{
    FT260_STATUS ftStatus = FT260_OK;
    uint8 I2Cstatus = 0;
    bool busy = false;

    for (int i = 0; i < 10; i++)
    {
        ftStatus = FT260_I2CMaster_GetStatus(handle, &I2Cstatus);
        if (FT260_OK != ftStatus)
        {
            printf("FT260_I2CMaster_GetStatus failed, status: %s\n", FT260StatusToString(ftStatus));
            return true;
        }

        D( printf("waitControllerBusy: 0x%02X\n", I2Cstatus); )
        busy = I2CM_CONTROLLER_BUSY(I2Cstatus);
        if (!busy)
            break;
    }

    return busy;
}

/*
** checkI2CStatus()
*   check the I2C Master status for last transaction for
*   any errors:
*   returns: true if error found false otherwise
*/
bool checkI2CStatus(FT260_HANDLE handle)
{
    FT260_STATUS ftStatus = FT260_OK;
    uint8 I2Cstatus = 0;

    ftStatus = FT260_I2CMaster_GetStatus(handle, &I2Cstatus);
    if (FT260_OK != ftStatus)
    {
        printf("FT260_I2CMaster_GetStatus failed, status: %s\n", FT260StatusToString(ftStatus));
        return true;
    }

    D( printf("checkI2CStatus(): 0x%02X\n", I2Cstatus); )
    return (I2Cstatus & 0x5E);
}

FT260_STATUS readSingleByte(FT260_HANDLE handle, uint8_t i2cAddress, uint8_t registerAddress, uint8_t& value)
{
    FT260_STATUS ftStatus = FT260_OK;
    DWORD readLength = 0;
    DWORD writeLength = 0;
    DWORD numBytesToRead = 0;
    DWORD numBytesToWrite = 0;
    uint8_t buffer[3] = {};

    D( printf("Read 1 byte from 0x%02X at address: 0x%02X with handle: 0x%p\n", i2cAddress, registerAddress, handle); )

    // lets make sure controller is ready
    if (waitControllerBusy(handle))
    {
        printf("Controller is busy! we should reset it and try again?\n");
        return FT260_OTHER_ERROR;
    }


    // first write register to read
    numBytesToWrite = 1;
    buffer[0] = registerAddress;
    ftStatus = FT260_I2CMaster_Write(handle, i2cAddress, FT260_I2C_START_AND_STOP, buffer, numBytesToWrite, &writeLength);
    if ((ftStatus != FT260_OK) || (writeLength != numBytesToWrite))
    {
        printf("FT260_I2CMaster_Write setting register fails: %d\n", ftStatus);
        return ftStatus;
    }

#if 0
    // check I2C status to see if valid return
    // anything other than if I2C bus is idle is bad...
    uint8 I2Cstatus;
    ftStatus = FT260_I2CMaster_GetStatus(handle, &I2Cstatus);
    printf("I2C Read Master status: 0x%02X\n", I2Cstatus);
    //std::cout << std::format("I2C Status = 0x{:02X}\n ", I2Cstatus);
    if (!I2CM_IDLE(I2Cstatus))
    {
        printf("I2C Master status is NOT idle\n");
    }
#endif
    // lets make sure controller is ready
    if (waitControllerBusy(handle))
    {
        printf("Controller is busy! we should reset it and try again?\n");
        return FT260_OTHER_ERROR;
    }

    if (checkI2CStatus(handle))
    {
        D( printf("1st write failed. Probably not a valid device\n"); )
        return FT260_I2C_READ_FAIL;
    }


    // Now lets read the byte specified
    numBytesToRead = 1;
    value = 0;
    ftStatus = FT260_I2CMaster_Read(handle, i2cAddress, FT260_I2C_START_AND_STOP, buffer, numBytesToRead, &readLength, 1000);
    if ((ftStatus != FT260_OK) || (readLength != numBytesToRead))
    {
        printf("FT260_I2CMaster_Read() fails: %d\n", ftStatus);
        return ftStatus;
    }
    value = buffer[0];
    D( printf("FT260_I2C_Read  ftStatus : % d  Read Length : %d, value: 0x%02X\n", ftStatus, readLength, value); )

    return ftStatus;
}

/*
 * check if there is a PCA9632 device at given address
 * by first configuring as I2C Master and trying to read
*/
int CheckMasterI2c(WCHAR *pathBuf, uint8_t i2c_address)
{
    FT260_STATUS ftStatus = FT260_OTHER_ERROR;
    FT260_HANDLE handle = INVALID_HANDLE_VALUE;

    ftStatus = FT260_OpenByDevicePath(pathBuf, &handle);
    if (FT260_OK != ftStatus)
    {
        printf("Open failed, status: %s\n", FT260StatusToString(ftStatus));
        return ftStatus;
    }
    else
    {
        wprintf(L"Opened path: %s OK\n", pathBuf);
        // check if PCA9632 devices are present on specific I2C addresses

        // Configure as I2C master
        ftStatus = FT260_I2CMaster_Init(handle, I2C_100KHZ);
        if (ftStatus != FT260_OK) {
            printf("FT260_I2CMaster_Init() fails with status: %s\n", FT260StatusToString(ftStatus));
            return ftStatus;
        }

        // try to read register 05h (PWM3) which is unused in our design
        uint8_t value;
        uint8_t registerAddress = 0x05;
        ftStatus = readSingleByte(handle, i2c_address, registerAddress, value);
        if (ftStatus != FT260_OK) {
            printf("readSingleByte fails with status: (%d)%s\n", ftStatus, FT260StatusToString(ftStatus));
        } 
        else
        {
           D( printf("From device 0x%02X, register: 0x%02X = 0x%02X\n", i2c_address, registerAddress, value); )
        }

        FT260_Close(handle);
    }

    return ftStatus;
}

int FindFT260Devices(std::map<FT260_LOCATION, std::wstring>& ft260_path)
{
    FT260_STATUS ftStatus = FT260_OTHER_ERROR;
    FT260_HANDLE handle = INVALID_HANDLE_VALUE;
    DWORD devNum = 0;
    WCHAR pathBuf[128];

    FT260_CreateDeviceList(&devNum);
    printf("Number of HID devices found: %d\n\n", devNum);

    for (DWORD i = 0; i < devNum; i++)
    {
        // Get device path and open device by device path
        printf("Checking device #%d\n", i);
        ftStatus = FT260_GetDevicePath(pathBuf, 128, i);
        if (FT260_OK != ftStatus)
        {
            printf("Get Device Path failed, status: %s\n", FT260StatusToString(ftStatus));
        }
        else
        {
            wprintf(L"Device path:%s \n", pathBuf);
        }


        // check if device path contains our VID/PID
        if (IsFT260Dev(pathBuf))
        {
            uint8_t i2cAddress = 0x60;
            wprintf(L"Found an FT260 Controller at path: %s\n", pathBuf);

            // Check for Internal RBG strip
            if (CheckMasterI2c(pathBuf, i2cAddress))
            {
                printf("NO FT260 Controller with LED Strip at: 0x%02X\n", i2cAddress);
                
                // Also check for External RGB strip
                i2cAddress = 0x62;
                if (CheckMasterI2c(pathBuf, i2cAddress))
                {
                    printf("NO FT260 Controller with LED Strip at: 0x%02X\n", i2cAddress);
                }
                else
                {
                    printf("We found an FT260 Controller for External LED Strip at: 0x%02X\n", i2cAddress);
                    ft260_path[EXTERNAL] = pathBuf;
                }

            }
            else
            {
                printf("We found an FT260 Controller for Internal LED Strip at: 0x%02X\n", i2cAddress);
                ft260_path[INTERNAL] = pathBuf;
            }
        }

        printf("\n");
    }

    return 0;
}

int main()
{
    int ret = 0;
    std::map<FT260_LOCATION, std::wstring> ft260_path;

    printf("Finding FT260 USB-to-I2C bridge devices\n");

    // Find ALL HID devices (included non-FT260 devices)
    ret = FindFT260Devices(ft260_path);

    printf("\n---------------------------------------\n");
    if (ft260_path.count(INTERNAL) > 0)
        std::wcout << L"Found Internal FT260 controller at: " << ft260_path[INTERNAL] << std::endl;
    else
        std::wcout << L"No Internal FT260 controller found" << std::endl;

    if (ft260_path.count(EXTERNAL) > 0)
        std::wcout << L"Found External FT260 controller at: " << ft260_path[EXTERNAL] << std::endl;
    else
        std::wcout << L"No External FT260 controller found" << std::endl;

    return ret;
}

