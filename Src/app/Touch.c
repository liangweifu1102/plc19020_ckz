/*
 * @Description: 
 * @Author: huacong
 * @LastEditors: ccc
 * @Date: 2019-04-20 12:40:10
 * @LastEditTime: 2019-05-06 17:17:02
 */

#include "Action.h"
#include "usermemory.h"
#include "../hardware/pwm.h"
#include "AutoRun.h"
#include "touch.h"
#include "../hardware/anglogbus.h"
#include "../usbapp/usb_host.h"

void LongBoardSet(void)
{
    if (FactoryParam->WorkMode == WorkMode_Code)
    {
        UserCurrentParam->BoardMode = 1;
    }
    else if (FactoryParam->WorkMode == WorkMode_Code)
    {
        if (FactoryParam->Hole_Probe == 0)
        {
            UserCurrentParam->BoardMode = 1;
        }
    }
}

/**
 * 检查启动
 * 
 * @author xt (2019-03-02)
 * 
 * @param void 
 */
extern unsigned short DrillMotorStopStep;

void CheckStart(void)
{
    //if ((!(bRunning || bResume || bStop || X_DRV || Y_DRV)) && (RebackXYFLG == 0))
    if (AlarmNum == 0 && DrillMotorStopStep == 0) // || AlarmNum == 10)
    {
        if ((bRunning1 || bRunning2 || bRunning3 || bRunning4))
        {
            bRunning = 1;
            cRunStep = 100;
            bMotorSpeedStopFlag = 1;
            ResetPeripheral();
            NoworkStopdelay = FactoryParam->NoWorktime + 1;
            NoworkStopdelay *= 1000;
            NoworkStopdelay += dwTickCount;
        }
    }
}
/**
 * @description: 
 * @param : undefined
 * @return: 
 */
void WriteMotorSpeed(unsigned short num, unsigned short speed)
{
    static uint16 motorspeedbackup[4] = { 0 };
    if (num > 1)
    {
        num = 1;
    }
    if (speed > 300)
    {
        speed = 300;
    }
    if (motorspeedbackup[num] != speed)
    {
        uint32 temp = 0;

        motorspeedbackup[num] = speed;
        temp = motorspeedbackup[num];
        temp *= 100;
        temp /= 300;
        temp *= 4095;
        temp /= 100;

        DAC_Conver(num, temp);
    }
}

void KeyPressIn1(void)
{
    if (bPress1_IN && !bPrePress1_IN && (Press1time < dwTickCount))
    {
        Press1time = FactoryParam->DrillStsrtDelay * 10;
        Press1time += dwTickCount;
        
        UserCurrentParam->debugdata1 = Press1time/1000;
        
        if (UserCurrentParam->BoardMode == 0)
        { // 短板
            if (!bPress1_Vavle)
            {
                bPress1_Vavle = 1;
                KeyPress1Flag = 1;
                bRunning1 = 1;
            }
            if (bPress1_Vavle && !bRunning1)
            {
                bPress1_Vavle = 0;
            }
        }
        else if (UserCurrentParam->BoardMode == 1)
        { // 长板
            if (!bPress1_Vavle)
            {
                bPress1_Vavle = 1;
                bPress2_Vavle = 1;
                bPress3_Vavle = 1;
                bPress4_Vavle = 1;
                bRunning1 = 1;
            }
            if (bPress1_Vavle && !bRunning1)
            {
                bPress1_Vavle = 0;
                bPress2_Vavle = 0;
                bPress3_Vavle = 0;
                bPress4_Vavle = 0;
            }
        }
        else if (UserCurrentParam->BoardMode == 2)
        { // 长板
            if (!bPress1_Vavle)
            {
                bPress1_Vavle = 1;
                bPress2_Vavle = 1;
                KeyPress1Flag = 1;
                bRunning1 = 1;
            }
            if (bPress1_Vavle && !bRunning1)
            {
                bPress1_Vavle = 0;
                bPress2_Vavle = 0;
            }
        }
    }
}

void KeyPressIn2(void)
{
    static unsigned char i_flg=0;
    if (i_flg)
    {
      
    }
    else
    {
       Press2time = 0;
    }
    if (bPress2_IN && !bPrePress2_IN && (Press2time < dwTickCount))
    {
        i_flg=1;
        Press2time = FactoryParam->DrillStsrtDelay * 10;
        Press2time += dwTickCount;

        UserCurrentParam->debugdata2 = Press2time/1000;

        if (UserCurrentParam->BoardMode == 0)
        {
            if (!bPress2_Vavle)
            {
                
                bPress2_Vavle = 1;
                bRunning2 = 1;
                KeyPress2Flag = 1;
            }
            if (bPress2_Vavle && !bRunning2)
            {
                bPress2_Vavle = 0;
            }
        }
        else if (UserCurrentParam->BoardMode == 1)
        {
            Press1time = Press2time;
            if (!bPress1_Vavle)
            {
                bPress1_Vavle = 1;
                bPress2_Vavle = 1;
                bPress3_Vavle = 1;
                bPress4_Vavle = 1;
                bRunning1 = 1;
            }
            if (bPress1_Vavle && !bRunning1)
            {
                bPress1_Vavle = 0;
                bPress2_Vavle = 0;
                bPress3_Vavle = 0;
                bPress4_Vavle = 0;
            }
        }
        else if (UserCurrentParam->BoardMode == 2)
        { // 长板
            if (!bPress2_Vavle)
            {
                bPress1_Vavle = 1;
                bPress2_Vavle = 1;
                KeyPress1Flag = 1;
                bRunning1 = 1;
            }
            if (bPress2_Vavle && !bRunning1)
            {
                bPress1_Vavle = 0;
                bPress2_Vavle = 0;
            }
        }
    }
}

void KeyPressIn3(void)
{
    static unsigned char i_flg=0;
    if (i_flg)
    {

    }
    else
    {
       Press3time = 0;
    }
    if (bPress3_IN && !bPrePress3_IN && (Press3time < dwTickCount))
    {
        i_flg=1;
        Press3time = FactoryParam->DrillStsrtDelay * 10;
        Press3time += dwTickCount;

        if (UserCurrentParam->BoardMode == 0)
        {
            if (!bPress3_Vavle)
            {
                bPress3_Vavle = 1;
                bRunning3 = 1;
                KeyPress3Flag = 1;
            }
            if (bPress3_Vavle && !bRunning3)
            {
                bPress3_Vavle = 0;
            }
        }
        else if (UserCurrentParam->BoardMode == 1)
        {
            Press1time = Press3time;
            if (!bPress1_Vavle)
            {
                bPress1_Vavle = 1;
                bPress2_Vavle = 1;
                bPress3_Vavle = 1;
                bPress4_Vavle = 1;
                bRunning1 = 1;
            }
            if (bPress1_Vavle && !bRunning1)
            {
                bPress1_Vavle = 0;
                bPress2_Vavle = 0;
                bPress3_Vavle = 0;
                bPress4_Vavle = 0;
            }
        }
        else if (UserCurrentParam->BoardMode == 2)
        { // 长板
            if (!bPress3_Vavle)
            {
                bPress3_Vavle = 1;
                bPress4_Vavle = 1;
                KeyPress3Flag = 1;
                bRunning3 = 1;
            }
            if (bPress3_Vavle && !bRunning3)
            {
                bPress3_Vavle = 0;
                bPress4_Vavle = 0;
            }
        }
    }
}

void KeyPressIn4(void)
{
    static unsigned char i_flg=0;
    if (i_flg)
    {

    }
    else
    {
       Press4time = 0;
    }
    if (bPress4_IN && !bPrePress4_IN && (Press4time < dwTickCount))
    {
        i_flg=1;
        Press4time = FactoryParam->DrillStsrtDelay * 10;
        Press4time += dwTickCount;

        if (UserCurrentParam->BoardMode == 0)
        {
            if (!bPress4_Vavle)
            {
                bPress4_Vavle = 1;
                bRunning4 = 1;
                KeyPress4Flag = 1;
            }
            if (bPress4_Vavle && !bRunning4)
            {
                bPress4_Vavle = 0;
            }
        }
        else if (UserCurrentParam->BoardMode == 1)
        {
            Press1time = Press4time;
            if (!bPress1_Vavle)
            {
                bPress1_Vavle = 1;
                bPress2_Vavle = 1;
                bPress3_Vavle = 1;
                bPress4_Vavle = 1;
                bRunning1 = 1;
            }
            if (bPress1_Vavle && !bRunning1)
            {
                bPress1_Vavle = 0;
                bPress2_Vavle = 0;
                bPress3_Vavle = 0;
                bPress4_Vavle = 0;
            }
        }
        else if (UserCurrentParam->BoardMode == 2)
        { // 长板
            if (!bPress4_Vavle)
            {
                bPress3_Vavle = 1;
                bPress4_Vavle = 1;
                KeyPress3Flag = 1;
                bRunning3 = 1;
            }
            if (bPress4_Vavle && !bRunning3)
            {
                bPress3_Vavle = 0;
                bPress4_Vavle = 0;
            }
        }
    }
}

/************************************************/
/*
��������

*/
/************************************************/
// �ⲿѹ������
void KeyPressIn(void)
{
    KeyPressIn1();
    KeyPressIn2();
    KeyPressIn3();
    KeyPressIn4();
}

void SetPlugSelect(void)
{
    if (bTWNei_ST)
    {
        bTWNei_ST = 0;
        FactoryParam->ChaXiaoMode = PlugTWNei;
    }
    else if (bTWWai_ST)
    {
        bTWWai_ST = 0;
        FactoryParam->ChaXiaoMode = PlugTWWai;
    }
    else if (bNOChaXiao_ST)
    {
        bNOChaXiao_ST = 0;
        FactoryParam->ChaXiaoMode = NoPlug;
    }
    else if (bCustomPlugState_ST)
    {
        bCustomPlugState_ST = 0;
        FactoryParam->ChaXiaoMode = PlugCustom;
    }
    else if (bCustomPlugAll_ST)
    {
        bCustomPlugAll_ST = 0;
        FactoryParam->ChaXiaoMode = PlugAll;
    }
    else if (bCustomPlugafterHole_ST)
    {
        bCustomPlugafterHole_ST = 0;
        FactoryParam->ChaXiaoMode = PlugafterHole;
    }
    else if (bCustomPlugBeforHole_ST)
    {
        bCustomPlugBeforHole_ST = 0;
        FactoryParam->ChaXiaoMode = PlugBeforHole;
    }
    else if (bPlugforOneHoleST)
    {
        bPlugforOneHoleST = 0;
        FactoryParam->ChaXiaoMode = PlugforOneHole;
    }
    else if (bPlugforTwoHoleST)
    {
        bPlugforTwoHoleST = 0;
        FactoryParam->ChaXiaoMode = PlugforTwoHole;
    }
}

void SetPlugDisplay(void)
{
    if (FactoryParam->ChaXiaoMode == PlugTWWai)
    {
        bTWWai = 1;
        //CustomPlug->OneHolePlug = 0;
    }
    else if (FactoryParam->ChaXiaoMode == PlugTWNei)
    {
        bTWNei = 1;
        //CustomPlug->OneHolePlug = 0;
    }
    else if (FactoryParam->ChaXiaoMode == PlugCustom)
    {
        bCustomPlugState = 1;
        //CustomPlug->OneHolePlug = 0;
    }
    else if (FactoryParam->ChaXiaoMode == PlugAll)
    {
        bCustomPlugAll = 1;
        //CustomPlug->OneHolePlug = 0;
    }
    else if (FactoryParam->ChaXiaoMode == PlugafterHole)
    {
        bCustomPlugafterHole = 1;
        //CustomPlug->OneHolePlug = 0;
    }
    else if (FactoryParam->ChaXiaoMode == PlugBeforHole)
    {
        bCustomPlugBeforHole = 1;
        //CustomPlug->OneHolePlug = 0;
    }
    else if (FactoryParam->ChaXiaoMode == NoPlug)
    {
        bNOChaXiao = 1;
        //CustomPlug->OneHolePlug = 0;
    }
    else if (FactoryParam->ChaXiaoMode == PlugforOneHole)
    {
        bPlugforOneHole = 1;
        //CustomPlug->OneHolePlug = 0;
    }
    else if (FactoryParam->ChaXiaoMode == PlugforTwoHole)
    {
        bPlugforTwoHole = 1;
        //CustomPlug->OneHolePlug = 0;
    }

    if (FactoryParam->ChaXiaoMode != NoPlug)
    {
        if (bMiddleChaXiao_ST)
        {
            bMiddleChaXiao_ST = 0;
            bMiddleChaXiao = ~bMiddleChaXiao;
        }
    }
}

void ClearPlugBit(void)
{
    bNOChaXiao = 0;
    bTWNei = 0;
    bTWWai = 0;
    bCustomPlugAll = 0;
    bCustomPlugafterHole = 0;
    bCustomPlugBeforHole = 0;
    bCustomPlugState = 0;
    bPlugforOneHole = 0;
    bPlugforTwoHole = 0;
}

// ����ѡ��
void CustomPlugSelect(void)
{
    if (FactoryParam->WorkMode != WorkMode_Scan)
    {
        if (FactoryParam->ChaXiaoMode != 0)
        {
            FactoryParam->ChaXiaoMode = 0;
        }
    }

    ClearPlugBit();
    SetPlugSelect();
    SetPlugDisplay();

    if (cChaXiaoBackup != FactoryParam->ChaXiaoMode)
    {
        cChaXiaoBackup = FactoryParam->ChaXiaoMode;
        //Write24C04_16bit(42, FactoryParam->ChaXiaoMode);
        Save16BitDate(FactoryParam->ChaXiaoMode, (55));
    }

    if (bCustomPlugState)
    {
        unsigned char i = 0;
        DWORD_VAL temp1, temp2;

        temp1.V8[0] = cMidleCoil[40].Val;
        temp1.V8[1] = cMidleCoil[41].Val;
        temp1.V8[2] = cMidleCoil[42].Val;
        temp1.V8[3] = cMidleCoil[43].Val;

        temp2.V8[0] = cMidleCoil[6].Val;
        temp2.V8[1] = cMidleCoil[7].Val;
        temp2.V8[2] = cMidleCoil[8].Val;
        temp2.V8[3] = cMidleCoil[9].Val;

        for (i = 0; i < 32; i++)
        {
            if (temp1.V32 & (1UL << i))
            {
                temp1.V32 &= ~(1UL << i);
                if (temp2.V32 & (1UL << i))
                {
                    temp2.V32 &= ~(1UL << i);
                }
                else
                {
                    temp2.V32 |= (1UL << i);
                }
            }
        }

        cMidleCoil[6].Val = temp2.V8[0];
        cMidleCoil[7].Val = temp2.V8[1];
        cMidleCoil[8].Val = temp2.V8[2];
        cMidleCoil[9].Val = temp2.V8[3];

        cMidleCoil[40].Val = temp1.V8[0];
        cMidleCoil[41].Val = temp1.V8[1];
        cMidleCoil[42].Val = temp1.V8[2];
        cMidleCoil[43].Val = temp1.V8[3];

        if (bChaXiaoSave)
        {
            Save16BitDate(temp2.V16[0], 46);
            Save16BitDate(temp2.V16[1], 47);
            //Write24C04_32bit(46, temp2.V32);
            bChaXiaoSave = 0;
        }
    }
}
/**
 * 扫码模式G代码数据轴配置
 * 
 * @author xt (2019-02-23)
 * 
 * @param void 
 */
void GcodeAxisSelectCheck(void)
{
//  if (FactoryParam->Axis_Depth_Flag == FactoryParam->Axis_Position_Flag)
//  {
//      FactoryParam->Axis_Depth_Flag += 1;
//      Save16BitDate(FactoryParam->Axis_Depth_Flag, 90);
//  }
    if (FactoryParam->Axis_Position_Flag < X_AxisConfigFlag || FactoryParam->Axis_Position_Flag > Z_AxisConfigFlag)
    {
        FactoryParam->Axis_Position_Flag = X_AxisConfigFlag;
        Save16BitDate(FactoryParam->Axis_Position_Flag, 89);
    }
    if (FactoryParam->Axis_Depth_Flag < X_AxisConfigFlag || FactoryParam->Axis_Depth_Flag > Z_AxisConfigFlag)
    {
        FactoryParam->Axis_Depth_Flag = X_AxisConfigFlag;
        Save16BitDate(FactoryParam->Axis_Depth_Flag, 90);
    }
}
/**
 * 是否使用探头扫描
 * 
 * @author xt (2019-02-23)
 * 
 * @param void 
 */
void UserProbeCheck(void)
{
    if (SystemParam->UseProbe > 0)
    { // 使用探头扫描板头
        UseProbeFlag = 1;
        bProbeModeDisplay = 1;
    }
    else
    { // 不使用探头
        UseProbeFlag = 0;
        bProbeModeDisplay = 0;

        UserCurrentParam->BoardMode = 1;

        if (FactoryParam->WorkMode == WorkMode_Scan)
        {
            FactoryParam->WorkMode = WorkMode_Hole;
        }

        FactoryParam->Hole_Probe = 0;
        FactoryParam->Code_Probe = 0;
    }

    if (FactoryParam->Hole_Probe == 0)
    {
        UserCurrentParam->BoardMode = 1;
    }

    if (FactoryParam->Code_Probe == 0)
    {
        UserCurrentParam->BoardMode = 1;
    }
}

/**
 * 系统模式设置
 * 
 * @author xt (2019-02-23)
 * 
 * @param void 
 */
void SystemModeSet(void)
{
    CLRBIT(SYSTEMMODE_4);
    CLRBIT(SYSTEMMODE_3);
    CLRBIT(SYSTEMMODE_2);
    CLRBIT(SYSTEMMODE_1);
    CLRBIT(SYSTEMMODE_0);

    switch (SystemParam->SystemMode)
    {
    case 4:
        SETBIT(SYSTEMMODE_4);
        break;
    case 3:
        SETBIT(SYSTEMMODE_3);
        break;
    case 2:
        SETBIT(SYSTEMMODE_2);
        break;
    case 1:
        SETBIT(SYSTEMMODE_1);
        break;
    case 0:
        SETBIT(SYSTEMMODE_0);
        FactoryParam->TongCaoDepth = FactoryParam->CaoPicDepth;
        if (FactoryParam->CaoPicDepth == 0)
        {
            FactoryParam->CaoPicDepth = 500;
        }
        break;
    default:
        SETBIT(SYSTEMMODE_1);
        SystemParam->SystemMode = 1;
        break;
    }
}

void WorkModeSelect(void)
{
    bScanHole = 0;
    bLocationHole = 0;
    bLocationCao = 0;
    bWorkMode_Code = 0;

    if (bScanHole_ST)
    {
        bScanHole_ST = 0;
        FactoryParam->WorkMode = WorkMode_Scan;
        Save16BitDate(FactoryParam->WorkMode, 74);
    }
    else if (bLocationCao_ST)
    {
        bLocationCao_ST = 0;
        FactoryParam->WorkMode = WorkMode_TW;
        UserCurrentParam->CurrentArray = FactoryParam->ArrayNumber;
        bReadParam = 1;
        Save16BitDate(FactoryParam->WorkMode, 74);
    }
    else if (bLocationHole_ST)
    {
        bLocationHole_ST = 0;
        FactoryParam->WorkMode = WorkMode_Hole;
        UserCurrentParam->CurrentArray = FactoryParam->ArrayNumber;
        bReadParam = 1;
        Save16BitDate(FactoryParam->WorkMode, 74);
    }
    else if (bWorkMode_Code_ST)
    {
        bWorkMode_Code_ST = 0;
        FactoryParam->WorkMode = WorkMode_Code;
        UserCurrentParam->CurrentArray = FactoryParam->ArrayNumber;
        bReadParam = 1;
        Save16BitDate(FactoryParam->WorkMode, 74);
    }

    if (FactoryParam->WorkMode == WorkMode_Scan)
    {
        bScanHole = 1;
    }
    else if (FactoryParam->WorkMode == WorkMode_TW)
    {
        bLocationCao = 1;
    }
    else if (FactoryParam->WorkMode == WorkMode_Hole)
    {
        bLocationHole = 1;
    }
    else if (FactoryParam->WorkMode == WorkMode_Code)
    {
        bWorkMode_Code = 1;
    }
}

void TWHoleSetProc(void)
{
    CLRBIT(DISP_TW_HOLE);
    CLRBIT(DISP_TW_HOLE_M);
    CLRBIT(DISP_TW_HOLE_X);

    if (SystemParam->SystemMode < SYSTEM_TYPE3)
    {
        if (GETBIT(DISP_TW_SLOT))
        {
            return;
        }
        else if (GETBIT(DISP_TW_SLOT_ONE))
        {
            return;
        }
        else if (GETBIT(DISP_T_SLOT))
        {
            return;
        }
    }

    if (GETBIT(SET_TW_HOLE))
    {
        CLRBIT(SET_TW_HOLE);
        if (FactoryParam->TWHoleMode & 0x0001)
        {
            FactoryParam->TWHoleMode &= ~0x0001;
        }
        else
        {
            FactoryParam->TWHoleMode |= 0x0001;
        }
        Save16BitDate(FactoryParam->TWHoleMode, 85);
    }

    if (GETBIT(SET_TW_HOLE_M))
    {
        CLRBIT(SET_TW_HOLE_M);
        if ((FactoryParam->TWHoleMode & 0x0001))
        {
            if (FactoryParam->TWHoleMode & 0x0002)
            {
                FactoryParam->TWHoleMode &= ~0x0002;
            }
            else
            {
                FactoryParam->TWHoleMode |= 0x0002;
            }
            Save16BitDate(FactoryParam->TWHoleMode, 85);
        }
    }

    if (GETBIT(SET_TW_HOLE_X))
    {
        CLRBIT(SET_TW_HOLE_X);
        if ((FactoryParam->TWHoleMode & 0x0001))
        {
            if (FactoryParam->TWHoleMode & 0x0004)
            {
                FactoryParam->TWHoleMode &= ~0x0004;
            }
            else
            {
                FactoryParam->TWHoleMode |= 0x0004;
            }
            Save16BitDate(FactoryParam->TWHoleMode, 85);
        }
    }

    if (FactoryParam->TWHoleMode & 0x0001)
    {
        SETBIT(DISP_TW_HOLE);
    }
    if (FactoryParam->TWHoleMode & 0x0002)
    {
        SETBIT(DISP_TW_HOLE_M);
    }
    if (FactoryParam->TWHoleMode & 0x0004)
    {
        SETBIT(DISP_TW_HOLE_X);
    }
}

void TWSlotSetProc(void)
{
    CLRBIT(DISP_TW_SLOT);
    CLRBIT(DISP_TW_SLOT_M);
    CLRBIT(DISP_TW_SLOT_ONE);

    if (SystemParam->SystemMode < SYSTEM_TYPE3)
    {
        if (GETBIT(DISP_TW_HOLE))
        {
            return;
        }
    }

    if (GETBIT(SET_TW_SLOT))
    {
        CLRBIT(SET_TW_SLOT);
        if (FactoryParam->TWSlotMode & 0x01)
        {
            FactoryParam->TWSlotMode &= ~0x01;
        }
        else
        {
            FactoryParam->TWSlotMode |= 0x01;
        }

        Save16BitDate(FactoryParam->TWSlotMode, 86);
    }

    if (GETBIT(SET_TW_SLOT_M))
    {
        CLRBIT(SET_TW_SLOT_M);
        if ((FactoryParam->TWSlotMode & 0x01) == 0)
        {
            return;
        }

        if (FactoryParam->TWSlotMode & 0x02)
        {
            FactoryParam->TWSlotMode &= ~0x02;
        }
        else
        {
            FactoryParam->TWSlotMode |= 0x02;
        }

        Save16BitDate(FactoryParam->TWSlotMode, 86);
    }

    if (GETBIT(SET_TW_SLOT_ONE))
    {
        CLRBIT(SET_TW_SLOT_ONE);

        if (FactoryParam->TWSlotMode & 0x04)
        {
            FactoryParam->TWSlotMode &= ~0x04;
        }
        else
        {
            FactoryParam->TWSlotMode |= 0x04;
        }

        Save16BitDate(FactoryParam->TWSlotMode, 86);
    }

    if (FactoryParam->TWSlotMode & 0x01)
    {
        SETBIT(DISP_TW_SLOT);
    }
    if (FactoryParam->TWSlotMode & 0x02)
    {
        SETBIT(DISP_TW_SLOT_M);
    }
    if (FactoryParam->TWSlotMode & 0x04)
    {
        SETBIT(DISP_TW_SLOT_ONE);
    }
}

void TSlotSetProc(void)
{
    CLRBIT(DISP_T_SLOT);

    if (SystemParam->SystemMode < SYSTEM_TYPE3)
    {
        if (GETBIT(DISP_TW_HOLE))
        {
            return;
        }
    }

    if (GETBIT(SET_T_SLOT))
    {
        CLRBIT(SET_T_SLOT);
        if (FactoryParam->TSlotMode & 0x01)
        {
            FactoryParam->TSlotMode &= ~0x01;
        }
        else
        {
            FactoryParam->TSlotMode |= 0x01;
        }

        Save16BitDate(FactoryParam->TSlotMode, 77);
    }

    if (FactoryParam->TSlotMode & 0x01)
    {
        SETBIT(DISP_T_SLOT);
    }

    CLRBIT(DISP_T_SLOT_TOOL);
    if (GETBIT(SET_T_SLOT_TOOL))
    {
        CLRBIT(SET_T_SLOT_TOOL);
        if (UserParam->TSlotTool & 0x01)
        {
            UserParam->TSlotTool &= ~0x01;
        }
        else
        {
            UserParam->TSlotTool |= 0x01;
        }
        Save16BitDate(UserParam->TSlotTool, 217);
    }

    if (SystemParam->SystemMode < SYSTEM_TYPE2)
    { // 左刀  单主轴
        UserParam->TSlotTool = 0;
    }
    else if (SystemParam->SystemMode < SYSTEM_TYPE3)
    { // 右刀 单主轴加钻包
        UserParam->TSlotTool = 1;
    }

    if (UserParam->TSlotTool & 0x01)
    {
        SETBIT(DISP_T_SLOT_TOOL);
    }
}
/**
 * @brief  扫描孔槽设置
 * @note   
 * @retval None
 */
void CaoModeSelect(void)
{
    TWHoleSetProc();
    TWSlotSetProc();
    TSlotSetProc();
}
/**
 * @brief  
 * @note   
 * @retval None
 */
void ScanCodeWorkState(void)
{
    if (UserCurrentParam->TopFlag == 1)
    {
        UserCurrentParam->TopFlag = 2;
    }
    else if (UserCurrentParam->RightFlag == 1)
    {
        UserCurrentParam->RightFlag = 2;
    }
    else if (UserCurrentParam->BottomFlag == 1)
    {
        UserCurrentParam->BottomFlag = 2;
    }
    else if (UserCurrentParam->LeftFlag == 1)
    {
        UserCurrentParam->LeftFlag = 2;
    }
}

void KeyBoardProc(void)
{
    CheckStart();

    if (bRest && !bRunning)
    { //��λ
        bRest = 0;
        bRunning = 0;
        AlarmNum = 0;
        RebackXYFLG = 1;
        bResume = 1;
        cResumeStep = 1;
    }

    if (AlarmNum == 0 || AlarmNum == 10)
    {
        KeyPressIn();
    }

    if (FactoryParam->WorkMode == WorkMode_Code)
    {
        //ScanCodeWorkState();
    }
    else
    {
        cDataArea[810] = 0;
        cDataArea[811] = 0;
        cDataArea[812] = 0;
        cDataArea[813] = 0;
    }

    if (!bRunning)
    {
        LongBoardSet();
        SystemModeSet();
        WorkModeSelect();
        CaoModeSelect();
        ManualGetFileName();
//        MotorSpeedSelect();
//        HoleDrillModeSelect();
        GcodeAxisSelectCheck();
        UserProbeCheck();

        if (bClrProduct)
        {
            bClrProduct = 0;
            UserParam->ProductNum = 0;
            Save32BitDate(UserParam->ProductNum, 200);
        }
    }

    if (bRunning1 || bRunning2 || bRunning3)
    {
        bKeyEnable = 1;
    }
    else
    {
        if (bClearC)
        {
            short i = 0;
            for (i = 0; i < 20; i++)
            {
                DingData->Orign[i] = 0;
                DingData->Length[i] = 0;
                DingData->Depth[i] = 0;
            }
            bClearC = 0;
        }

        bKeyEnable = 0;

        CustomPlugSelect();
    }
}
