/*
 * @Description: 
 * @Author: huacong
 * @LastEditors: ccc
 * @Date: 2019-04-17 14:32:41
 * @LastEditTime: 2019-05-06 16:12:09
 */
#include "config.h"

//long dwRealPosi, cSetPulse;
WORD_BITS cAxisManuRunState;
/************************************************/
/*
Y���ֶ�

*/
/************************************************/
void ManualProc_YAxis(void)
{
    unsigned long Speedtmp = 0;
    long Distancetmp = 0;
    //  long temp = 0;

    Speedtmp = SpeedParam->YManualSpeed;
    if ((bDotMVST))
    {
        if (!(Y_Dotting || Y_DRV)) //&&(!(bXRst || bYRst )))
        {
            if (bMVYn && !Y_Orign)
            {
                Y_Dotting = 1;

                Distancetmp = PositionToPluse(Y_AXIS, FactoryParam->DotDistance);
                Distancetmp = tYAxisStepper.cRealPosi - Distancetmp;
                MoveAction_Pulse(Y_AXIS, Distancetmp, Speedtmp);
                bMVYn = 0;
            }
            else if (bMVYp && !bLimitP_Y)
            {
                Y_Dotting = 1;

                Distancetmp = PositionToPluse(Y_AXIS, FactoryParam->DotDistance);
                Distancetmp = tYAxisStepper.cRealPosi + Distancetmp;
                MoveAction_Pulse(Y_AXIS, Distancetmp, Speedtmp);
                bMVYp = 0;
            }
        }
        if (!Y_DRV)
        {
            Y_Dotting = 0;
        }
    }
    else
    {
        if (!(Y_DRV || Y_Dotting)) //&&!(bXRst || bYRst ))
        {
            if (bMVYn && !Y_Orign)
            {
                Y_Dotting = 1;
                MV_Set_Startv(Y_AXIS, 0);
                MV_AccDec_Set(Y_AXIS, 80, 80);
                MV_Const_Move(Y_AXIS, Speedtmp, 1);
            }
            else if (bMVYp && !bLimitP_Y)
            {
                Y_Dotting = 1;
                MV_Set_Startv(Y_AXIS, 0);
                MV_AccDec_Set(Y_AXIS, 80, 80);
                MV_Const_Move(Y_AXIS, Speedtmp, 0);
            }
        }
        if (Y_Dotting &&
            (!(bMVYn || bMVYp) || (bMVYn && Y_Orign) || bMVYp && (bLimitP_Y)))
        {
            bMVYn = 0;
            bMVYp = 0;
            Y_Dotting = 0;
            MV_Dec_Stop(Y_AXIS);
        }
    }
}

void ManualProc_ZAxis(void)
{
    unsigned long Speedtmp = SpeedParam->YManualSpeed;
    long Distancetmp = FactoryParam->DotDistance;
    long OverLcttmp = 0;

    if ((bDotMVST))
    {
        if (!(Z_Dotting || Z_DRV)) //&&(!(bXRst || bYRst )))
        {
            if (bMVYn && !Y_Orign)
            {
                Z_Dotting = 1;

                OverLcttmp = PositionToPluse(Y_AXIS, Distancetmp);
                OverLcttmp = tZAxisStepper.cRealPosi - OverLcttmp;
                MoveAction_Pulse(Z_AXIS, OverLcttmp, Speedtmp);
                bMVYn = 0;
            }
            else if (bMVYp && !bLimitP_Y)
            {
                Z_Dotting = 1;

                OverLcttmp = PositionToPluse(Y_AXIS, Distancetmp);
                OverLcttmp = tZAxisStepper.cRealPosi + OverLcttmp;
                MoveAction_Pulse(Z_AXIS, OverLcttmp, Speedtmp);
                bMVYp = 0;
            }
        }
        if (!Z_DRV)
        {
            Z_Dotting = 0;
        }
    }
    else
    {
        if (!(Z_DRV || Z_Dotting)) //&&!(bXRst || bYRst ))
        {
            if (bMVYn && !Y_Orign)
            {
                Z_Dotting = 1;
                MV_Set_Startv(Z_AXIS, 0);
                MV_AccDec_Set(Z_AXIS, 80, 80);
                MV_Const_Move(Z_AXIS, Speedtmp, 1);
            }
            else if (bMVYp && !bLimitP_Y)
            {
                Z_Dotting = 1;
                MV_Set_Startv(Z_AXIS, 0);
                MV_AccDec_Set(Z_AXIS, 80, 80);
                MV_Const_Move(Z_AXIS, Speedtmp, 0);
            }
        }
        if (Z_Dotting &&
            (!(bMVYn || bMVYp) || (bMVYn && Y_Orign) || bMVYp && (bLimitP_Y)))
        {
            bMVYn = 0;
            bMVYp = 0;
            Z_Dotting = 0;
            MV_Dec_Stop(Z_AXIS);
        }
    }
}
/************************************************/
/*
X���ֶ�

*/
/************************************************/
void ManualProc_XAxis(void)
{
    unsigned long Speedtmp;
    long Distancetmp;

    Speedtmp = SpeedParam->ManualSpeed;
    if ((bDotMVST))
    {
        if (!(X_Dotting || X_DRV))
        {
            if (bMVXn && !X_Orign)
            {
                X_Dotting = 1;

                Distancetmp = PositionToPluse(X_AXIS, FactoryParam->DotDistance);
                Distancetmp = tXAxisStepper.cRealPosi - Distancetmp;
                MoveAction_Pulse(X_AXIS, Distancetmp, Speedtmp);
                bMVXn = 0;
            }
            else if (bMVXp && !bLimitP_X)
            {
                X_Dotting = 1;
                Distancetmp = PositionToPluse(X_AXIS, FactoryParam->DotDistance);
                Distancetmp = tXAxisStepper.cRealPosi + Distancetmp;
                MoveAction_Pulse(X_AXIS, Distancetmp, Speedtmp);
                bMVXp = 0;
            }
        }
        if (!X_DRV)
        {
            X_Dotting = 0;
        }
    }
    else
    {
        if (!(X_DRV || X_Dotting)) //&&!(bXRst || bYRst ))
        {
            if (bMVXn && !X_Orign)
            {
                X_Dotting = 1;
                MV_Set_Startv(X_AXIS, 0);
                MV_AccDec_Set(X_AXIS, 150, 150);
                MV_Const_Move(X_AXIS, Speedtmp, 1);
            }
            else if (bMVXp && !bLimitP_X)
            {
                X_Dotting = 1;
                MV_Set_Startv(X_AXIS, 0);
                MV_AccDec_Set(X_AXIS, 150, 150);
                MV_Const_Move(X_AXIS, Speedtmp, 0);
            }
        }
        if (X_Dotting &&
            (!(bMVXn || bMVXp) || (bMVXn && X_Orign) ||
             (bMVXp && (bLimitP_X))))
        {
            bMVXn = 0;
            bMVXp = 0;
            X_Dotting = 0;
            MV_Dec_Stop(X_AXIS);
        }
    }
}
/************************************************/
/*
IO�ֶ�

*/
/************************************************/
void ManualProc_IO(void)
{
    if (M19)
    {
        M19 = 0;
        Y10 = ~Y10;
    }
    if (M20)
    {
        M20 = 0;
        Y00 = ~Y00;
    }

    if (M21)
    {
        M21 = 0;
        Y01 = ~Y01;
    }

    if (M22)
    {
        Y02 = ~Y02;
        M22 = 0;
    }

    if (M23)
    {
        Y03 = ~Y03;
        M23 = 0;
    }

    if (M24)
    {
        M24 = 0;
        Y04 = ~Y04;
    }

    if (M28)
    {
        M28 = 0;
        Y06 = ~Y06;
    }

    if (SystemParam->SystemMode == SYSTEM_TYPE3)
    {
        if (M29)
        {
            M29 = 0;

            if (GETBIT(248))
            { // Y10变频器已经启动
            }
            else
            {
                if (GETBIT(245))
                { // Y05已经输出
                }
                else
                {
                    Y07 = ~Y07;
                }
            }
        }
        if (M25)
        {
            M25 = 0;
            if (GETBIT(248))
            { //Y10 变频器已经启动
            }
            else
            {
                if (GETBIT(247))
                { // Y07已经输出
                }
                else
                {
                    Y05 = ~Y05;
                }
            }
        }
        if (M19)
        {
            M19 = 0;
            Y10 = ~Y10;
        }
    }
    else
    {
        if (M25)
        {
            M25 = 0;
            Y05 = ~Y05;
        }
        if (M29)
        {
            M29 = 0;
            Y07 = ~Y07;
        }
    }

    if (SystemParam->SystemMode == SYSTEM_TYPE0)
    { // ���ŷ�ģʽ
        if (bMVYn)
        {
            bDrillValve = 0;
        }

        if (bMVYp)
        {
            bDrillValve = 1;
        }
    }
}

/************************************************/
/*
�ֶ�����

*/
/************************************************/
void ManualAction(void)
{
    //if ((!bRunning) && (!bRBOrignXY) && (!bResume) && !bStop)
    {
        //SystemMode = 1;
        if (AXIS_NUM > 2)
        {
            ManualProc_ZAxis();
        }
        if (SystemParam->SystemMode != SYSTEM_TYPE0)
        {
            ManualProc_YAxis();
        }

        ManualProc_XAxis();

        ManualProc_IO();
    }
}
uint16 cXRstStep = 0;
uint16 cYRstStep = 0;
uint16 cZRstStep = 0;
uint16 cURstStep = 0;
/************************************************/
/*
Y���ԭ��  ����Ḵλ

*/
/************************************************/
void RebackYaxisOrign(void)
{
    int32 OverLcttmp;
    unsigned short speed;

    if (cYRstStep == 1)
    {
        if (Y_Orign)
        {
            cYRstStep = 8;
            MV_Set_Startv(Y_AXIS, 0);
            MV_AccDec_Set(Y_AXIS, 100, 100);
            MV_Const_Move(Y_AXIS, 0, YCW); // 0);// Զ��ԭ��
        }
        else
        {
            cYRstStep = 2;
        }
    }
    else if (cYRstStep == 8) // && !X_DRV)
    {
        if (!Y_Orign)
        {
            cYRstStep = 4;
            MV_Dec_Stop(Y_AXIS);
        }
    }
    else if (cYRstStep == 2)
    {
        if (!Y_DRV)
        {
            cYRstStep = 3;
            MV_Set_Startv(Y_AXIS, 0);
            MV_AccDec_Set(Y_AXIS, 100, 100);
            speed = SystemParam->YResetSpeed;
            MV_Const_Move(Y_AXIS, speed, YCCW); // 1);// ��ԭ����
        }
    }
    else if (cYRstStep == 3)
    {
        if (Y_Orign)
        {
            MV_Dec_Stop(Y_AXIS);
            cYRstStep = 7;
        }
    }
    else if (cYRstStep == 7)
    {
        if (!Y_DRV)
        {
            cYRstStep = 8;
            MV_Set_Startv(Y_AXIS, 0);
            MV_AccDec_Set(Y_AXIS, 100, 100);
            MV_Const_Move(Y_AXIS, 0, YCW); // 0);// Զ��ԭ��
        }
    }
    else if (cYRstStep == 4)
    {
        if (!Y_DRV)
        {
            cYRstStep = 5;
        }
    }
    else if (cYRstStep == 5) // && !X_DRV)
    {
        cYRstStep = 6;

        AxisOrign_PWM(Y_AXIS);
    }
    else if (cYRstStep == 6)
    {
        if (Y_DRV)
        {
            return;
        }

        OverLcttmp = UserParam->YOrignOffset;
        OverLcttmp = PositionToPluse(Y_AXIS, OverLcttmp);
        SetPulse_Y(0, 3, OverLcttmp);
        cYRstStep = 9;
    }
    else if (cYRstStep == 9)
    {
        if (Y_DRV)
        {
            return;
        }

        MV_Set_Command_Pos(Y_AXIS,0);
        cYRstStep = 0;
        bYRst = 0;
    }
}
/************************************************/
/*
X���ԭ��

*/
/************************************************/

void RebackXaxisOrign(void)
{
    int32 OverLcttmp;
    static unsigned long ResetEncodeDelay = 0;
    unsigned short speed;// = SystemParam->XResetSpeed; //10;

    if (cXRstStep == 1)
    {
        if (X_Orign)
        {
            cXRstStep = 8;
            MV_Set_Startv(X_AXIS, 0);
            MV_AccDec_Set(X_AXIS, 50, 50);
            MV_Const_Move(X_AXIS, 3, XCW); // 0);// Զ��ԭ��
        }
        else
        {
            cXRstStep = 2;
        }
    }
    else if (cXRstStep == 2)
    {
        if (!X_DRV)
        {
            cXRstStep = 3;
            speed = SystemParam->XResetSpeed;
            MV_Set_Startv(X_AXIS, 0);
            MV_AccDec_Set(X_AXIS, 150, 150);
            MV_Const_Move(X_AXIS, speed, XCCW); // 1);// ��ԭ����
        }
    }
    else if (cXRstStep == 3)
    {
        if (X_Orign)
        {
            MV_Dec_Stop(X_AXIS);
            cXRstStep = 7;
        }
    }
    else if (cXRstStep == 4 && !X_DRV)
    {
        cXRstStep = 5;
    }
    else if (cXRstStep == 5 && !X_DRV)
    {
        cXRstStep = 6;
        AxisOrign_PWM(X_AXIS);
    }
    else if (cXRstStep == 6)
    {
        if (X_DRV)
        {
            return;
        }

        OverLcttmp = UserParam->XOrignOffset;
        OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
        SetPulse_X(0, 3, OverLcttmp);
        cXRstStep = 10;
    }
    else if (cXRstStep == 7 && !X_DRV)
    {
        cXRstStep = 8;
        MV_Set_Startv(X_AXIS, 0);
        MV_AccDec_Set(X_AXIS, 50, 50);
        MV_Const_Move(X_AXIS, 3, XCW); // 0);
    }
    else if (cXRstStep == 8)
    {
        if (!X_Orign)
        {
            cXRstStep = 4;
            MV_Dec_Stop(X_AXIS);
        }
    }
    else if (cXRstStep == 9)
    {
        if (ResetEncodeDelay < dwTickCount)
        {
            cXRstStep = 0;
            setEncoderCount(0);
            bXRst = 0;
        }
    }
    else if (cXRstStep == 10)
    {
        if (X_DRV)
        {
            return;
        }
        MV_Set_Command_Pos(X_AXIS,0);
        cXRstStep = 9; 
        ResetEncodeDelay = dwTickCount + 500;
    }
}

void QiGangReBack(void)
{
    if (bQiGangComeSgn || bDrillValve)
    {
        bDrillValve = 0;
        bRightToolMotor = 0;
    }

    if (bQiGangBackSgn)
    {
        bYRst = 0;
        Y_DRV = 0;
    }
}
/************************************************/
/*
XY�����

*/
/************************************************/
void RebackXYOrign(void)
{
}

/************************************************/
/*
����

*/
/************************************************/
void ResumeLocation(void)
{
    //UserCurrentParam->debugdata = cRunStep;
    if (bResume) // && (NOAirFLG == 0))
    {
        if (cResumeStep == 1)
        {
            MV_Dec_Stop(X_AXIS);
            MV_Dec_Stop(Y_AXIS);

            bLimitAlarmEn_X = 0;
            bLimitAlarmEn_Y = 0;

            bLeftToolValve = 0;
            bLeftToolMotor = 0;
            bRightToolValve = 0;
            bRightToolMotor = 0;
            bPress1_Vavle = 0;
            bPress2_Vavle = 0;
            bPress3_Vavle = 0;
            bPress4_Vavle = 0;
            ResetPeripheral();
            cResumeStep = 2;
        }
        else if ((cResumeStep == 2) && (RstPphlDlay == 0))
        {
            bRBOrignXY = 1;
            bXRst = 1;
            bYRst = 1;
            cYRstStep = 1;
            cXRstStep = 1;
            cResumeStep = 3;
        }
        else if (cResumeStep == 3)
        {
            if (SystemParam->SystemMode != SYSTEM_TYPE0)
            {
                RebackYaxisOrign();
            }
            else
            {
                QiGangReBack();
            }

            if (!bYRst && !(Y_DRV))
            {
                cResumeStep = 4;
            }
        }
        else if ((cResumeStep == 4))
        {
            RebackXaxisOrign();
            if (!bXRst && !X_DRV)
            {
                cResumeStep = 5;
            }
        }
        else if ((cResumeStep == 5))
        {
            long OverLcttmp;

            OverLcttmp = PositionToPluse(X_AXIS, SystemParam->XMaxLength);
            MV_Set_Limit_P(X_AXIS, OverLcttmp);
            bLimitAlarmEn_X = 1;
//            OverLcttmp = PositionToPluse(Y_AXIS, SystemParam->YMaxLength);
//            MV_Set_Limit_P(Y_AXIS, OverLcttmp);
//            bLimitAlarmEn_Y = 1;

            cResumeStep = 0;
            bRBOrignXY = 0;
            bResume = 0;
            RebackXYFLG = 0;
        }
    }
}
