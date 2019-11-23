/**
 ******************************************************************************
  * @file            : USB_HOST
  * @version         : v1.0_Cube
  * @brief           :  This file implements the USB Host 
  ******************************************************************************
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"
#include "usbh_hid.h"
#include "fatfs.h"
#include "../app/usermemory.h"
#include "../app/gcode.h"
static FIL fsrc;

//GcodeFile_Str GcodeFile @".ccram";
GcodeFile_Str GcodeFile;
/* USB Host Core handle declaration */
USBH_HandleTypeDef hUsbHostHS;
USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state_HS = APPLICATION_IDLE;
ApplicationTypeDef Appli_state_FS = APPLICATION_IDLE;
/**
* -- Insert your variables declaration here --
*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
* user callbak declaration
*/
static void USBH_UserProcess_HS(USBH_HandleTypeDef *phost, uint8_t id);
static void USBH_UserProcess_FS(USBH_HandleTypeDef *phost, uint8_t id);
static void USBH_UserProcess_FS_Keyboard(USBH_HandleTypeDef *phost);
static void USBH_UserProcess_HS_File(USBH_HandleTypeDef *phost);
/**
* -- Insert your external function declaration here --
*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* init function */
void MX_USB_HOST_Init(void)
{
    /* Init Host Library,Add Supported Class and Start the library*/
//  USBH_Init(&hUsbHostHS, USBH_UserProcess_FS, USBH_UserProcess_FS_Keyboard, HOST_HS);
//  USBH_RegisterClass(&hUsbHostHS, USBH_HID_CLASS);
    USBH_Init(&hUsbHostHS, USBH_UserProcess_HS, USBH_UserProcess_HS_File, HOST_HS);
    USBH_RegisterClass(&hUsbHostHS, USBH_MSC_CLASS);

    USBH_Start(&hUsbHostHS);

    /* Init Host Library,Add Supported Class and Start the library*/
//  USBH_Init(&hUsbHostFS, USBH_UserProcess_HS, USBH_UserProcess_HS_File, HOST_FS);
//  USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS);
    USBH_Init(&hUsbHostFS, USBH_UserProcess_FS, USBH_UserProcess_FS_Keyboard, HOST_FS);
    USBH_RegisterClass(&hUsbHostFS, USBH_HID_CLASS);

    USBH_Start(&hUsbHostFS);
}

uint8* getGcodeSrcBuff(void)
{
    return GcodeFile.buf;
}

FATFS usbdiskfs;
/*
 * user callbak definition
*/
static void USBH_UserProcess_HS(USBH_HandleTypeDef *phost, uint8_t id)
{
    FRESULT re;
    /* USER CODE BEGIN 2 */
    switch (id)
    {
    case HOST_USER_SELECT_CONFIGURATION:
        break;

    case HOST_USER_DISCONNECTION:
        Appli_state_HS = APPLICATION_DISCONNECT;
        f_mount(0, USBH_Path, 1);
        CLRBIT(DISP_U_CONNECT_OK);
        CLRBIT(DISP_READFILE_OK);
        CLearWorkFlag();
        printf("usb disk disconnect!\r\n");
        break;

    case HOST_USER_CLASS_ACTIVE:
        if (Appli_state_HS != APPLICATION_READY && Appli_state_HS != APPLICATION_OVER)
        {
            Appli_state_HS = APPLICATION_READY;
            re = f_mount(&usbdiskfs, USBH_Path, 1);

            if (re == FR_OK)
            {
                SETBIT(DISP_U_CONNECT_OK);
                printf("usb disk mount ok!\r\n");
            }
            else
            {
                printf("mount fail!\r\n");
            }
        }
        break;

    case HOST_USER_CONNECTION:
        //Appli_state_HS = APPLICATION_START;

        printf("usb disk connect!\r\n");
        break;

    default:
        break;
    }
    /* USER CODE END 2 */
}

void USBH_UserProcess_HS_File(USBH_HandleTypeDef *phost)
{
    uint32 len = 0;
    uint8 i = 0;
    uint8 j = 0;
    FRESULT re;
    (void)phost;
    switch (Appli_state_HS)
    {
    case APPLICATION_READY:
        if (Appli_state_FS != APPLICATION_OVER)
        {
            return;
        }
        memset(GcodeFile.dir, 0, sizeof(GcodeFile.dir));
        for (i = 0; i < 4; i++)
        {
            if (USBH_Path[i] != 0)
            {
                GcodeFile.dir[i] = USBH_Path[i];
            }
            else
            {
                j = i;
                break;
            }
        }
        if (j == 0)
        {
            return;
        }
        for (i = 0; i < 96; i++)
        {
            if (GcodeFile.name[i] != 0)
            {
                GcodeFile.dir[j + i] = GcodeFile.name[i];
            }
            else
            {
                break;
            }
        }

        GcodeFile.dir[j + i + 0] = '.';
        GcodeFile.dir[j + i + 1] = 'n';
        GcodeFile.dir[j + i + 2] = 'c';

        re = f_open(&fsrc, (char *)GcodeFile.dir, (FA_READ | FA_OPEN_EXISTING));
        if (re == FR_OK)
        {
            CLearWorkFlag();
            memset(GcodeFile.buf, 0, sizeof(GcodeFile.buf));
            re = f_read(&fsrc, GcodeFile.buf, sizeof(GcodeFile.buf), &len);

            if (re == FR_OK)
            {
                SETBIT(DISP_READFILE_OK);

                printf("file is: %s\r\n", GcodeFile.buf);
                printf("read ok!\r\n");
            }
            else
            {
                printf("read fail!\r\n");
                CLRBIT(DISP_READFILE_OK);
                CLearWorkFlag();
            }
            f_close(&fsrc);
        }
        else if (re == FR_NO_FILE)
        {
            printf("file not exist!\r\n");
            CLRBIT(DISP_READFILE_OK);
            CLearWorkFlag();
        }
        else 
        {
            printf("file open error!\r\n");
            CLRBIT(DISP_READFILE_OK);
            CLearWorkFlag();
        }

        if (GETBIT(DISP_READFILE_OK))
        {
            uint8 *buf = GcodeFile.buf;
            if (GcodeFile.buf[1024 * GCODELEN - 1] != 0)
            {
                AlarmNum = 23;
            }
            else
            {
                GCodePreRead(buf);
            }
        }
        break;

    default:
        break;
    }
}

static void USBH_UserProcess_FS(USBH_HandleTypeDef *phost, uint8_t id)
{
    /* USER CODE BEGIN 21 */
    switch (id)
    {
    case HOST_USER_SELECT_CONFIGURATION:
        break;

    case HOST_USER_DISCONNECTION:
        Appli_state_FS = APPLICATION_DISCONNECT;
        CLRBIT(DISP_BARCODE_OK);
        printf("keyboard disconnect!\r\n");
        break;

    case HOST_USER_CLASS_ACTIVE:
        if (Appli_state_FS != APPLICATION_READY && Appli_state_FS != APPLICATION_OVER)
        {
            //Appli_state_FS = APPLICATION_READY;
            Appli_state_FS = APPLICATION_START;
            SETBIT(DISP_BARCODE_OK);
            printf("keyboard init ok!\r\n");
        }
        break;

    case HOST_USER_CONNECTION:
        //Appli_state_FS = APPLICATION_START;
        printf("keyboard connect!\r\n");
        break;

    default:
        break;
    }
    /* USER CODE END 21 */
}

static void USBH_UserProcess_FS_Keyboard(USBH_HandleTypeDef *phost)
{
    HID_KEYBD_Info_TypeDef *KeybdInfo ;
    static uint8 *buf = GcodeFile.name;
    uint8 keyinfo;
//    uint8 keybuff[8]={0};
//    for(keyinfo = 0;keyinfo < 8;keyinfo++)
//    {
//        keybuff[keyinfo] = pbuf[keyinfo];
//    }
//    keyinfo = 0;

    switch (Appli_state_FS)
    {
    case APPLICATION_DISCONNECT:
        break;
    case APPLICATION_START:
        memset(GcodeFile.name, 0, sizeof(GcodeFile.name));
        buf = GcodeFile.name;
        Appli_state_FS = APPLICATION_READY;
        printf("keyboard Start!\r\n");
        break;
    case APPLICATION_READY:
        //keyinfo = KEYBRD_Decode(pbuf);
        KeybdInfo = USBH_HID_GetKeybdInfo(phost);
        keyinfo = USBH_HID_GetASCIICode(KeybdInfo);
        printf("key:%c\r\n", keyinfo);
        //if (keyinfo != 0x0d && keyinfo != 0x0a && ((keyinfo < 0x21) || (keyinfo > 0x7e)))
        if (keyinfo != 0x0a && ((keyinfo < 0x21) || (keyinfo > 0x7e)))
        {
            return;
        }

        if (keyinfo == 0x0a)
        {
            Appli_state_FS = APPLICATION_OVER;
        }
        else
        {
            *buf = keyinfo;
            buf++;
            break;
        }
    case APPLICATION_OVER:
        printf("name:%s\r\n", GcodeFile.name);
        keyinfo = 0;
        memset(FileNameData->name, 0, sizeof(FileNameData->name));
        while (GcodeFile.name[keyinfo] != 0)
        {
            if (keyinfo < 64)
            {
                FileNameData->name[keyinfo] = GcodeFile.name[keyinfo];
            }
            keyinfo++;
        }

        if (keyinfo > 0)
        {
            FileNameData->readok = 1;
            USBH_UserProcess_HS_File(phost);
            memset(GcodeFile.name, 0, sizeof(GcodeFile.name));
            buf = GcodeFile.name;
        }
        //SETBIT(DISP_FILENAME_UPDATE);
        Appli_state_FS = APPLICATION_READY;
        break;
    default:
        break;
    }
}

void ManualGetFileName(void)
{
    int i;

    if (ManualFilename->Writeok == 0)
    {

    }
    else if (ManualFilename->Writeok == 2)
    {
    }
    else if (ManualFilename->Writeok == 1)
    {
        if (ManualFilename->len > 0)
        {
            for (i = 0; i < ManualFilename->len; i++)
            {
                if (ManualFilename->name[i] == 0)
                {
                    break;
                }
            }

            if (i == ManualFilename->len)
            {
                ManualFilename->Writeok = 3;
            }
        }
    }
    else if (ManualFilename->Writeok == 3)
    {
        for (i = 0; i < ManualFilename->len; i++)
        {
            if (ManualFilename->name[i] > 0)
            {
                GcodeFile.name[i] = ManualFilename->name[i];
            }
        }
        Appli_state_FS = APPLICATION_OVER;
        ManualFilename->Writeok = 4;

        i = 0;
        memset(FileNameData->name, 0, sizeof(FileNameData->name));
        while (GcodeFile.name[i] != 0)
        {
            if (i < 64)
            {
                FileNameData->name[i] = GcodeFile.name[i];
            }
            i++;
        }

        if (i > 0)
        {
            FileNameData->readok = 1;
            USBH_UserProcess_HS_File(&hUsbHostHS);
            memset(GcodeFile.name, 0, sizeof(GcodeFile.name));
        }
        //SETBIT(DISP_FILENAME_UPDATE);
        Appli_state_FS = APPLICATION_READY;

    }
}

/*
 * Background task
*/
void MX_USB_HOST_Process(void)
{
    /* USB Host Background task */
    USBH_Process(&hUsbHostHS);
    USBH_Process(&hUsbHostFS);
}
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
