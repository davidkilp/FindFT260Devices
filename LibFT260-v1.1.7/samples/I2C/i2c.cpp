/**
* @file i2c.cpp
*
* @author FTDI
* @date 2015-07-01
*
* Copyright c 2011 Future Technology Devices International Limited
* Company Confidential
*
* The sample source code is provided as an example and is neither guaranteed
* nor supported by FTDI.
*
* Rivision History:
* 1.0 - initial version
*/

//------------------------------------------------------------------------------
#include <stdio.h>
#include <conio.h>
#include <vector>
#include <iostream>

//------------------------------------------------------------------------------
// include FTDI libraries
//
#include "LibFT260.h"

#define I2C_SLAVE_ADDR 0x22
#define VID 0x0403
#define PID 0x6030
UCHAR gI2CAddr = I2C_SLAVE_ADDR;

inline std::vector<unsigned char> HexTextToData(std::string hexText)
{
    std::vector<unsigned char> data;
    
    for (size_t i = 0; i < hexText.size(); i += 2)
    {
        unsigned char byte[3] = {hexText[i], hexText[i + 1], 0};
        unsigned char val = static_cast<unsigned char>( strtoul((char*)byte, NULL, 16) );
        data.push_back(val);
    }

    return data;
}

//------------------------------------------------------------------------------
// main
//------------------------------------------------------------------------------
int main(void)
{
    FT260_HANDLE mhandle1 = INVALID_HANDLE_VALUE;
    FT260_STATUS ftStatus = FT260_OK;
    DWORD devNum = 0;
    FT260_CreateDeviceList(&devNum);

    ftStatus = FT260_OpenByVidPid(VID ,PID, 0, &mhandle1);
    printf("FT260_OpenByVidPid = %d\n",ftStatus );

    ftStatus = FT260_I2CMaster_Init(mhandle1, 100);

    while (1)
    {
        printf("I2C app (0)Set Addr (1)Read (2)Write (3)Reset (4)Get status (5)Read EEPROM (6)Write EEPROM  (7)Quit>>>> \n");
        char input = getch();

        if (input == '0')
        {	
          char buffer[5];        	
          
          printf("please input new i2c address, original is %x\n",gI2CAddr);
          
        	fgets (buffer, 256, stdin);
          UCHAR i2cAddr = static_cast<UCHAR>( strtoul(buffer, NULL, 16) );
          
          if(i2cAddr>=80 && i2cAddr<=0)
          {
          	printf("invalid i2c addr\n");	
          	continue;
          }	
          gI2CAddr = i2cAddr;
        	printf("set i2cAddr = %x\n",gI2CAddr);
					
			  }
        else if (input == '1')
        {
            DWORD readLength = 0;
            unsigned long len;
            char buffer[5];
            unsigned char *readData;

            printf("How many bytes do you want to read?\n");
            printf ("Enter an unsigned number: ");
            fgets (buffer, 256, stdin);
            len = strtoul (buffer, NULL, 0);
            if (len <= 0)
            {
                printf ("It is not a vaild number\n");
                continue;
            }
            readData = (unsigned char *)malloc(len);

            ftStatus = FT260_I2CMaster_Read(mhandle1, gI2CAddr, FT260_I2C_START_AND_STOP, readData, len , &readLength, 5000);
            printf("FT260_I2C_Read  ftStatus: %d  Read Length: %d\n\n", ftStatus, readLength);    // ftStatus = 0 is FT260_OK
            for(DWORD i=0; i< len; i++)
                printf("%02x", readData[i]);
            printf("\n");
            free(readData);
        }
        else if (input == '2')
        {
            DWORD writeLength = 0;
            unsigned char *writeData;
            char buffer[256];
                        
            printf("Write the content and press enter\n");
            fgets (buffer, 256, stdin);
            std::string numStr = buffer;
            numStr.pop_back();

            if (numStr.size() <= 1)
            {
                printf ("It is not a vaild content\n");
                continue;
            }
           
            std::vector<unsigned char> data = HexTextToData(numStr);

            ftStatus = FT260_I2CMaster_Write(mhandle1, gI2CAddr, FT260_I2C_START_AND_STOP, &data[0], data.size(), &writeLength);
            printf("FT260_I2C_Write  ftStatus: %d  Write Length: %d\n\n", ftStatus, writeLength);
        }
        else if (input == '3')
        {
            ftStatus = FT260_I2CMaster_Reset(mhandle1);
            printf("FT260_I2C_Reset  ftStatus: %d\n\n", ftStatus);
        }
        else if (input == '4')
        {
            /* I2C Master Controller Status (I2Cstauts variable)
             *   bit 0 = controller busy: all other status bits invalid
             *   bit 1 = error condition
             *   bit 2 = slave address was not acknowledged during last operation
             *   bit 3 = data not acknowledged during last operation
             *   bit 4 = arbitration lost during last operation
             *   bit 5 = controller idle
             *   bit 6 = bus busy
             */
            uint8 I2Cstauts;
            ftStatus = FT260_I2CMaster_GetStatus(mhandle1, &I2Cstauts);
            printf("FT260_I2C_GetStatus  ftStatus: %d, I2Cstauts: %d\n\n", ftStatus, I2Cstauts);
        }
        else if (input == '5')
        {
            char buffer[256];
            unsigned long len;
                    	
            printf("Read content from EEPROM\n");
						printf("input the read addr (hex format), do not input 0x\n");
            fgets (buffer, 256, stdin);
            unsigned char addr = static_cast<unsigned char>( strtoul(buffer, NULL, 16) );
						printf("input the read size (dec format)\n");
            fgets (buffer, 256, stdin);
            len = strtoul (buffer, NULL, 0);
            if (len <= 0)
            {
                printf ("It is not a vaild number\n");
                continue;
            } 
            
            char data;
            DWORD writeLength = 0;
            DWORD readLength = 0;
            unsigned char *readData;
            data = addr;
            readData = (unsigned char *)malloc(len);
            
            FT260_I2CMaster_Write(mhandle1, gI2CAddr, FT260_I2C_START, &data, 1, &writeLength);
            FT260_I2CMaster_Read(mhandle1, gI2CAddr, (FT260_I2C_FLAG)(FT260_I2C_REPEATED_START | FT260_I2C_STOP), readData, len , &readLength, 5000);
						
						for(int idx=0; idx<len; idx++)
						{
							printf("%02x ",readData[idx]);
							if(idx%16==15)
								printf("\n");	
						}
						printf("\n");	
        }
        else if (input == '6')
        {
            char buffer[256];
            unsigned long len;
                    	
            printf("Write content into EEPROM\n");
						printf("input the write addr (hex format), do not input 0x, it must align to multiple*8 (page wirte)\n");
            fgets (buffer, 256, stdin);
            unsigned char addr = static_cast<unsigned char>( strtoul(buffer, NULL, 16) );
						printf("Write the content and press enter, max size is 8 bytes \n");
            fgets (buffer, 256, stdin);

            std::string numStr = buffer;
            numStr.pop_back();

            if (numStr.size() <= 1)
            {
                printf ("It is not a vaild content\n");
                continue;
            }
           
            std::vector<unsigned char> data = HexTextToData(numStr);

            DWORD writeLength = 0;
            
            FT260_I2CMaster_Write(mhandle1, gI2CAddr, FT260_I2C_START, &addr, 1, &writeLength);
            FT260_I2CMaster_Write(mhandle1, gI2CAddr, FT260_I2C_STOP, &data[0], data.size(), &writeLength);
				}
        else if (input == '7')
        {
            printf("Quit\n\n");
            break;
        }
    }

    system("PAUSE");
    return 0;
}
