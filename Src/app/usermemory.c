#include "../app/action.h"
#include "config.h"
#include "../modbus/ModbusSlave.h"
#include "../app/usermemory.h"

SystemParam_str *SystemParam;
SpeedParam_Str *SpeedParam;
FactoryParam_str *FactoryParam;
UserParam_str *UserParam;
HolePos_str *HolePos;
CurAxisDistStr *SysCurrentParam;
UserCurrentPara_Str *UserCurrentParam;
ScanCodeData_str *ScanCodeWorkData;
//CodeModeParam_str *CodeModeParam;
IOConfigParamStrct *IOConfigParam;
DingData_str *DingData;
Filename_str *FileNameData;
ManualFilename_str *ManualFilename;


//USB_UserState_str *USB_UserState;

BYTE_BITS cMidleCoil[MEMSIZE_COIL / 8] @".ccram";
unsigned short cDataArea[MEMSIZE_REG] @".ccram";

void Save16BitDate(unsigned short val, unsigned short addr)
{
    Write24C04_16bit(addr, val);
    // unsigned char byteh = 0;
    // unsigned char bytel = 0;
    // unsigned short temp = 0;

    // byteh = (data >> 8) & 0x0ff;
    // bytel = data & 0x0ff;
    // temp = addr * 2;

    // Write24C04(temp, bytel);
    // Write24C04((temp + 1), byteh);
}

void Save32BitDate(unsigned long val, unsigned short addr)
{
    Write24C04_32bit(addr, val);
    // unsigned char bytehh = 0;
    // unsigned char bytehl = 0;
    // unsigned char bytelh = 0;
    // unsigned char bytell = 0;
    // unsigned short temp = 0;

    // bytehh = (val >> 24) & 0x0ff;
    // bytehl = (val >> 16) & 0x0ff;
    // bytelh = (val >> 8) & 0x0ff;
    // bytell = val & 0x0ff;
    // temp = addr * 2;

    // Write24C04(temp, bytell);
    // Write24C04((temp + 1), bytelh);
    // Write24C04((temp + 2), bytehl);
    // Write24C04((temp + 3), bytehh);
}

/************************************************/
/*
初始化系统变量

*/
/************************************************/
// 第一次上电时的初始化
void FirstInit(void)
{
    unsigned char *buf = (unsigned char *)&cDataArea[0];
    unsigned short i;

    memset(cDataArea, 0, 2048);

    // for (i = 0; i < 2048; i++)
    // {
    //     buf[i] = Read24C04(i);
    // }
    Read24C04_nByte(buf, I2CSIZE, 0);

    for (i = 0; i < 20; i++)
    {
        ScanCodeWorkData->Orign[i] = 0;
        ScanCodeWorkData->Length[i] = 0;
        ScanCodeWorkData->Depth[i] = 0;
    }
    if (cDataArea[0] != 0xaa)
    {
        cDataArea[0] = 0xaa;

        for (i = 0; i < 50; i++)
        {
            IOConfigParam->XPin[i] = i;
            IOConfigParam->YPin[i] = i;
        }

        SystemParam->EncodeDirection = 0;
        
        SystemParam->XDir = 1;
        SystemParam->YDir = 0;
        SystemParam->DemoFunction = 0;

        SystemParam->XSevoAlarmState = 1;
        SystemParam->YSevoAlarmState = 1;
        SystemParam->InverterAlarm_L = 0;
        SystemParam->InverterAlarm_R = 1;
        
        SystemParam->SystemMode = 4;
        SystemParam->UseProbe = 1;
        SystemParam->XCalculate = 1;
        
        SystemParam->XResetSpeed = 8;
        SystemParam->YResetSpeed = 3;
        
        SystemParam->XCycleLength = 21428;
        SystemParam->XCyclePulse = 5000;
        SystemParam->XMaxLength = 260000;

        SystemParam->YCycleLength = 5000;
        SystemParam->YCyclePulse = 2500;
        SystemParam->YMaxLength_R = 4500;
        FactoryParam->YMaxLength_L = 4500;
        
        SystemParam->Work1Dist = 0;
        SystemParam->Work2Dist = 80000;
        SystemParam->Work3Dist = 160000;
        SystemParam->Work4Dist = 300000;
        //cDataArea[7] = 50;

        // 厂家参数
        //cDataArea[2] = 0;

        //cDataArea[30] = 10;
        SpeedParam->ManualSpeed = 10;
        SpeedParam->ScanSpeed2 = 20;
        SpeedParam->XIdleSpeed = 60;
        SpeedParam->YIdleSpeed = 30;
        SpeedParam->YDrillSpeed = 20;
        SpeedParam->XSlotSpeed = 3;
        SpeedParam->XMoveSpeed = 50;
        SpeedParam->CheckHoleSpeed2 = 39;
        //cDataArea[37] = 800;
        SpeedParam->YManualSpeed = 3;


        FactoryParam->ChaXiaoMode = 0;
        //cDataArea[43] = 0;
        FactoryParam->ChaXiaoIndex = 0;
        FactoryParam->ChaXiaoDepth = 10;
        FactoryParam->ChaXiaoDist = 3200;
        FactoryParam->CustomPlug1 = 0;
        FactoryParam->CustomPlug2 = 0;
        
        FactoryParam->ArrayNumber = 0;

        FactoryParam->MinHoleRaidus = 1000;
        FactoryParam->MaxHoleRaidus = 1800;

        FactoryParam->YOrignDist_R = 1000;
        FactoryParam->YOrignDist_L = 1000;
        FactoryParam->HoleDepth = 1000;

        FactoryParam->ToolOffSet_Scan = 1000;
        
        FactoryParam->ToolOffSet_Hole_L = 1000;
        FactoryParam->ToolOffSet_Hole_R = 1000;
        
        FactoryParam->ToolOffSet_TW_L = 10000;
        FactoryParam->ToolOffSet_TW_R = 10000;
        
        FactoryParam->ToolOffSet_Code_L = 10000;
        FactoryParam->ToolOffSet_Code_R = 10000;
        
        
        
        FactoryParam->Hole_Probe = 1;
        FactoryParam->Code_Probe = 1;
        FactoryParam->PreDecDistance = 10000;
        FactoryParam->SlowMotorBuchang = 0;

        FactoryParam->NoWorktime = 30;
        FactoryParam->DrillCycleTime = 15;
        FactoryParam->DrillStsrtDelay = 50;
        
        FactoryParam->MotorStartDelay_R = 80;
        FactoryParam->MotorStartDelay_L = 80;
        
        FactoryParam->MotorStopDelay_R = 100;
        FactoryParam->MotorStopDelay_L = 100;        
        
        FactoryParam->QiGangReBackDelay = 150;
        FactoryParam->XBackDelayTime = 0;
        FactoryParam->HoleFeedDepth = 1000;
        FactoryParam->HoleDrillMode = 0;

        FactoryParam->WorkMode = 0;
        FactoryParam->DotDistance = 1000;
        FactoryParam->DrillRaidus = 800;
        //FactoryParam->CaoMode = 0;
        FactoryParam->CaoPicDepth = 500;

        FactoryParam->TongCaoDepth = 300;

        Write24C04_nByte(buf, 0, I2CSIZE);
    }
}
// 版本管理
void VerControl(void)
{
    SysCurrentParam->Ver1 = 2;
    SysCurrentParam->Ver2 = 0;
    SysCurrentParam->Ver3 = 0;
}

void PlugInit(void)
{
    DWORD_VAL temp;

    cChaXiaoBackup = FactoryParam->ChaXiaoMode;

    temp.word.V16L = FactoryParam->CustomPlug1;
    temp.word.V16H = FactoryParam->CustomPlug2;

    cMidleCoil[6].Val = temp.V8[0];
    cMidleCoil[7].Val = temp.V8[1];
    cMidleCoil[8].Val = temp.V8[2];
    cMidleCoil[9].Val = temp.V8[3];

    if (temp.V32 == 0xffffffff)
    {
        bCustomPlugAll = 1;
        bCustomPlugafterHole = 0;
        bCustomPlugBeforHole = 0;
    }
    else if (temp.word.V16L == 0xffff)
    {
        bCustomPlugAll = 0;
        bCustomPlugafterHole = 0;
        bCustomPlugBeforHole = 1;
    }
    else if (temp.word.V16H == 0xffff)
    {
        bCustomPlugAll = 0;
        bCustomPlugafterHole = 1;
        bCustomPlugBeforHole = 0;
    }
}

void PointInit(void)
{
    SystemParam = (SystemParam_str *)(cDataArea + 1);
    FactoryParam = (FactoryParam_str *)(cDataArea + 45);
    SpeedParam = (SpeedParam_Str *)(cDataArea + 30);
    UserParam = (UserParam_str *)(cDataArea + 200);
    IOConfigParam = (IOConfigParamStrct *)(cDataArea + 100);

    HolePos = (HolePos_str *)(cDataArea + TestData_offset);
    SysCurrentParam = (CurAxisDistStr *)(cDataArea + CurrentParam_offset);

    ScanCodeWorkData = (ScanCodeData_str *)(cDataArea + 500);
    FileNameData = (Filename_str *)(cDataArea + 600);
    ManualFilename = (ManualFilename_str *)(cDataArea + 700);
    UserCurrentParam = (UserCurrentPara_Str *)(cDataArea + 800);
    DingData = (DingData_str *)(cDataArea + 900);
}
uint16 ServoDelay;
//运行变量初始化
void InitVariable(void)
{
    Press1time=0;
    Press2time=0;
    Press3time=0;
    Press4time=0;
    PointInit();
    FirstInit();
    VerControl();
    //WorkModeInit();
    //PlugInit();

    RebackXYFLG = 1;
    cAxisRunState.Val = 0;

//  if (SystemParam->SystemMode > 4)
//  {
//      SystemParam->SystemMode = 0;
//  }
//  if (SystemParam->SystemMode > 1)
//  {
//      SystemParam->ToolNumber = 1;
//  }
//
//  if (FactoryParam->ArrayNumber_Slot > 99)
//      FactoryParam->ArrayNumber_Slot = 99;
//  if (FactoryParam->ArrayNumber_Hole > 99)
//      FactoryParam->ArrayNumber_Hole = 99;
//  if (FactoryParam->ArrayNumber_Code > 99)
//      FactoryParam->ArrayNumber_Code = 99;
//
//  if (FactoryParam->WorkMode == WorkMode_Slot)
//  {
//      bReadParam = 1;
//      UserCurrentParam->CurrentArray = FactoryParam->ArrayNumber_Slot;
//  }
//  else if (FactoryParam->WorkMode == WorkMode_Hole)
//  {
//      bReadParam = 1;
//      UserCurrentParam->CurrentArray = FactoryParam->ArrayNumber_Hole;
//  }
//  else if (FactoryParam->WorkMode == WorkMode_Code)
//  {
//      bReadParam = 1;
//      UserCurrentParam->CurrentArray = FactoryParam->ArrayNumber_Code;
//  }

    ServoDelay = 1000;
    while (ServoDelay)
        ;
}

unsigned long getMemAddr(void)
{
    unsigned long addr = 0;

    addr = UserCurrentParam->CurrentArray;
    FactoryParam->ArrayNumber = addr;
    Write24C04_16bit(45, addr);
    addr += 100;

    return addr;
}

/************************************************/
/*
保存参数集，读取参数集

*/
/************************************************/
#define Page_of_4K 4096 // 一页  4096
static uint8 buffertmp[Page_of_4K];
static unsigned char buffer[512];
void SaveAndLoadParams(void)
{
    //unsigned char buffer[512];
    //unsigned short i = 0, j = 0, k = 0;
    unsigned short j = 0;//, k = 0;
    unsigned long addr = 0;
    static unsigned long displayreaddelay = 0;
    static unsigned long displaywritedelay = 0;

    if (!bRunning)
    {
        if (UserCurrentParam->CurrentArray > 99)
            UserCurrentParam->CurrentArray = 99;
        // if (FactoryParam->ArrayNumber_Slot > 99)
        //     FactoryParam->ArrayNumber_Slot = 99;
        // if (FactoryParam->ArrayNumber_Hole > 99)
        //     FactoryParam->ArrayNumber_Hole = 99;
        // if (FactoryParam->ArrayNumber_Code > 99)
        //     FactoryParam->ArrayNumber_Code = 99;

        if (bReadOkDisplay)
        {
            if (displayreaddelay < dwTickCount)
            {
                bReadOkDisplay = 0;
            }
        }
        if (bWriteOkDisplay)
        {
            if (displaywritedelay < dwTickCount)
            {
                bWriteOkDisplay = 0;
            }
        }

        if (bWriteParam)
        {
            addr = getMemAddr();
            addr *= 4096;
            j = 100 * 2;
            memset(buffertmp, 0, sizeof(buffertmp));
            memcpy(buffertmp, &cDataArea[900], j);

            W25QXX_Write(buffertmp, addr, j);

            bWriteParam = 0;

            bWriteOkDisplay = 1;
            displaywritedelay = dwTickCount + 3000;
        }
        else if (bReadParam)
        {
            addr = getMemAddr();
            addr *= 4096;
            j = 100 * 2;
            memset(buffertmp, 0, sizeof(buffertmp));
            //ReadPage(addr, buffertmp);
            W25QXX_Read(buffertmp, addr, j);

            memcpy(&cDataArea[900], buffertmp, j);
            memcpy(buffer, buffertmp, j);

            //Write24C04_nByte(buffer, 200, j);
            // for (i = 0; i < j; i++)
            // {
            //     Write24C04(200 + i, buffer[i]);
            // }
            bReadParam = 0;
            bReadOkDisplay = 1;
            displayreaddelay = dwTickCount + 3000;
        }
        // else if (bWriteHParam)
        // {
        //     k = 100 + FactoryParam->ArrayNumber_Hole;
        //     addr = Page_of_4K * k;
        //     j = 20 * 4 + 20 * 2;

        //     memset(buffertmp, 0, sizeof(buffertmp));
        //     memcpy(buffertmp, &cDataArea[180], j);

        //     //EraseSector(addr);
        //     //ProgramPage(addr, buffertmp);
        //     //W25QXX_Erase_Sector(addr);
        //     W25QXX_Write(buffertmp, addr, j);

        //     bWriteHParam = 0;
        // }
        // else if (bReadHParam)
        // {
        //     k = 100 + FactoryParam->ArrayNumber_Hole;
        //     addr = Page_of_4K * k;
        //     j = 20 * 4 + 20 * 2;

        //     memset(buffertmp, 0, sizeof(buffertmp));
        //     //ReadPage(addr, buffertmp);
        //     W25QXX_Read(buffertmp, addr, j);

        //     memcpy(&cDataArea[180], buffertmp, j);
        //     memcpy(buffer, buffertmp, j);

        //     Write24C04_nByte(buffer, 360, j);
        //     // for (i = 0; i < j; i++)
        //     // {
        //     //     Write24C04(360 + i, buffer[i]);
        //     // }
        //     bReadHParam = 0;
        // }
        else if (bWriteFactory)
        {
            addr = Page_of_4K * 500;
            j = I2CSIZE;

            memset(buffertmp, 0, sizeof(buffertmp));
            memcpy(buffertmp, &cDataArea[0], j);

            /*             EraseSector(addr);
            ProgramPage(addr, buffertmp); */

            W25QXX_Erase_Sector(addr);
            W25QXX_Write(buffertmp, addr, j);

            bWriteFactory = 0;
        }
        else if (bReadFactory)
        {
            addr = Page_of_4K * 500;
            j = I2CSIZE;

            memset(buffertmp, 0, sizeof(buffertmp));
            //ReadPage(addr, buffertmp);
            W25QXX_Read(buffertmp, addr, j);
            memcpy(&cDataArea[0], buffertmp, j);
            memcpy(buffer, buffertmp, j);

            // for (i = 0; i < j; i++)
            // {
            //     Write24C04(i, buffer[i]);
            // }
            Write24C04_nByte(buffer, 0, j);
            bReadFactory = 0;
        }
    }
}

/************************************************/
/*
初始化是否的速度

*/
/************************************************/
/*void ServoSpeedInit(void )
{
    X_ALARMEN=1; //驱动器故障使能
    Y_ALARMEN=1;
    MV_Set_Range(X_AXIS,100);
    MV_Set_Range(Y_AXIS,100);

    MV_Set_Acc(X_AXIS,200);
    MV_Set_Acc(Y_AXIS,100);

    MV_Set_Dec(X_AXIS,300);
    MV_Set_Dec(Y_AXIS,100);

    MV_Set_Startv(X_AXIS,10);
    MV_Set_Startv(Y_AXIS,10);

    MV_Set_Speed(X_AXIS,400);
    MV_Set_Speed(Y_AXIS,400);

    CalDecStop(X_CurSpeed,&XCurSpeedPusel);
    CalAccRun(X_CurSpeed,&XCurSpeedPusel);
    CalDecStop(Y_CurSpeed,&YCurSpeedPusel);
    CalAccRun(Y_CurSpeed,&YCurSpeedPusel);

    MV_Set_Command_Pos(X_AXIS,0);
    MV_Set_Command_Pos(Y_AXIS,0);

    //MV_Set_Startv(X_AXIS,10);
    //MV_Set_Speed(X_AXIS,300);
    //cSetPulse=4000;
    //MV_Pmov(X_AXIS, cSetPulse);

    //MV_Continue_Move(X_AXIS, 0);
    //MV_Continue_Move(Y_AXIS, 0);
    //MV_Pmov(X_AXIS, 100);

    //MV_Set_Command_SlMTN(X_AXIS, -1000);
    //MV_Set_Command_SlMTP(X_AXIS, 10000);
    //MV_Set_Command_SlMTN(Y_AXIS, -1000);
    //MV_Set_Command_SlMTP(Y_AXIS, 10000);

}

*/
