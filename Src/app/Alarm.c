#include "Action.h"
#include "usermemory.h"
#include "Manual.h"
#include "../hardware/servostep.h"
#include "AutoRun.h"

/************************************************/
/*
首次上电需要复位

*/
/************************************************/
typedef struct
{
    unsigned long Alarm3;
    unsigned long Alarm4;
    unsigned long Alarm5;
    unsigned long Alarm6;
    unsigned long Alarm12;
    unsigned long Alarm18;
    unsigned long Alarm20;
    unsigned long Alarm21;
    unsigned long Alarm22;
} AlarmTime_str;

static AlarmTime_str AlarmTime[2]@".ccram";
extern unsigned short DrillMotorStopStep;

void OncePowerResume(void)
{
    if (RebackXYFLG)
    {
        AlarmNum = 1;
    }
    else
    {
        if (AlarmNum == 1)
        {
            AlarmNum = 0;
        }
    }

    if (bEmgStop)
    {
        AlarmNum = 9;
    }
    else if (AlarmNum == 9)
    {
        AlarmNum = 0;
    }
}

void ServoAlarm(void)
{
    if (bXServoAlarm != SystemParam->XSevoAlarmState)
    {
        if (AlarmTime[0].Alarm3 < dwTickCount)
        {
            AlarmNum = 3;
            RebackXYFLG = 1;
            AlarmTime[1].Alarm3 = dwTickCount + 5000;
            if (bRunning) bAlarmFlag = 1; //M43=1;
        }
    }
    else if (AlarmNum == 3)
    {
        AlarmTime[0].Alarm3 = dwTickCount + 5000;
        if (dwTickCount > AlarmTime[1].Alarm3) AlarmNum = 0;
    }

    if (SystemParam->SystemMode == SYSTEM_TYPE1)
    {
        if (bYServoAlarm != SystemParam->YSevoAlarmState)
        {
            if (AlarmTime[0].Alarm3 < dwTickCount)
            {
                AlarmNum = 12;
                RebackXYFLG = 1;
                AlarmTime[1].Alarm12 = dwTickCount + 5000;
                if (bRunning) bAlarmFlag = 1; //M43=1;
            }
        }
        else if (AlarmNum == 12)
        {
            AlarmTime[0].Alarm12 = dwTickCount + 5000;
            if (dwTickCount > AlarmTime[1].Alarm12) AlarmNum = 0;
        }
    }
    else
    {
        if (AlarmNum == 12)
        {
            AlarmNum = 0;
        }
    }
}
void SystemAlarmProc(void)
{
    long OverLcttmp = 0;

    if (RebackXYFLG == 1)
    {
//      if (AlarmNum == 13)
//      {
//          AlarmNum = 0;
//      }
//      if (AlarmNum == 2)
//      {
//          AlarmNum = 0;
//      }
//      if (AlarmNum == 16)
//      {
//          AlarmNum = 0;
//      }
        return;
    }

    if (X_DRV && !bXDirction &&
        (!bLimitP_X_Old && (bLimitP_X || X_LIMIT_P)))
    {
        MV_Dec_Stop(X_AXIS);

        bAlarmFlag = 1;

        if (X_LIMIT_P) AlarmNum = 2;
        else AlarmNum = 16;
    }
    if (X_DRV && bXDirction && !bLimitP_X && !X_LIMIT_P)
    {
        if (AlarmNum == 2)
        {
            AlarmNum = 0;
        }
        if (AlarmNum == 16)
        {
            AlarmNum = 0;
        }
    }
    bLimitP_X_Old = bLimitP_X || X_LIMIT_P;

    if (SystemParam->SystemMode > SYSTEM_TYPE0)
    {
        if (bRightToolValve)
        {
            OverLcttmp = PositionToPluse(Y_AXIS, SystemParam->YMaxLength_R);
            MV_Set_Limit_P(Y_AXIS, OverLcttmp);
            if (Y_DRV && bYDirction && !bLimitP_Y &&
                (tYAxisStepper.cRealPosi > tYAxisStepper.cMaxLen))
            {
                bLimitP_Y = 1;
                bAlarmFlag = 1;

                MV_Dec_Stop(Y_AXIS);

                AlarmNum = 13;
            }
            if (Y_DRV && !bYDirction && (tYAxisStepper.cRealPosi < tYAxisStepper.cMaxLen))
            {
                bLimitP_Y = 0;
                if (AlarmNum == 13)
                {
                    AlarmNum = 0;
                }
            }
        }
        if (bLeftToolValve)
        {
            OverLcttmp = PositionToPluse(Y_AXIS, FactoryParam->YMaxLength_L);
            MV_Set_Limit_P(Y_AXIS, OverLcttmp);
            if (Y_DRV && bYDirction && !bLimitP_Y &&
                (tYAxisStepper.cRealPosi > tYAxisStepper.cMaxLen))
            {
                bLimitP_Y = 1;
                bAlarmFlag = 1;

                MV_Dec_Stop(Y_AXIS);

                AlarmNum = 13;
            }
            if (Y_DRV && !bYDirction && (tYAxisStepper.cRealPosi < tYAxisStepper.cMaxLen))
            {
                bLimitP_Y = 0;
                if (AlarmNum == 13)
                {
                    AlarmNum = 0;
                }
            }
        }
    }
    else
    {
        if (AlarmNum == 13)
        {
            AlarmNum = 0;
        }
    }
}
unsigned long bpq_alarm_time;

void UserAlarmProc(void)
{
    char flag = 0;

    if (SystemParam->SystemMode == SYSTEM_TYPE0)
    {
        if (bDrillValve != bQiGangComeSgn)
        {
            if (AlarmTime[0].Alarm5 < dwTickCount)
            {
                AlarmTime[1].Alarm5 = dwTickCount + 5000;
                AlarmNum = 5;
                if (bRunning) bAlarmFlag = 1;
            }
        }
        else
        {
            AlarmTime[0].Alarm5 = dwTickCount + 5000;
            if (AlarmNum == 5)
            {
                if (AlarmTime[1].Alarm5 < dwTickCount) AlarmNum = 0;
            }
        }

        if (bDrillValve == bQiGangBackSgn)
        {
            if (AlarmTime[0].Alarm6 < dwTickCount)
            {
                AlarmTime[1].Alarm6 = dwTickCount + 5000;
                AlarmNum = 6;
                if (bRunning) bAlarmFlag = 1;
            }
        }
        else
        {
            AlarmTime[0].Alarm6 = dwTickCount + 5000;
            if (AlarmNum == 6)
            {
                if (AlarmTime[1].Alarm6 < dwTickCount) AlarmNum = 0;
            }
        }
    }

    if (bLeftInvertAlarm != 1)  //保存不了SystemParam->InverterAlarm_L
    {
        if (bpq_alarm_time==0)
        {
            AlarmNum = 20;
            if (bRunning) bAlarmFlag = 1; //M43=1;
        }
    }
    else
    {
        bpq_alarm_time = 5000;
        if (AlarmNum == 20)
        {
            AlarmNum = 0;
        }
    }

    if (SystemParam->SystemMode == SYSTEM_TYPE4)
    {
        if (bRightInvertAlarm != SystemParam->InverterAlarm_R)
        {
            if (AlarmTime[0].Alarm4 < dwTickCount)
            {
                AlarmNum = 4;
                AlarmTime[1].Alarm4 = dwTickCount + 5000;
                if (bRunning) bAlarmFlag = 1;
            }
        }
        else
        {
            AlarmTime[0].Alarm4 = dwTickCount + 5000;
            if (AlarmNum == 4)
            {
                if (dwTickCount > AlarmTime[1].Alarm4) AlarmNum = 0;
            }
        }
    }

    if (SystemParam->SystemMode == SYSTEM_TYPE2 || SystemParam->SystemMode == SYSTEM_TYPE3 || SystemParam->SystemMode == SYSTEM_TYPE4)
    {
        if (FactoryParam->ToolValveOrignEnable > 0)
        {
            if (bRightToolValve == bRightToolOrign)
            {
                if (AlarmTime[0].Alarm21 < dwTickCount)
                {
                    AlarmNum = 21;
                    AlarmTime[1].Alarm21 = dwTickCount + 5000;
                    if (bRunning) bAlarmFlag = 1; //M43=1;
                }
            }
            else
            {
                AlarmTime[0].Alarm21 = dwTickCount + 5000;
                if (AlarmNum == 21)
                {
                    if (dwTickCount > AlarmTime[1].Alarm21) AlarmNum = 0;
                }
            }

            if (bLeftToolValve == bLeftToolOrign)
            {
                if (AlarmTime[0].Alarm22 < dwTickCount)
                {
                    AlarmNum = 22;
                    AlarmTime[1].Alarm22 = dwTickCount + 5000;
                    if (bRunning) bAlarmFlag = 1; //M43=1;
                }
            }
            else
            {
                AlarmTime[0].Alarm22 = dwTickCount + 5000;
                if (AlarmNum == 22)
                {
                    if (dwTickCount > AlarmTime[1].Alarm22)
                    {
                        AlarmNum = 0;
                    }
                }
            }
        }
        else
        {
            if (AlarmNum == 22)
            {
                AlarmNum = 0;
            }
            if (AlarmNum == 21)
            {
                AlarmNum = 0;
            }
        }
    }

    if (FactoryParam->WorkMode == WorkMode_Code)
    {
        //      if (GETBIT(USER_USB_CONNECT))
        //      {
        //          if (AlarmNum == 18)
        //          {
        //              if (dwTickCount > AlarmTime[1].Alarm18)
        //              {
        //                  AlarmNum = 0;
        //              }
        //          }
        //      }
        //      else
        //      {
        //          UserCurrentParam->cAlarmNum = 18;
        //          AlarmTime[1].Alarm18 = dwTickCount + 5000;
        //          if (bRunning)
        //          {
        //              bAlarmFlag = 1;
        //          }
        //      }
    }
    else
    {
        if (AlarmNum == 18)
        {
            if (dwTickCount > AlarmTime[1].Alarm18)
            {
                AlarmNum = 0;
            }
        }
    }

    if (bRunning)
    {
        if (AlarmNum == 0)
        {
            AlarmNum = 10;
        }
    }
    else
    {
        if (AlarmNum == 10)
        {
            AlarmNum = 0;
        }
    }

    if (bResume)
    {
        AlarmNum = 7;
    }
    else
    {
        if (AlarmNum == 7)
        {
            AlarmNum = 0;
        }
    }

    if (AlarmNum == 10 || AlarmNum == 0 || AlarmNum == 7 || AlarmNum == 11 || AlarmNum == 1 || AlarmNum == 18)
    {
        flag = 0; //
    }
    else
    {
        flag = 1;
    }

    if (flag)
    {
        bAlarmFlag = 1;
    }
}

void StopAxis(void)
{
    if (AXIS_NUM > 0)
    {
        if (X_DRV) MV_Dec_Stop(X_AXIS);
    }
    if (AXIS_NUM > 1)
    {
        if (Y_DRV) MV_Dec_Stop(Y_AXIS);
    }
    if (AXIS_NUM > 2)
    {
        if (Z_DRV) MV_Dec_Stop(Z_AXIS);
    }
    // if (AXIS_NUM > 3)
    // {
    //     if (U_DRV)
    //         MV_Dec_Stop(U_AXIS);
    // }
}

void StopOut(void)
{
    bPress1_Vavle = 0;
    bPress2_Vavle = 0;
    bPress3_Vavle = 0;
    bPress4_Vavle = 0;
    bDrillValve = 0;
    bCuiQi = 0;

    bRightToolValve = 0;
    bLeftToolValve = 0;
    if (SystemParam->SystemMode == SYSTEM_TYPE3)
    {
        DrillMotorStopStep = 1;
    }
    else
    {
        bRightToolMotor = 0;
        bLeftToolMotor = 0;
    }
}

void StopResume(void)
{
    if (bResume)
    {
        bRBOrignXY = 0;
        bXRst = 0;
        bYRst = 0;
        bResume = 0;

        if (X_DRV) MV_Dec_Stop(X_AXIS);
        if (Y_DRV) MV_Dec_Stop(Y_AXIS);
    }
}

void StopRunnning(void)
{
    bRunning = 0;
    bRunning1 = 0;
    bRunning2 = 0;
    bRunning3 = 0;

    KeyPress1Flag = 0;
    KeyPress2Flag = 0;
    KeyPress3Flag = 0;

    ResetPeripheral();

    cRunStep = 0;
    StartFlag = 0;
}

void DrillMotorStopProc(void)
{
    static unsigned long Mortstopdelay = 0;
    if (DrillMotorStopStep == 1)
    {
        if (bInvertStart)
        {
            bInvertStart = 0;
            bLeftToolValve = 0;
            bRightToolValve = 0;
            if (GETBIT(245))
            {
                Mortstopdelay = FactoryParam->MotorStopDelay_R + 1;
            }
            else if (GETBIT(247))
            {
                Mortstopdelay = FactoryParam->MotorStopDelay_L + 1;
            }
            else
            {
                Mortstopdelay = 0;
            }
            Mortstopdelay *= 100;
            Mortstopdelay += dwTickCount;
        }

        DrillMotorStopStep = 2;
    }
    else if (DrillMotorStopStep == 2)
    {
        if (Mortstopdelay < dwTickCount)
        {
            bLeftToolMotor = 0;
            bRightToolMotor = 0;
            Mortstopdelay = 50;
            Mortstopdelay += dwTickCount;
            DrillMotorStopStep = 3;
        }
    }
    else if (DrillMotorStopStep == 3)
    {
        if (Mortstopdelay < dwTickCount)
        {
            DrillMotorStopStep = 0;
        }
    }
}
void ClearRunningData(void)
{
    bCodeWorkFlag1 = 0;
    bCodeWorkFlag2 = 0;
    bCodeWorkFlag3 = 0;
    bCodeWorkFlag4 = 0;

    cDataArea[810] = 0;
    cDataArea[811] = 0;
    cDataArea[812] = 0;
    cDataArea[813] = 0;
}

void StopKeyPro(void)
{
    bStop = 0;
    // M38 = 0;
    // M39 = 0;

    StopOut();
    StopResume();
    StopRunnning();
    ClearRunningData();
    StopAxis();
}
/**
 * @brief  停止函数
 * @note   
 * @retval None
 */
void StopProc(void)
{
    if (bStop) // && !bOldStop))
    {

        AlarmNum = 0;
        StopKeyPro();
    }
    if (bAlarmFlag || (bEmgStop))
    {
        bAlarmFlag = 0;
        StopKeyPro();
    }
    DrillMotorStopProc();
}

/************************************************/
/*
告警保护

*/
/************************************************/
void AlarmProtect(void)
{
    OncePowerResume();
    ServoAlarm();
    ServoSoftProtect();
    SystemAlarmProc();
    UserAlarmProc();

    StopProc();
}
