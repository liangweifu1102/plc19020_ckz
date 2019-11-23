#include "servostep.h"

StepperDataInfo tXAxisStepper @".ccram";
StepperDataInfo tYAxisStepper @".ccram";
StepperDataInfo tZAxisStepper @".ccram";
StepperDataInfo tUAxisStepper @".ccram";

WORD_BITS cAxisDriveState @".ccram";
WORD_BITS cAxisAlarmState @".ccram";

static StepperDataInfo *StepInfo[] = {
    &tXAxisStepper,
    &tYAxisStepper,
    &tZAxisStepper,
    &tUAxisStepper };

#define X_FreqTab_Number 150
#define Y_FreqTab_Number 150

const unsigned long FreqTab_XX[] = {
    2000, 4000, 6774, 9440, 15998, 19998, 24002, 27997, 32009, 35994,
    39997, 44016, 47979, 51982, 55995, 60022, 64018, 67952, 72046, 76008,
    79994, 83940, 87945, 91968, 95958, 100083, 104086, 107894, 111991, 116107,
    119882, 123912, 127852, 132050, 136113, 139989, 144093, 147949, 152016, 155763,
    160278, 163840, 168200, 172127, 176242, 179824, 184320, 188241, 192333, 195738,
    200166, 203856, 207684, 211659, 215789, 220083, 224552, 228024, 231606, 236560,
    240417, 244402, 248521, 251345, 255704, 260216, 263314, 268101, 271391, 276480,
    279979, 283569, 287251, 292958, 296891, 300930, 302991, 307200, 311526, 315977,
    320556, 322896, 327680, 332607, 335127, 340283, 342920, 348321, 351085, 356748,
    359648, 362596, 368640, 371737, 374888, 381351, 384667, 388042, 391476, 394971,
    398529, 402152, 405842, 409600, 413428, 417328, 421302, 425353, 429483, 433694,
    437988, 442368,
};
static unsigned short FreqTab_X[X_FreqTab_Number]@".ccram";
static unsigned short FreqTab_X2[X_FreqTab_Number]@".ccram";

const unsigned long FreqTab_YY[] = {
    2000, 4000, 9999, 11668, 13332, 15000, 16667, 18332, 19998, 21663,
    23331, 25006, 26664, 28338, 29991, 31665, 33335, 34997, 36680, 38333,
    39997, 41654, 43326, 45001, 46663, 48346, 49985, 51678, 53361, 55020,
    56641, 58359, 60022, 61697, 63376, 64958, 66621, 67331, 67952, 68690,
    69336, 69994, 70665, 71349, 72046, 72638, 73361, 73974, 74724, 75360,
    76008, 76666, 77337, 78019, 78713, 79277, 79994, 80724, 81317, 82071,
    82685, 83308, 83940, 84744, 85399, 86063, 86738, 87252, 87945, 88650,
    89367, 89912, 90649, 91398, 91968, 92739, 93326, 93921, 94725, 95337,
    95958, 96586, 97437, 98086, 98742, 99408, 100083, 100767, 101228, 101928,
    102637, 103357, 104086, 104578, 105325, 106083, 106594, 107370, 107894, 108689,
    109226, 110041, 110592, 111427, 111991,
};
static unsigned short FreqTab_Y[X_FreqTab_Number]@".ccram";
static unsigned short FreqTab_Y2[X_FreqTab_Number]@".ccram";

#define APB2_TIM_CLOCK 120000000UL //120000000UL
#define APB1_TIM_CLOCK 60000000UL
//unsigned long temp1, temp2;
void CalFreqTab(void)
{
    short i = 0;
    short j = 0; //sizeof(FreqTab_XX) / 2;

    j = sizeof(FreqTab_XX) / 4;
    for (i = 0; i < j; i++)
    {
        if (FreqTab_XX[i] > 0)
        {
            FreqTab_X[i] = APB1_TIM_CLOCK / FreqTab_XX[i] - 1;
            FreqTab_X2[i] = (FreqTab_X[i] + 1) >> 1;
        }
    }

    j = sizeof(FreqTab_YY) / 4;
    for (i = 0; i < j; i++)
    {
        if (FreqTab_YY[i] > 0)
        {
            FreqTab_Y[i] = APB2_TIM_CLOCK / FreqTab_YY[i] - 1;
            FreqTab_Y2[i] = (FreqTab_Y[i] + 1) >> 1;
        }
    }
}
/********************************************

MTU10-PWM????????ж????

*********************************************/
//#pragma inline
void XAxis_PWMProc(void)
{
    if (bXDirction) tXAxisStepper.cRealPosi--;
    else tXAxisStepper.cRealPosi++;

    if (tXAxisStepper.bAuto_Mov == 1)
    {
        tXAxisStepper.cPmovPulse--;
        if (tXAxisStepper.cPmovPulse == 0)
        {
            X_AXIS_PULSE_OFF();
            X_DRV = 0;
            tXAxisStepper.bAuto_Mov = 0;
            tXAxisStepper.cCurSpeed = 0;
        }
        else if (tXAxisStepper.cPmovPulse > tXAxisStepper.cCalDecPulse)
        {
            if (tXAxisStepper.cCurSpeed < tXAxisStepper.cCalSpeed)
            {
                tXAxisStepper.cAccDecVal++;
                if (tXAxisStepper.cAccDecVal >= tXAxisStepper.cSetAccVal)
                {
                    tXAxisStepper.cAccDecVal = 0;
                    tXAxisStepper.cCurSpeed++;

                    X_ARR = FreqTab_X[tXAxisStepper.cCurSpeed];
                    X_CCR = FreqTab_X2[tXAxisStepper.cCurSpeed];
                }
            }
        }
        else
        {
            if (tXAxisStepper.cCurSpeed > tXAxisStepper.cSetStartSpeed)
            {
                tXAxisStepper.cAccDecVal++;
                if (tXAxisStepper.cAccDecVal >= tXAxisStepper.cSetDecVal)
                {
                    tXAxisStepper.cAccDecVal = 0;
                    tXAxisStepper.cCurSpeed--;

                    X_ARR = FreqTab_X[tXAxisStepper.cCurSpeed];
                    X_CCR = FreqTab_X2[tXAxisStepper.cCurSpeed];
                }
            }
        }
    }
    else
    {
        if (tXAxisStepper.State == 3)
        {
            if (X_Orign)
            {
                tXAxisStepper.cZeroFilter++;
                if (tXAxisStepper.cZeroFilter >= 3)
                {
                    X_AXIS_PULSE_OFF();
                    X_DRV = 0;
                    tXAxisStepper.cRealPosi = 0;
                    tXAxisStepper.State = 0;
                    tXAxisStepper.cCurSpeed = 0;
                }
            }
            else
            {
                tXAxisStepper.cZeroFilter = 0;
            }
        }
        else if (tXAxisStepper.State == 1)
        {
            if (tXAxisStepper.cCurSpeed < tXAxisStepper.cSetSpeed)
            {
                ++tXAxisStepper.cAccDecVal;
                if (tXAxisStepper.cAccDecVal >= tXAxisStepper.cSetAccVal)
                {
                    tXAxisStepper.cAccDecVal = 0;
                    tXAxisStepper.cCurSpeed++;
                    X_ARR = FreqTab_X[tXAxisStepper.cCurSpeed];
                    X_CCR = FreqTab_X2[tXAxisStepper.cCurSpeed];
                }
            }
            else
            {
                tXAxisStepper.State = 0;
            }
        }
        else if (tXAxisStepper.State == 2)
        {
            if (tXAxisStepper.cCurSpeed > tXAxisStepper.cSetStartSpeed)
            {
                ++tXAxisStepper.cAccDecVal;
                if (tXAxisStepper.cAccDecVal >= tXAxisStepper.cSetDecVal)
                {
                    tXAxisStepper.cAccDecVal = 0;
                    tXAxisStepper.cCurSpeed--;
                    X_ARR = FreqTab_X[tXAxisStepper.cCurSpeed];
                    X_CCR = FreqTab_X2[tXAxisStepper.cCurSpeed];
                }
            }
            else
            {
                X_AXIS_PULSE_OFF();
                X_DRV = 0;
                tXAxisStepper.State = 0;
                tXAxisStepper.cCurSpeed = 0;
            }
        }
    }
}

/********************************************

MTU9-PWM????????ж????

*********************************************/
//#pragma inline
void YAxis_PWMProc(void)
{ // y zhou
    if (bYDirction) tYAxisStepper.cRealPosi++;
    else tYAxisStepper.cRealPosi--;

    if (tYAxisStepper.bAuto_Mov)
    {
        if (--tYAxisStepper.cPmovPulse == 0)
        {
            Y_AXIS_PULSE_OFF();
            Y_DRV = 0;
            tYAxisStepper.cCurSpeed = 0;
            tYAxisStepper.bAuto_Mov = 0;
        }
        else if (tYAxisStepper.cPmovPulse > tYAxisStepper.cCalDecPulse)
        {
            if (tYAxisStepper.cCurSpeed < tYAxisStepper.cCalSpeed)
            {
                if (++tYAxisStepper.cAccDecVal >= tYAxisStepper.cSetAccVal)
                {
                    tYAxisStepper.cAccDecVal = 0;
                    tYAxisStepper.cCurSpeed++;
                    Y_ARR = FreqTab_Y[tYAxisStepper.cCurSpeed];
                    Y_CCR = FreqTab_Y2[tYAxisStepper.cCurSpeed];
                }
            }
        }
        else
        {
            if (tYAxisStepper.cCurSpeed > tYAxisStepper.cSetStartSpeed)
            {
                if (++tYAxisStepper.cAccDecVal >= tYAxisStepper.cSetDecVal)
                {
                    tYAxisStepper.cAccDecVal = 0;
                    tYAxisStepper.cCurSpeed--;
                    Y_ARR = FreqTab_Y[tYAxisStepper.cCurSpeed];
                    Y_CCR = FreqTab_Y2[tYAxisStepper.cCurSpeed];
                }
            }
        }
    }
    else
    {
        if (tYAxisStepper.State == 3)
        {
            if (Y_Orign)
            {
                tYAxisStepper.cZeroFilter++;
                if (tYAxisStepper.cZeroFilter >= 50)
                {
                    Y_AXIS_PULSE_OFF();
                    Y_DRV = 0;
                    tYAxisStepper.cRealPosi = 0;
                    tYAxisStepper.State = 0;
                    tYAxisStepper.cCurSpeed = 0;
                }
            }
            else tYAxisStepper.cZeroFilter = 0;
        }
        else if (tYAxisStepper.State == 1)
        {
            if (tYAxisStepper.cCurSpeed < tYAxisStepper.cSetSpeed)
            {
                if (++tYAxisStepper.cAccDecVal >= tYAxisStepper.cSetAccVal)
                {
                    tYAxisStepper.cAccDecVal = 0;
                    tYAxisStepper.cCurSpeed++;
                    Y_ARR = FreqTab_Y[tYAxisStepper.cCurSpeed];
                    Y_CCR = FreqTab_Y2[tYAxisStepper.cCurSpeed];
                }
            }
            else tYAxisStepper.State = 0;
        }
        else if (tYAxisStepper.State == 2)
        {
            if (tYAxisStepper.cCurSpeed > tYAxisStepper.cSetStartSpeed)
            {
                if (++tYAxisStepper.cAccDecVal >= tYAxisStepper.cSetDecVal)
                {
                    tYAxisStepper.cAccDecVal = 0;
                    tYAxisStepper.cCurSpeed--;
                    Y_ARR = FreqTab_Y[tYAxisStepper.cCurSpeed];
                    Y_CCR = FreqTab_Y2[tYAxisStepper.cCurSpeed];
                }
            }
            else
            {
                Y_AXIS_PULSE_OFF();
                Y_DRV = 0;
                tYAxisStepper.State = 0;
                tYAxisStepper.cCurSpeed = 0;
            }
        }
    }
}

#pragma inline
void ZAxis_PWMProc(void)
{ // y zhou
    if (bZDirction)
    {
        tZAxisStepper.cRealPosi--;
    }
    else
    {
        tZAxisStepper.cRealPosi++;
    }

    if (tZAxisStepper.bAuto_Mov)
    {
        if (--tZAxisStepper.cPmovPulse == 0)
        {
            Z_AXIS_PULSE_OFF();
            Z_DRV = 0;
            tZAxisStepper.bAuto_Mov = 0;
            tZAxisStepper.cCurSpeed = 0;
        }
        else if (tZAxisStepper.cPmovPulse > tZAxisStepper.cCalDecPulse)
        {
            if (tZAxisStepper.cCurSpeed < tZAxisStepper.cCalSpeed)
            {
                if (++tZAxisStepper.cAccDecVal >= tZAxisStepper.cSetAccVal)
                {
                    tZAxisStepper.cAccDecVal = 0;
                    tZAxisStepper.cCurSpeed++;
                    TIM9->ARR = FreqTab_Y[tZAxisStepper.cCurSpeed];
                    TIM9->CCR2 = FreqTab_Y2[tZAxisStepper.cCurSpeed];
                }
            }
        }
        else
        {
            if (tZAxisStepper.cCurSpeed > tZAxisStepper.cSetStartSpeed)
            {
                if (++tZAxisStepper.cAccDecVal >= tZAxisStepper.cSetDecVal)
                {
                    tZAxisStepper.cAccDecVal = 0;
                    tZAxisStepper.cCurSpeed--;
                    TIM9->ARR = FreqTab_Y[tZAxisStepper.cCurSpeed];
                    TIM9->CCR2 = FreqTab_Y2[tZAxisStepper.cCurSpeed];
                }
            }
        }
    }
    else
    {
        if (tZAxisStepper.State == 3)
        {
            if (Y_Orign)
            {
                tZAxisStepper.cZeroFilter++;
                if (tZAxisStepper.cZeroFilter >= 50)
                {
                    Z_AXIS_PULSE_OFF();
                    Z_DRV = 0;
                    tZAxisStepper.cRealPosi = 0;
                    tZAxisStepper.State = 0;
                    tZAxisStepper.cCurSpeed = 0;
                }
            }
            else tZAxisStepper.cZeroFilter = 0;
        }
        else if (tZAxisStepper.State == 1)
        {
            if (tZAxisStepper.cCurSpeed < tZAxisStepper.cSetSpeed)
            {
                if (++tZAxisStepper.cAccDecVal >= tZAxisStepper.cSetAccVal)
                {
                    tZAxisStepper.cAccDecVal = 0;
                    tZAxisStepper.cCurSpeed++;
                    TIM9->ARR = FreqTab_Y[tZAxisStepper.cCurSpeed];
                    TIM9->CCR2 = FreqTab_Y2[tZAxisStepper.cCurSpeed];
                }
            }
            else tZAxisStepper.State = 0;
        }
        else if (tZAxisStepper.State == 2)
        {
            if (tZAxisStepper.cCurSpeed > tZAxisStepper.cSetStartSpeed)
            {
                if (++tZAxisStepper.cAccDecVal >= tZAxisStepper.cSetDecVal)
                {
                    tZAxisStepper.cAccDecVal = 0;
                    tZAxisStepper.cCurSpeed--;
                    TIM9->ARR = FreqTab_Y[tZAxisStepper.cCurSpeed];
                    TIM9->CCR2 = FreqTab_Y2[tZAxisStepper.cCurSpeed];
                }
            }
            else
            {
                Z_AXIS_PULSE_OFF();
                Z_DRV = 0;
                tZAxisStepper.State = 0;
                tZAxisStepper.cCurSpeed = 0;
            }
        }
    }
}

void AxisOrign_PWM(uint16 cAxisNo)
{
    if (cAxisNo == X_AXIS)
    {
        DR_CCW_X();
        tXAxisStepper.bAuto_Mov = 0;
        tXAxisStepper.State = 3;
        tXAxisStepper.cSetSpeed = 3;
        tXAxisStepper.cSetStartSpeed = 1;
        tXAxisStepper.cAccDecVal = 0;
        StartPWM(X_AXIS);
    }
    else
    {
        DR_CCW_Y();
        tYAxisStepper.bAuto_Mov = 0;
        tYAxisStepper.State = 3;
        tYAxisStepper.cSetSpeed = 1;
        tYAxisStepper.cSetStartSpeed = 0;
        tYAxisStepper.cAccDecVal = 0;
        StartPWM(Y_AXIS);
    }
}

void StartPWM(unsigned short cAxisNo)
{
    if (cAxisNo == X_AXIS)
    {
        tXAxisStepper.cCurSpeed = tXAxisStepper.cSetStartSpeed;

        X_ARR = FreqTab_X[tXAxisStepper.cCurSpeed];
        X_CCR = FreqTab_X2[tXAxisStepper.cCurSpeed];
        X_CNT = 0;
        X_AXIS_PULSE_ON();
        X_DRV = 1;
    }
    else if (cAxisNo == Y_AXIS)
    {
        tYAxisStepper.cCurSpeed = tYAxisStepper.cSetStartSpeed;

        Y_ARR = FreqTab_Y[tYAxisStepper.cCurSpeed];
        Y_CCR = FreqTab_Y2[tYAxisStepper.cCurSpeed];
        Y_CNT = 0;
        Y_AXIS_PULSE_ON();
        Y_DRV = 1;
    }
    else if (cAxisNo == Z_AXIS)
    {
        tZAxisStepper.cCurSpeed = tZAxisStepper.cSetStartSpeed;

        TIM9->ARR = FreqTab_Y[tZAxisStepper.cCurSpeed];
        TIM9->CCR2 = FreqTab_Y2[tZAxisStepper.cCurSpeed];

        Z_AXIS_PULSE_ON();
        Z_DRV = 1;
    }
}

void LCalRunPulse(StepperDataInfo *tCurSetStpper)
{
    if (tCurSetStpper->cDestPosi & 0x80000000)
    {
        tCurSetStpper->cPmovPulse = 0 - tCurSetStpper->cDestPosi;
        if (tCurSetStpper->cAxisNo == X_AXIS) DR_CCW_X();
        else if (tCurSetStpper->cAxisNo == Y_AXIS) DR_CCW_Y();
        else if (tCurSetStpper->cAxisNo == Z_AXIS) DR_CCW_Z();
        else tCurSetStpper->cPmovPulse = 0;
    }
    else
    {
        tCurSetStpper->cPmovPulse = tCurSetStpper->cDestPosi;
        if (tCurSetStpper->cAxisNo == X_AXIS) DR_CW_X();
        else if (tCurSetStpper->cAxisNo == Y_AXIS) DR_CW_Y();
        else if (tCurSetStpper->cAxisNo == Z_AXIS) DR_CW_Z();
        else tCurSetStpper->cPmovPulse = 0;
    }
    tCurSetStpper->cCalAccPulse = tCurSetStpper->cSetAccVal * (tCurSetStpper->cSetSpeed - tCurSetStpper->cSetStartSpeed);
    tCurSetStpper->cCalDecPulse = tCurSetStpper->cSetDecVal * (tCurSetStpper->cSetSpeed - tCurSetStpper->cSetStartSpeed);
    tCurSetStpper->cCurSpeed = tCurSetStpper->cSetStartSpeed;
    tCurSetStpper->cAccDecVal = 0;

    if (tCurSetStpper->cPmovPulse < (tCurSetStpper->cCalAccPulse + tCurSetStpper->cCalDecPulse))
    {
        tCurSetStpper->cCalSpeed = tCurSetStpper->cPmovPulse / (tCurSetStpper->cSetAccVal + tCurSetStpper->cSetDecVal);
        tCurSetStpper->cCalDecPulse = tCurSetStpper->cSetDecVal * (tCurSetStpper->cCalSpeed - tCurSetStpper->cSetStartSpeed);
    }
    else
    {
        tCurSetStpper->cCalSpeed = tCurSetStpper->cSetSpeed;
    }
}

void MV_Set_Startv(unsigned short cAxisNo, unsigned short cSpeed)
{
    if (cAxisNo < AXIS_NUM)
    {
        StepInfo[cAxisNo]->cSetStartSpeed = cSpeed;
    }
}

void MV_Set_Speed(unsigned short cAxisNo, unsigned short cSpeed)
{
    if (cAxisNo < AXIS_NUM) StepInfo[cAxisNo]->cSetSpeed = cSpeed;
}

void MV_Set_Acc(unsigned short cAxisNo, unsigned short cSetAcc)
{
    if (cAxisNo < AXIS_NUM)
    {
        StepInfo[cAxisNo]->cSetAccVal = cSetAcc;
        StepInfo[cAxisNo]->cAccDecVal = 0;
    }
}

void MV_AccDec_Set(unsigned short cAxisNo, unsigned short cSetAcc, unsigned short cSetDec)
{
    if (cAxisNo < AXIS_NUM)
    {
        StepInfo[cAxisNo]->cSetAccVal = cSetAcc;
        StepInfo[cAxisNo]->cSetDecVal = cSetDec;
        StepInfo[cAxisNo]->cAccDecVal = 0;
    }
}

void MV_Set_Dec(unsigned short cAxisNo, unsigned short cSetDec)
{
    if (cAxisNo < AXIS_NUM)
    {
        StepInfo[cAxisNo]->cSetDecVal = cSetDec;
        StepInfo[cAxisNo]->cAccDecVal = 0;
    }
}

long MV_Get_Command_Pos(unsigned short cAxisNo)
{
    long cRealPosi;

    if (cAxisNo < AXIS_NUM)
    {
        cRealPosi = StepInfo[cAxisNo]->cRealPosi;
    }
    return cRealPosi;
}

void MV_Set_Command_Pos(unsigned short cAxisNo, long pos)
{
    if (cAxisNo < AXIS_NUM)
    {
        StepInfo[cAxisNo]->cRealPosi = pos;
    }
}

void MV_Pmove(unsigned short cAxisNo, unsigned long dwPosi)
{
    if (cAxisNo < AXIS_NUM)
    {
        StepInfo[cAxisNo]->cDestPosi = dwPosi - StepInfo[cAxisNo]->cRealPosi;
        //StepInfo[cAxisNo]->cSetStartSpeed = 3;
        StepInfo[cAxisNo]->State = 0;
        StepInfo[cAxisNo]->cAccDecVal = 0;
        LCalRunPulse(StepInfo[cAxisNo]);

        if (StepInfo[cAxisNo]->cPmovPulse)
        {
            StepInfo[cAxisNo]->bAuto_Mov = 1;
            StartPWM(cAxisNo);
        }
    }
}

//void MV_Pmove2(unsigned short cAxisNo, unsigned long dwPosi)
//{
//    if (cAxisNo < AXIS_NUM)
//    {
//        StepInfo[cAxisNo]->cDestPosi = dwPosi - StepInfo[cAxisNo]->cRealPosi;
//        StepInfo[cAxisNo]->cSetStartSpeed = 0;
//        StepInfo[cAxisNo]->State = 0;
//        StepInfo[cAxisNo]->cAccDecVal = 0;
//        LCalRunPulse(StepInfo[cAxisNo]);
//
//        if (StepInfo[cAxisNo]->cPmovPulse)
//        {
//            StepInfo[cAxisNo]->bAuto_Mov = 1;
//            StartPWM(cAxisNo);
//        }
//    }
//}

void MV_Set_Limit_P(unsigned short cAxisNo, unsigned long dwMaxLen)
{
    if (cAxisNo < AXIS_NUM)
    {
        StepInfo[cAxisNo]->cMaxLen = dwMaxLen;
    }
}

void MV_Set_Limit_N(unsigned short cAxisNo, unsigned long dwMinLen)
{
    if (cAxisNo < AXIS_NUM)
    {
        StepInfo[cAxisNo]->cMinLen = dwMinLen;
    }
}

void MV_Suddent_Stop(unsigned short cAxisNo)
{
    if (cAxisNo < AXIS_NUM)
    {
        switch (cAxisNo)
        {
        case X_AXIS:
            X_AXIS_PULSE_OFF();
            X_DRV = 0;
            tXAxisStepper.bAuto_Mov = 0;
            break;
        case Y_AXIS:
            Y_AXIS_PULSE_OFF();
            Y_DRV = 0;
            tYAxisStepper.bAuto_Mov = 0;
            break;
        case Z_AXIS:
            Z_AXIS_PULSE_OFF();
            Z_DRV = 0;
            tZAxisStepper.bAuto_Mov = 0;
            break;
        }
        StepInfo[cAxisNo]->bAuto_Mov = 0;
        StepInfo[cAxisNo]->cRstStep = 0;
        StepInfo[cAxisNo]->State = 0;
        StepInfo[cAxisNo]->cCurSpeed = 0;
    }
}

void MV_Dec_Stop(unsigned short cAxisNo)
{
    if (cAxisNo < AXIS_NUM)
    {
        StepInfo[cAxisNo]->bAuto_Mov = 0;
        StepInfo[cAxisNo]->State = 2;
    }
}

void MV_Const_Move(unsigned short cAxisNo, unsigned short cSpeed, unsigned short dir)
{
    unsigned char sta;

    if (cAxisNo < AXIS_NUM)
    {
        StepInfo[cAxisNo]->bAuto_Mov = 0;
        switch (cAxisNo)
        {
        case X_AXIS:
            sta = X_PWM_STATE;
            if (dir) DR_CCW_X();
            else DR_CW_X();
            break;
        case Y_AXIS:
            sta = Y_PWM_STATE;
            if (dir) DR_CCW_Y();
            else DR_CW_Y();
            break;
        case Z_AXIS:
            sta = Z_PWM_STATE;
            if (dir) DR_CCW_Z();
            else DR_CW_Z();
            break;
        }
        //?????????????????????????
        if (sta == 0)
        {
            StepInfo[cAxisNo]->State = 1;
            StepInfo[cAxisNo]->cSetSpeed = cSpeed;
            //StepInfo[cAxisNo]->cSetStartSpeed = 3;
            StepInfo[cAxisNo]->cAccDecVal = 0;
            StartPWM(cAxisNo);
        }
    }
}

void ReadAxisStatus(void)
{
    if (X_PWM_STATE)
    {
        X_DRV = 1;
    }
    else
    {
        X_DRV = 0;
    }

    if (Y_PWM_STATE)
    {
        Y_DRV = 1;
    }
    else
    {
        Y_DRV = 0;
    }

//  if (Z_PWM_STATE)
//  {
//      Z_DRV = 1;
//  }
//  else
//  {
//      Z_DRV = 0;
//  }

}

void MV_Move_To_Position(unsigned short cAxisNo, long dwPosi)
{
    if (cAxisNo < AXIS_NUM)
    {
        if (dwPosi < StepInfo[cAxisNo]->cRealPosi)
        {
            MV_Dec_Stop(cAxisNo);
        }
        else
        {
            StepInfo[cAxisNo]->cDestPosi = dwPosi - StepInfo[cAxisNo]->cRealPosi;
            StepInfo[cAxisNo]->cPmovPulse = StepInfo[cAxisNo]->cDestPosi;
            StepInfo[cAxisNo]->State = 0;
            StepInfo[cAxisNo]->cAccDecVal = 0;
            StepInfo[cAxisNo]->cCalSpeed = StepInfo[cAxisNo]->cSetSpeed;
            StepInfo[cAxisNo]->cCalDecPulse = (StepInfo[cAxisNo]->cSetSpeed - StepInfo[cAxisNo]->cSetStartSpeed);
            StepInfo[cAxisNo]->cCalDecPulse *= StepInfo[cAxisNo]->cSetDecVal;
            StepInfo[cAxisNo]->cCalDecPulse += 50;

            StepInfo[cAxisNo]->bAuto_Mov = 1;
        }
    }
}

/**
 * 连续走
 * 
 * @author xt (2019/7/23)
 * 
 * @param cAxisNo 
 * @param dir 
 * @param startv 
 * @param cSpeed 
 */
void MoveAction_Const(unsigned short cAxisNo, unsigned short dir, unsigned short cSpeed)
{
    if (cAxisNo == X_AXIS)
    {
        MV_Set_Startv(cAxisNo, 0);
        MV_AccDec_Set(cAxisNo, XIDEACC, XIDEACC);
        MV_Const_Move(cAxisNo, cSpeed, dir);
    }
    else if (cAxisNo == Y_AXIS)
    {
        MV_Set_Startv(Y_AXIS, 3);
        MV_AccDec_Set(Y_AXIS, YIDEACC, YIDEACC);
        MV_Const_Move(Y_AXIS, cSpeed, dir);
    }
}

/**
 * 走绝对位置
 * 
 * @author zhang (2019/9/26)
 * 
 * @param startv 
 * @param cSpeed 
 * @param dwPosi 
 */
void SetPulse_X(unsigned short startv, unsigned short cSpeed, long dwPosi)
{
    uint16 acc = XIDEACC;
//  int32 OverLcttmp1 = PositionToPluse(X_AXIS, 10000);
//  int32 OverLcttmp2 = MV_Get_Command_Pos(X_AXIS);
//
//  OverLcttmp2 = dwPosi - OverLcttmp2;
//  if (OverLcttmp2 < 0)
//  {
//      OverLcttmp2 = 0 - OverLcttmp2;
//  }
//  if (OverLcttmp1 > OverLcttmp2)
//  {
//      acc *= 2;
//      acc /= 3;
//  }

    if (cSpeed < 3)
    {
        cSpeed = 3;
    }

    MV_Set_Startv(X_AXIS, startv);
    MV_Set_Acc(X_AXIS, acc);
    MV_Set_Dec(X_AXIS, acc);
    MV_Set_Speed(X_AXIS, cSpeed);
    MV_Pmove(X_AXIS, dwPosi);
}

void SetPulse_Y(unsigned short startv, unsigned short cSpeed, long dwPosi)
{
    uint16 acc = YIDEACC;
//  int32 OverLcttmp = PositionToPluse(Y_AXIS, 2000);
//  int32 OverLcttmp2 = MV_Get_Command_Pos(Y_AXIS);
//
//  OverLcttmp2 = dwPosi - OverLcttmp2;
//  if (OverLcttmp2 < 0)
//  {
//      OverLcttmp2 = 0 - OverLcttmp2;
//  }
//  if (OverLcttmp > OverLcttmp2)
//  {
//      acc *= 2;
//      acc /= 3;
//  }

    if (cSpeed < 3)
    {
        cSpeed = 3;
    }

    MV_Set_Startv(Y_AXIS, startv);
    MV_Set_Acc(Y_AXIS, acc);
    MV_Set_Dec(Y_AXIS, acc);
    MV_Set_Speed(Y_AXIS, cSpeed);
    MV_Pmove(Y_AXIS, dwPosi);
}

// 走绝对位置
void MoveAction_Pulse(unsigned short cAxisNo, long dwPosi, unsigned short cSpeed)
{
    if (cAxisNo == X_AXIS)
    {
        uint16 acc = XIDEACC;
//      int32 OverLcttmp1 = PositionToPluse(X_AXIS, 8000);
//      int32 OverLcttmp2 = MV_Get_Command_Pos(X_AXIS);
//
//      OverLcttmp2 = dwPosi - OverLcttmp2;
//      if (OverLcttmp2 < 0)
//      {
//          OverLcttmp2 = 0 - OverLcttmp2;
//      }
//      if (OverLcttmp1 > OverLcttmp2)
//      {
//          acc *= 2;
//          acc /= 3;
//      }
        if (cSpeed < 3)
        {
            cSpeed = 3;
        }
        MV_Set_Startv(X_AXIS, STARTSPEED_3);
        MV_Set_Acc(X_AXIS, acc);
        MV_Set_Dec(X_AXIS, acc);
        MV_Set_Speed(X_AXIS, cSpeed);
        MV_Pmove(X_AXIS, dwPosi);
    }
    else if (cAxisNo == Y_AXIS)
    {
        uint16 acc = YIDEACC;
//      int32 OverLcttmp = PositionToPluse(Y_AXIS, 2000);
//      int32 OverLcttmp2 = MV_Get_Command_Pos(Y_AXIS);
//
//      OverLcttmp2 = dwPosi - OverLcttmp2;
//      if (OverLcttmp2 < 0)
//      {
//          OverLcttmp2 = 0 - OverLcttmp2;
//      }
//      if (OverLcttmp > OverLcttmp2)
//      {
//          acc *= 2;
//          acc /= 3;
//      }

        if (cSpeed < 3)
        {
            cSpeed = 3;
        }

        MV_Set_Startv(Y_AXIS, STARTSPEED_3);
        MV_Set_Acc(Y_AXIS, acc);
        MV_Set_Dec(Y_AXIS, acc);
        MV_Set_Speed(Y_AXIS, cSpeed);
        MV_Pmove(Y_AXIS, dwPosi);
    }
}

// 走距离
void MoveAction_Dist(unsigned short cAxisNo, unsigned long dwPosi, unsigned short cSpeed)
{
    long OverLcttmp = 0;

    if (cAxisNo == X_AXIS)
    {
        OverLcttmp = PositionToPluse(X_AXIS, dwPosi);

        MV_Set_Startv(X_AXIS, 0);
        MV_AccDec_Set(X_AXIS, XIDEACC, XIDEACC);
        MV_Set_Speed(X_AXIS, cSpeed);
        MV_Pmove(X_AXIS, OverLcttmp);
    }
    else if (cAxisNo == Y_AXIS)
    {
        OverLcttmp = PositionToPluse(Y_AXIS, dwPosi);
        MV_Set_Startv(Y_AXIS, 3);
        MV_AccDec_Set(Y_AXIS, YIDEACC, YIDEACC);
        MV_Set_Speed(Y_AXIS, cSpeed);
        MV_Pmove(Y_AXIS, OverLcttmp);
    }
}

void ServoSoftProtect(void)
{
    if (bLimitAlarmEn_X)
    {
        if (tXAxisStepper.cRealPosi > tXAxisStepper.cMaxLen)
        {
            bLimitP_X = 1;
        }
        else
        {
            bLimitP_X = 0;
        }
    }
    else
    {
        bLimitP_X = 0;
    }

    if (bLimitAlarmEn_Y)
    {
        if (tYAxisStepper.cRealPosi > tYAxisStepper.cMaxLen)
        {
            bLimitP_Y = 1;
        }
        else
        {
            bLimitP_Y = 0;
        }
    }
    else
    {
        bLimitP_Y = 0;
    }

    if (bLimitAlarmEn_Z)
    {
        if (tZAxisStepper.cRealPosi > tZAxisStepper.cMaxLen)
        {
            bLimitP_Z = 1;
        }
        else
        {
            bLimitP_Z = 0;
        }
    }
    else
    {
        bLimitP_Z = 0;
    }
}


void ServoInit(void)
{
    CalFreqTab();

    XAxis_PWM_Init();
    YAxis_PWM_Init();
    //TIM13_PWM_Init();
    //TIM9_PWM_Init();

    ServoDelay = 2000; //?????500ms
    while (ServoDelay)
    ;
    //ReadInput();

    tXAxisStepper.cAxisNo = X_AXIS;
    tXAxisStepper.cDestPosi = 0;
    tXAxisStepper.cSetStartSpeed = 0;
    tXAxisStepper.cSetSpeed = 20;
    tXAxisStepper.cSetAccVal = 50;
    tXAxisStepper.cSetDecVal = 50;
    tXAxisStepper.State = 0;
    tXAxisStepper.cAccDecVal = 0;

    tYAxisStepper.cAxisNo = Y_AXIS;
    tYAxisStepper.cDestPosi = 0;
    tYAxisStepper.cSetStartSpeed = 0;
    tYAxisStepper.cSetSpeed = 20;
    tYAxisStepper.cSetAccVal = 50;
    tYAxisStepper.cSetDecVal = 50;
    tYAxisStepper.State = 0;
    tYAxisStepper.cAccDecVal = 0;

    tZAxisStepper.cAxisNo = Z_AXIS;
    tZAxisStepper.cDestPosi = 0;
    tZAxisStepper.cSetStartSpeed = 0;
    tZAxisStepper.cSetSpeed = 20;
    tZAxisStepper.cSetAccVal = 50;
    tZAxisStepper.cSetDecVal = 50;
    tZAxisStepper.State = 0;
    tZAxisStepper.cAccDecVal = 0;

    ServoDelay = 1000;
    while (ServoDelay)
    ;
}
