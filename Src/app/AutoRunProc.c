/*
 * @Description: 
 * @Author: huacong
 * @LastEditors: ccc
 * @Date: 2019-04-17 14:32:41
 * @LastEditTime: 2019-05-06 21:45:05
 */
#include "config.h"
#include "encode.h"
#include "userconf.h"
#include "gcodea.h"

extern volatile long ulEncoderCount;
unsigned char cRunStep = 0; //
unsigned char cWorkRunStep = 0; //
static uint8 LookForBoardHeadstep = 0;
unsigned short DrillMotorStopStep = 0;

static unsigned long HoleCurrrentDrillDepth = 0;

unsigned char OneHolePlugFlag = 0; // �����Զ��жϲ���λ�ñ�ʾ��
unsigned char TwoHolePlugFlag = 0; //

unsigned long DelayTime2;
unsigned short WorkingFlag = 0;
unsigned long NoworkStopdelay;
long XWorkOrignPos = 0; // 使用探头, 脉冲数

unsigned short NeedWorkNum;
unsigned short CodeNumber;

// 不使用探头
long _gDingHoleDist = 0;
long _gDingSlotDist = 0;

unsigned short MotorCoilSelectStep = 0;
unsigned long MotorCoilSelectDelay = 0;

unsigned short CurDepth;
unsigned short CurDepthNum;

static long DingweiDist[20];
static long DingweiWitch[30];
static long DingweiDepth[20];
static unsigned char DingWorkFlag;
static unsigned char SlotTypeFlag;
static long DingWeiCao[30];
static long DingCaoWitch[30];
static short DingCaoDepth[30];

long CodeWorkOrign[40];           // 500
long CodeWorkLength[40];          // 540
unsigned short CodeWorkDepth[40]; // 580

static long infos[100];
static long Winfos[100];

long HoleinfoF[100];
long HoleinfoB[100];

static unsigned long DrillRunStepDelay;
static unsigned long ToolValveOutDelay;
static unsigned char cCaoMoveDir = 0;

char currentHoleNum = 0;
char currentWorkHoleNum = 0;
uint8 UseProbeFlag = 0;

unsigned char cScanDrillRunStep = 0;
unsigned char cScanHoleStep = 0;

unsigned char cDrillRunStep = 0;

static unsigned char cDingHoleWorkStep = 0;
static unsigned char cDingHoleScanStep = 0;

static unsigned char cTongCaoStartFlag = 0;

unsigned char cTongCao2ScanStep = 0;
static unsigned char cTongCao2RunStep = 0;

static unsigned char cDuanCaoRunStep = 0;
unsigned char cWorkScanStep = 0;

static long BoardHead = 0;         // ��ͷ
static unsigned long BoardEnd = 0; // ��β

static long CurWoodLength = 0; // ��ͷ
static long endposition = 0;   //��β

volatile char CheckHoleNum[2] = { 0 };
unsigned char StartFlag = 0;

volatile long CheckSgnFilter = 0;
volatile long CheckSgnFilterOff = 0;

//volatile
unsigned char flgSgn = 0;
//volatile
unsigned char flgSgnOld = 0;

static volatile char NoObjectFlag = 0;

unsigned char cToolOutFlag = 0;

TempDist_str TempDist;

static unsigned short CurrentWorkDepth = 0;
static unsigned long CurrentWorkOrign = 0;
static unsigned long CurrentWorkLength = 0;
static short CurrentToolOffsetX = 0;
static short CurrentToolOffsetY = 0;
static long CurrentBoardOrign = 0;

static void DataCompare(void);
static void ScanCheckHoleProc(void);
static void ScanDrillHoleRun(void);
static void OutSideStop(void);
static void HoleDrillDepthProcess(long depth);
static void DrillingHoleProc(long yoffset, long depth);
static void DrillingSlotProc(SlotWorkData_str *pdata);
static void ToolOutProc(void);

static void RightToolOutProc(void)
{
    bLeftToolValve = 0;
    if (!bRightToolValve)
    {
        bRightToolValve = 1;
        ToolValveOutDelay = 100UL * UserParam->ToolValveDelay;
        ToolValveOutDelay += dwTickCount;
    }

    if (!bRightToolMotor)
    {
        bRightToolMotor = 1;
        DelayTime2 = 100UL * FactoryParam->RightMotorStartDelay;
        DelayTime2 += dwTickCount;
    }
}

static void LeftToolOutProc(void)
{
    bRightToolValve = 0;
    if (!bLeftToolValve)
    {
        bLeftToolValve = 1;
        ToolValveOutDelay = 100UL * UserParam->ToolValveDelay;
        ToolValveOutDelay += dwTickCount;
    }

    if (!bLeftToolMotor)
    {
        bLeftToolMotor = 1;
        DelayTime2 = 100UL * FactoryParam->RightMotorStartDelay;
        DelayTime2 += dwTickCount;
    }
}

void Work1StopCheck(void)
{
    if (SystemParam->DemoFunction > 0)
    {
        bRunning1 = 0;
        bRunning2 = 0;
        bRunning3 = 0;
        if (KeyPress2Flag > 0)
        {
            bRunning2 = 1;
        }
        else if (KeyPress3Flag > 0)
        {
            bRunning3 = 1;
        }
        else
        {
            bRunning1 = 1;
        }
    }
    else
    {
        bRunning1 = 0;
    }

    if (bRunning2)
    {
        WorkingFlag = 1;
    }
    else if (bRunning3)
    {
        WorkingFlag = 3;
    }
    else
    {
        bCuiQi = 0;
        WorkingFlag = 0;
        MoveAction_Pulse(X_AXIS, TempDist.Work1Dist, SpeedParam->XIdleSpeed);
    }
}

void Work2StopCheck(void)
{
    if (SystemParam->DemoFunction > 0)
    {
        bRunning1 = 0;
        bRunning2 = 0;
        bRunning3 = 0;
        if (KeyPress3Flag > 0)
        {
            bRunning3 = 1;
        }
        else if (KeyPress1Flag > 0)
        {
            bRunning1 = 1;
        }
        else
        {
            bRunning2 = 1;
        }
    }
    else
    {
        bRunning2 = 0;
    }

    if (bRunning3)
    {
        WorkingFlag = 2;
    }
    else if (bRunning1)
    {
        WorkingFlag = 0;
    }
    else
    {
        bCuiQi = 0;
        WorkingFlag = 0;
        MoveAction_Pulse(X_AXIS, TempDist.Work2Dist, SpeedParam->XIdleSpeed);
    }
}

void Work3StopCheck(void)
{
    if (SystemParam->DemoFunction > 0)
    {
        bRunning1 = 0;
        bRunning2 = 0;
        bRunning3 = 0;
        if (KeyPress1Flag > 0)
        {
            bRunning1 = 1;
        }
        else if (KeyPress2Flag > 0)
        {
            bRunning2 = 1;
        }
        else
        {
            bRunning3 = 1;
        }
    }
    else
    {
        bRunning3 = 0;
    }

    if (bRunning1)
    {
        WorkingFlag = 0;
    }
    else if (bRunning2)
    {
        WorkingFlag = 1;
    }
    else
    {
        bCuiQi = 0;
        WorkingFlag = 0;
        MoveAction_Pulse(X_AXIS, TempDist.Work3Dist, SpeedParam->XIdleSpeed);
    }
}

/**
 * 深度计算，全局变量当前深度
 * 
 * @author xt (2019-03-29)
 * 
 * @param depth 需要加工的深度
 * @param picdepth 每次进刀深度
 * 
 * @return char 深度走完 返回1
 */
char DepthCal(unsigned short depth, unsigned short picdepth)
{
    char flag = 0;
    unsigned short tmpdepth = 0;

    if (depth > CurDepth)
    {
        tmpdepth = depth;
        tmpdepth -= CurDepth;
        if (tmpdepth > picdepth)
        {
            CurDepth += picdepth;
        }
        else
        {
            CurDepth += tmpdepth;
        }
    }
    else
    {
        CurDepth = 0;
        flag = 1;
    }

    return flag;
}

void MotorCoilSelect(void)
{
    if (MotorCoilSelectStep == 1)
    {
        if (bInvertStart)
        {
            MotorCoilSelectStep = 10;
        }
        else
        {
            MotorCoilSelectStep = 2;
        }
    }
    else if (MotorCoilSelectStep == 10)
    {
        if (GETBIT(247))
        {
            MotorCoilSelectStep = 11;
        }
        else if (GETBIT(245))
        {
            MotorCoilSelectStep = 12;
        }
    }
    else if (MotorCoilSelectStep == 11)
    {
        if (cToolOutFlag == TOOLSELECT_L)
        {
            MotorCoilSelectStep = 4;
        }
        else if (cToolOutFlag == TOOLSELECT_R)
        {
            DrillMotorStopStep = 1;
            MotorCoilSelectStep = 2;
        }
    }
    else if (MotorCoilSelectStep == 12)
    {
        if (cToolOutFlag == TOOLSELECT_L)
        {
            DrillMotorStopStep = 1;
            MotorCoilSelectStep = 2;
        }
        else if (cToolOutFlag == TOOLSELECT_R)
        {
            MotorCoilSelectStep = 4;
        }
    }
    else if (MotorCoilSelectStep == 2)
    {
        if (DrillMotorStopStep == 0)
        {
            MotorCoilSelectStep = 3;
        }
    }
    else if (MotorCoilSelectStep == 3)
    {
        if (cToolOutFlag == TOOLSELECT_L)
        {
            bLeftToolMotor = 1;

            MotorCoilSelectStep = 4;
            MotorCoilSelectDelay = 50;
            MotorCoilSelectDelay += dwTickCount;

            bLeftToolValve = 1;
            bRightToolValve = 0;
            ToolValveOutDelay = 100UL * UserParam->ToolValveDelay;
            ToolValveOutDelay += dwTickCount;
        }
        else if (cToolOutFlag == TOOLSELECT_R)
        {
            bRightToolMotor = 1;

            MotorCoilSelectStep = 4;
            MotorCoilSelectDelay = 50;
            MotorCoilSelectDelay += dwTickCount;

            bLeftToolValve = 0;
            bRightToolValve = 1;
            ToolValveOutDelay = 100UL * UserParam->ToolValveDelay;
            ToolValveOutDelay += dwTickCount;
        }
    }
    else if (MotorCoilSelectStep == 4)
    {
        if (MotorCoilSelectDelay < dwTickCount)
        {
            MotorCoilSelectStep = 5;
        }
    }
    else if (MotorCoilSelectStep == 5)
    {
        if (!bInvertStart)
        {
            bInvertStart = 1;
            DelayTime2 = 100UL * FactoryParam->RightMotorStartDelay;
            DelayTime2 += dwTickCount;

        }
        MotorCoilSelectStep = 6;
    }
    else if (MotorCoilSelectStep == 6)
    {
        MotorCoilSelectStep = 0;

    }
}

void WorkOver(void)
{
    bRunning = 0;
    bRunning1 = 0;
    bRunning2 = 0;
    bRunning3 = 0;
    bPress1_Vavle = 0;
    bPress2_Vavle = 0;
    bPress3_Vavle = 0;

    bCuiQi = 0;
    bRightToolMotor = 0;
}

//ɨ���жϳ�ʼ��
void VariableInit_Int(void)
{
    memset(HoleinfoF, 0, sizeof(HoleinfoF));
    memset(HoleinfoB, 0, sizeof(HoleinfoB));
    CheckHoleNum[0] = 0;
    CheckHoleNum[1] = 0;
    StartFlag = 0;
    flgSgn = 0;
    flgSgnOld = 0;
}

void VariableInit_Hole(void)
{
    uint8 i = 0;
    long depthcheck = 0;

    memset(DingweiDist, 0, sizeof(DingweiDist));
    memset(DingweiWitch, 0, sizeof(DingweiWitch));
    memset(DingweiDepth, 0, sizeof(DingweiDepth));

    depthcheck = UserParam->YMaxLength_Left;
    depthcheck -= UserParam->YOrignDist_Left;

    NeedWorkNum = 0;
    for (i = 0; i < 20; i++)
    {
        if (DingData->Orign[i] == 0 && DingData->Depth[i] == 0)
        {
            continue;
        }
        if (DingData->Length[i] > 0)
        {
            continue;
        }

        DingweiDist[NeedWorkNum] = DingData->Orign[i];
        if (depthcheck < DingData->Depth[i])
        {
            DingData->Depth[i] = depthcheck - 1;
        }
        DingweiDepth[NeedWorkNum] = DingData->Depth[i];

        NeedWorkNum++;
    }

    for (i = 0; i < NeedWorkNum; i++)
    {
        DingweiDist[i] += UserParam->Hole_LeftToolOffSet;
    }

    if (NeedWorkNum > 0)
    {
        DingWorkFlag = 2;
    }
}

void VariableInit_Slot(void)
{
    uint8 i = 0;
    //uint8 j = 0;
    long depthcheck = 0;
    long tooloffset;

    memset(DingWeiCao, 0, sizeof(DingWeiCao));
    memset(DingCaoWitch, 0, sizeof(DingCaoWitch));
    memset(DingCaoDepth, 0, sizeof(DingCaoDepth));

    depthcheck = SystemParam->YMaxLength;
    depthcheck -= FactoryParam->YOrignDist;

    if (SystemParam->SystemMode > SYSTEM_TYPE1)
    { // 双刀具
        tooloffset = FactoryParam->DrillOffset_Hole;
    }
    else
    { // 单刀具
        tooloffset = UserParam->Hole_LeftToolOffSet;
    }

    NeedWorkNum = 0;
    for (i = 0; i < 20; i++)
    {
        if (DingData->Orign[i] == 0 && DingData->Depth[i] == 0)
        {
            continue;
        }
        if (DingData->Length[i] == 0)
        {
            continue;
        }
        DingWeiCao[NeedWorkNum] = DingData->Orign[i];
        DingCaoWitch[NeedWorkNum] = DingData->Length[i];

        if (depthcheck < DingData->Depth[i])
        {
            DingData->Depth[i] = depthcheck - 1;
        }
        DingCaoDepth[NeedWorkNum] = DingData->Depth[i];
        NeedWorkNum++;
    }

    for (i = 0; i < NeedWorkNum; i++)
    {
        DingWeiCao[i] += tooloffset;
        DingWeiCao[i] = PositionToPluse(X_AXIS, DingWeiCao[i]);
        DingCaoWitch[i] = PositionToPluse(X_AXIS, DingCaoWitch[i]);

        if (SystemParam->SystemMode == SYSTEM_TYPE0)
        {
            DingCaoDepth[i] = FactoryParam->CaoPicDepth;
        }
    }

    if (NeedWorkNum > 0)
    {
        DingWorkFlag = 1;
    }
}
//WorkData_Code_str WorkData_Code;
// ������Ҫ�ӹ����ļ�������ȡ�ӹ�����
void VariableInit_Code(void)
{
    uint16 i = 0;
    long depthcheck = 0;

    CodeNumber = 0;

    for (i = 0; i < CODEARRAY; i++)
    {
        if (FactoryParam->WorkToolNumber == TOOLNUM_2)
        { // 来自参数
            if (bCodeWorkDataPos)
            {
                ScanCodeWorkData->Orign[i] = DingData->Orign[i];
            }

            if (ScanCodeWorkData->Orign[i] > 0)
            {
                CodeNumber++;
                ScanCodeWorkData->Depth[i] = FactoryParam->CodeSlotDepth;
                ScanCodeWorkData->Length[i] = FactoryParam->CodeSlotLength;
            }
        }
        else
        { // 单刀具  右刀工作
            if (bCodeWorkDataPos)
            {
                ScanCodeWorkData->Orign[i] = DingData->Orign[i];
            }

            if (FactoryParam->WorkDataSource == WORKDATASOURCE_FILE)
            { // 来自文件
                if (ScanCodeWorkData->Length[i] > 0)
                {
                    depthcheck = SystemParam->YMaxLength;
                    depthcheck -= FactoryParam->YOrignDist;
                }
                else
                {
                    depthcheck = UserParam->YMaxLength_Left;
                    depthcheck -= UserParam->YOrignDist_Left;
                }

                if (ScanCodeWorkData->Depth[i] > 0 && ScanCodeWorkData->Orign[i] > 0)
                {
                    if (depthcheck < ScanCodeWorkData->Depth[i])
                    {
                        ScanCodeWorkData->Depth[i] = depthcheck - 1;
                    }
                    CodeNumber++;
                }
            }
            else
            {
                if (bOneToolMode_Hole)
                { // 左刀打孔
                    if (ScanCodeWorkData->Orign[i] > 0)
                    {
                        ScanCodeWorkData->Length[i] = 0;
                        ScanCodeWorkData->Depth[i] = UserParam->HoleDepth_Left;
                        CodeNumber++;
                    }
                }
                if (bOneToolMode_Slot)
                { // 右刀铣槽
                    if (ScanCodeWorkData->Orign[i] > 0)
                    {
                        ScanCodeWorkData->Length[i] = FactoryParam->CodeSlotLength;
                        ScanCodeWorkData->Depth[i] = FactoryParam->CodeSlotDepth;
                        CodeNumber++;
                    }
                }
            }
        }
    }

    for (i = 0; i < CODEARRAY; i++)
    {
        CodeWorkOrign[i] = ScanCodeWorkData->Orign[i];
        CodeWorkLength[i] = ScanCodeWorkData->Length[i];
        CodeWorkDepth[i] = ScanCodeWorkData->Depth[i];
    }

    if (CodeNumber == 0)
    {
        bAlarmFlag = 1;
        UserCurrentParam->cAlarmNum = 19;
    }
}

static void RunInit(void)
{
    memset(infos, 0, sizeof(infos));
    memset(Winfos, 0, sizeof(Winfos));
    VariableInit_Int();

    //��λ��
    if (FactoryParam->WorkMode == WorkMode_Hole)
    {
        DingWorkFlag = 0;
        switch (SystemParam->SystemMode)
        {
        case SYSTEM_TYPE0:
        case SYSTEM_TYPE1:
        case SYSTEM_TYPE2:
            if (bDingMode_ST)
            {
                DingWorkFlag = 0;
                VariableInit_Slot();
            }
            else
            {
                DingWorkFlag = 0;
                VariableInit_Hole();
            }
            break;
        case SYSTEM_TYPE3:
        case SYSTEM_TYPE4:
            VariableInit_Slot();

            if (DingWorkFlag == 0)
            {
                VariableInit_Hole();
            }
            break;
        }
    }
    else if (FactoryParam->WorkMode == WorkMode_Code)
    {
        VariableInit_Code();
    }

    currentHoleNum = 0;
    currentWorkHoleNum = 0;
    CurWoodLength = 0;

    if (SystemParam->SystemMode != SYSTEM_TYPE0)
    {
        MoveAction_Pulse(Y_AXIS, 0, SpeedParam->YIdleSpeed);
    }
    else
    {
        bDrillValve = 0;
    }
}

static unsigned short RunCheckFlag = 0;
static uint8 RunCheck(void)
{
    uint8 flag = 0;
    //static uint8 flagold = 0;
    static uint32 delay = 0;

    if (bRunning1)
    {
        if (Press1time < dwTickCount)
        {
            if (RunCheckFlag == 0)
            {
                bCuiQi = 1;
                WorkingFlag = 0;
                RunCheckFlag = 1;
                MoveAction_Pulse(X_AXIS, TempDist.Work1Dist, SpeedParam->XIdleSpeed);
            }
        }
    }
    else if (bRunning2)
    {
        if (Press2time < dwTickCount)
        {
            if (RunCheckFlag == 0)
            {
                bCuiQi = 1;
                WorkingFlag = 1;
                RunCheckFlag = 1;
                MoveAction_Pulse(X_AXIS, TempDist.Work2Dist, SpeedParam->XIdleSpeed);
            }
        }
    }
    else if (bRunning3)
    {
        if (Press3time < dwTickCount)
        {
            if (RunCheckFlag == 0)
            {
                bCuiQi = 1;
                WorkingFlag = 2;
                RunCheckFlag = 1;
                MoveAction_Pulse(X_AXIS, TempDist.Work3Dist, SpeedParam->XIdleSpeed);
            }
        }
    }
    else
    {
        //if (NoworkStopdelay == 0)
        if (NoworkStopdelay < dwTickCount) M43 = 1;
    }

    if (RunCheckFlag > 0)
    {
        if (!X_DRV)
        {
            if (delay < dwTickCount)
            {
                RunCheckFlag = 0;
                flag = 1;
            }
        }
        else
        {
            delay = dwTickCount + 500;
        }
    }

    return flag;
}
/**
 * @brief  和当前位置比，哪个更近点
 * @note   
 * @param  cur: 当前位置
 * @param  start: 起始位置
 * @param  end: 结束位置
 * @retval 返回值 0 离起始位置更近  1 离结束位置更近
 */
char CheckNearPositon(long cur, long start, long end)
{
    if (cur < end)
    {
        end -= -cur;
    }
    else
    {
        end = cur - end;
    }

    if (cur < start)
    {
        start -= cur;
    }
    else
    {
        start = cur - start;
    }

    if (end > start)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void TongCao2ScanProc(void)
{
    long temp = 0;
    static unsigned long _lxstopdelay = 0;

    if (cTongCao2ScanStep == 1)
    {
        if (flgSgn > 0)
        { // 检测到板头
            BoardHead = HoleinfoB[0] / XEncodeDivide;
            cTongCao2ScanStep = 2;
        }
    }
    else if (cTongCao2ScanStep == 2)
    { // ����ֻ��Ҫ����β
        uint8 flag = 0;

        if (flgSgn < 1)
        {
            temp = ulEncoderCount;                  //
            temp -= HoleinfoF[CheckHoleNum[1] - 1]; // / XEncodeDivide;
            temp /= XEncodeDivide;

            BoardEnd = HoleinfoF[CheckHoleNum[1] - 1];
            BoardEnd /= XEncodeDivide;

            if (temp > 0)
            {
                if (temp > TempDist.MaxHoleR)
                {
                    cTongCao2ScanStep = 3;
                    StartFlag = 0;
                    MV_Dec_Stop(X_AXIS);
                }
                else
                {
                    flag = 1;
                }
            }
        }
        else
        {
            flag = 1;
        }

        if (flag > 0)
        {
            if (!X_DRV)
            {
                if (_lxstopdelay < dwTickCount)
                {
                    AlarmNum = 17;
                    bAlarmFlag = 1;
                }
            }
            else
            {
                _lxstopdelay = 10 * FactoryParam->XBackDelayTime;
                _lxstopdelay += dwTickCount;
            }
        }

        temp = SystemParam->XMaxLength;
        temp -= FactoryParam->PreDecDistance;
        if (SysCurrentParam->XDistance > temp)
        {
            temp = PositionToPluse(X_AXIS, SystemParam->XMaxLength);
            MV_Set_Dec(X_AXIS, 165);
            MV_Move_To_Position(X_AXIS, temp);
            cTongCao2ScanStep = 4;
        }
    }
    else if (cTongCao2ScanStep == 3)
    {
        if (!X_DRV)
        { // ����ɨ�����
            cTongCao2ScanStep = 0;
        }
    }
    else if (cTongCao2ScanStep == 4)
    {
        if (!X_DRV)
        {
            _lxstopdelay = dwTickCount + 100;
            cTongCao2ScanStep = 5;
        }
    }
    else if (cTongCao2ScanStep == 5)
    {
        if (_lxstopdelay < dwTickCount)
        {
            StartFlag = 0;
            cTongCao2ScanStep = 0;
            BoardEnd = ulEncoderCount / XEncodeDivide;
        }
    }
}
SlotWorkData_str SlotWorkData;
//头尾定长槽
void TWSlotWorkProc(void)
{
    //static unsigned long delaytick = 0;

    long OverLcttmp = 0;
    long start = 0;
    long end = 0;
    long depth = 0;
    long xoffset = 0;
    long yoffset = 0;

    switch (cTongCao2RunStep)
    {
    case 1:
        XWorkOrignPos = MV_Get_Command_Pos(X_AXIS);
        ulEncoderCount = XWorkOrignPos * XEncodeDivide;
        SlotTypeFlag = 0;
        cTongCao2RunStep = 199;
        break;
    case 199:
        VariableInit_Int();
        StartFlag = 1;

        MV_Set_Startv(X_AXIS, 3);
        MV_AccDec_Set(X_AXIS, 165, 100); //165);
        MV_Const_Move(X_AXIS, SpeedParam->ScanSpeed2, XCW);
        cTongCao2RunStep = 200;
        cTongCao2ScanStep = 1;

        break;
    case 200:
        TongCao2ScanProc();

        if (cTongCao2ScanStep == 0)
        { // ɨ�����
            StartFlag = 0;
            flgSgn = 0;
            flgSgnOld = 0;

            MV_Dec_Stop(X_AXIS);
            bCuiQi = 0;

            cTongCao2RunStep = 201;
        }
        break;
    case 201:
        cTongCaoStartFlag = 0;
        if (GETBIT(DISP_T_SLOT))
        { // 桶槽
            cTongCaoStartFlag |= 0x01;
        }

        if (GETBIT(DISP_TW_SLOT_ONE))
        { // 头尾定长单槽
            cTongCaoStartFlag |= 0x02;
        }

        if (GETBIT(DISP_TW_SLOT))
        {
            cTongCaoStartFlag |= 0x04;
        }

        if (GETBIT(DISP_TW_HOLE))
        {
            cTongCaoStartFlag |= 0x08;
        }

        cTongCao2RunStep = 202;
        break;
    case 202:
        if (SystemParam->SystemMode == SYSTEM_TYPE3)
        {
            MotorCoilSelectStep = 1;
        }
        else
        {
            MotorCoilSelectStep = 0;
        }

        if (cTongCaoStartFlag & 0x01)
        { // 桶槽
            if (GETBIT(DISP_T_SLOT_TOOL))
            { // 右刀
                cToolOutFlag = TOOLSELECT_R;
            }
            else
            { // 左刀
                cToolOutFlag = TOOLSELECT_L;
            }

            memset(&SlotWorkData, 0, sizeof(SlotWorkData));
            cTongCao2RunStep = 2;
        }
        else if (cTongCaoStartFlag & 0x02)
        { // 头尾定长操
            cToolOutFlag = TOOLSELECT_R;
            memset(&SlotWorkData, 0, sizeof(SlotWorkData));
            cTongCao2RunStep = 5;
        }
        else if (cTongCaoStartFlag & 0x04)
        { // 头尾定长段操
            cToolOutFlag = TOOLSELECT_R;
            memset(&SlotWorkData, 0, sizeof(SlotWorkData));
            cTongCao2RunStep = 8;
        }
        else if (cTongCaoStartFlag & 0x08)
        { // 头尾定长孔
            cToolOutFlag = TOOLSELECT_L;
            memset(&SlotWorkData, 0, sizeof(SlotWorkData));
            cTongCao2RunStep = 11;
        }
        else
        {
            cTongCaoStartFlag = 0;
            cTongCao2RunStep = 0;
        }
        break;
    case 2:
        ToolOutProc();
        if (MotorCoilSelectStep == 0)
        {
            cTongCao2RunStep = 3;
        }
        break;
    case 3:
        if (SystemParam->SystemMode < SYSTEM_TYPE2)
        {
            xoffset = UserParam->Slot_LeftToolOffSet;
            yoffset = UserParam->YOrignDist_Left;
        }
        else if (SystemParam->SystemMode == SYSTEM_TYPE2)
        {
            xoffset = FactoryParam->DrillOffset_Cao;
            yoffset = FactoryParam->YOrignDist;
        }
        else
        {
            if (GETBIT(DISP_T_SLOT_TOOL))
            { // 右刀
                xoffset = FactoryParam->DrillOffset_Cao;
                yoffset = FactoryParam->YOrignDist;
            }
            else
            { // 左刀
                xoffset = UserParam->Slot_LeftToolOffSet;
                yoffset = UserParam->YOrignDist_Left;
            }
        }

        depth = FactoryParam->TongCaoDepth;

        OverLcttmp = xoffset;
        OverLcttmp += FactoryParam->DrillRaidus;
        OverLcttmp += 500;
        OverLcttmp -= FactoryParam->TongCao2_EndOffset;
        OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
        OverLcttmp += BoardEnd;
        SlotWorkData.xstart = OverLcttmp;
        // ��β�Ϳ�ʼϳ
        OverLcttmp = xoffset;
        OverLcttmp -= FactoryParam->DrillRaidus;
        OverLcttmp -= 500;
        OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
        OverLcttmp += BoardHead;
        SlotWorkData.xstop = OverLcttmp;

        SlotWorkData.ydepth = depth;
        SlotWorkData.yorign = yoffset;

        cTongCao2RunStep = 4;
        cDrillRunStep = 1;
        MoveAction_Pulse(X_AXIS, SlotWorkData.xstart, SpeedParam->XMoveSpeed);
        break;
    case 4:
        DrillingSlotProc(&SlotWorkData);
        if (cDrillRunStep == 0)
        {
            cTongCaoStartFlag &= ~0x01;
            cTongCao2RunStep = 202;
        }
        break;
    case 5: // 头尾定长单槽 右刀  // 右刀
        ToolOutProc();
        if (MotorCoilSelectStep == 0)
        {
            cTongCao2RunStep = 6;
        }
        break;
    case 6:
        if (SystemParam->SystemMode < SYSTEM_TYPE2)
        {
            xoffset = UserParam->Slot_LeftToolOffSet;
            yoffset = UserParam->YOrignDist_Left;
        }
        else
        {
            xoffset = FactoryParam->DrillOffset_Cao;
            yoffset = FactoryParam->YOrignDist;
        }

        depth = UserParam->TW_SlotOneDepth;

        OverLcttmp = xoffset;
        OverLcttmp -= FactoryParam->TongCao2_EndOffset;
        OverLcttmp -= FactoryParam->TongCao2_EndDist;

        OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
        OverLcttmp += BoardEnd;
        end = OverLcttmp;

        OverLcttmp = xoffset;
        OverLcttmp += FactoryParam->TongCao2_HeadDist;
        OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
        OverLcttmp += BoardHead;
        start = OverLcttmp;

        OverLcttmp = MV_Get_Command_Pos(X_AXIS);
        if (CheckNearPositon(OverLcttmp, start, end) > 0)
        {
            SlotWorkData.xstart = end;
            SlotWorkData.xstop = start;
        }
        else
        {
            SlotWorkData.xstart = start;
            SlotWorkData.xstop = end;
        }

        SlotWorkData.ydepth = depth;
        SlotWorkData.yorign = yoffset;

        cTongCao2RunStep = 7;
        cDrillRunStep = 1;
        MoveAction_Pulse(X_AXIS, SlotWorkData.xstart, SpeedParam->XMoveSpeed);
        break;
    case 7:
        DrillingSlotProc(&SlotWorkData);
        if (cDrillRunStep == 0)
        {
            cTongCaoStartFlag &= ~0x02;
            cTongCao2RunStep = 202;
        }
        break;
    case 8: // 头尾定长段槽 右刀
        ToolOutProc();
        if (MotorCoilSelectStep > 0)
        {
            return;
        }

        if (SystemParam->SystemMode < SYSTEM_TYPE2)
        {
            xoffset = UserParam->Slot_LeftToolOffSet;
            yoffset = UserParam->YOrignDist_Left;
        }
        else
        {
            xoffset = FactoryParam->DrillOffset_Cao;
            yoffset = FactoryParam->YOrignDist;
        }

        depth = UserParam->TW_SlotDepth;

        OverLcttmp = xoffset;
        OverLcttmp -= FactoryParam->TongCao2_EndOffset;
        OverLcttmp -= FactoryParam->TongCao2_EndDist;
        OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
        OverLcttmp += BoardEnd;
        end = OverLcttmp;

        OverLcttmp = xoffset;
        OverLcttmp += FactoryParam->TongCao2_HeadDist;
        OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
        OverLcttmp += BoardHead;
        start = OverLcttmp;

        memset(DingweiDist, 0, sizeof(DingweiDist));
        memset(DingweiWitch, 0, sizeof(DingweiWitch));
        memset(DingweiDepth, 0, sizeof(DingweiDepth));
        OverLcttmp = MV_Get_Command_Pos(X_AXIS);
        NeedWorkNum = 2;
        if (CheckNearPositon(OverLcttmp, start, end) > 0)
        {
            if (UserParam->ScanSlotType)
            {
                SlotTypeFlag = 1;
            }
            OverLcttmp = UserParam->TW_SlotLen;
            OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);

            DingweiDist[0] = end;
            DingweiWitch[0] = DingweiDist[0];
            DingweiWitch[0] -= OverLcttmp;
            DingweiDepth[0] = depth;

            DingweiWitch[1] = start;
            DingweiDist[1] = DingweiWitch[1];
            DingweiDist[1] += OverLcttmp;
            DingweiDepth[1] = depth;
        }
        else
        { // 从左往右
            if (UserParam->ScanSlotType)
            {
                SlotTypeFlag = 2;
            }
            OverLcttmp = UserParam->TW_SlotLen;
            OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);

            DingweiDist[0] = start;
            DingweiWitch[0] = DingweiDist[0];
            DingweiWitch[0] += OverLcttmp;
            DingweiDepth[0] = depth;

            DingweiWitch[1] = end;
            DingweiDist[1] = DingweiWitch[1];
            DingweiDist[1] -= OverLcttmp;
            DingweiDepth[1] = depth;
        }

        if (GETBIT(DISP_TW_SLOT_M))
        { // 算中间槽起点
            NeedWorkNum = 3;
            DingweiDist[2] = DingweiDist[1];
            DingweiWitch[2] = DingweiWitch[1];
            DingweiDepth[2] = DingweiDepth[1];

            DingweiDist[1] += DingweiDist[0];
            DingweiDist[1] /= 2;
            DingweiWitch[1] += DingweiWitch[0];
            DingweiWitch[1] /= 2;
            DingweiDepth[1] = depth;
        }
        currentHoleNum = NeedWorkNum;
        currentWorkHoleNum = 0;
        cTongCao2RunStep = 9;

        SlotWorkData.yorign = yoffset;
        break;
    case 9:
        if (currentHoleNum <= currentWorkHoleNum)
        {
            cTongCaoStartFlag &= ~0x04;
            cTongCao2RunStep = 202;
            break;
        }

        SlotWorkData.xstart = DingweiDist[currentWorkHoleNum];
        SlotWorkData.xstop = DingweiWitch[currentWorkHoleNum];
        SlotWorkData.ydepth = DingweiDepth[currentWorkHoleNum];

        cDrillRunStep = 1;
        //MoveAction_Pulse(X_AXIS, SlotWorkData.xstart, SpeedParam->XMoveSpeed);
        cTongCao2RunStep = 10;

        break;
    case 10:
        DrillingSlotProc(&SlotWorkData);
        if (cDrillRunStep == 0)
        {
            currentWorkHoleNum++;
            cTongCao2RunStep = 9;
        }
        break;
    case 11:
        ToolOutProc();
        if (MotorCoilSelectStep > 0)
        {
            return;
        }

        if (SystemParam->SystemMode == SYSTEM_TYPE2)
        {
            xoffset = FactoryParam->DrillOffset_Cao;
            yoffset = FactoryParam->YOrignDist;
        }
        else
        {
            xoffset = UserParam->Slot_LeftToolOffSet;
            yoffset = UserParam->YOrignDist_Left;
        }

        depth = UserParam->TW_HoleDepth;

        OverLcttmp = xoffset;
        OverLcttmp -= FactoryParam->TongCao2_EndOffset;
        OverLcttmp -= FactoryParam->TongCao2_EndDist;
        OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
        OverLcttmp += BoardEnd;
        end = OverLcttmp;

        OverLcttmp = xoffset;
        OverLcttmp += FactoryParam->TongCao2_HeadDist;
        OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
        OverLcttmp += BoardHead;
        start = OverLcttmp;

        memset(DingweiDist, 0, sizeof(DingweiDist));
        memset(DingweiWitch, 0, sizeof(DingweiWitch));
        memset(DingweiDepth, 0, sizeof(DingweiDepth));
        OverLcttmp = MV_Get_Command_Pos(X_AXIS);
        NeedWorkNum = 0;
        if (CheckNearPositon(OverLcttmp, start, end) > 0)
        { // 从右往左加工
            OverLcttmp = UserParam->TW_HoleDist;
            OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);

            DingweiDist[NeedWorkNum] = end;
            DingweiDepth[NeedWorkNum] = depth;
            NeedWorkNum++;

            if (GETBIT(DISP_TW_HOLE_X))
            {
                DingweiDist[NeedWorkNum] = DingweiDist[0];
                DingweiDist[NeedWorkNum] -= OverLcttmp;
                DingweiDepth[NeedWorkNum] = depth;
                NeedWorkNum++;

                DingweiDist[NeedWorkNum] = start;
                DingweiDist[NeedWorkNum] += OverLcttmp;
                DingweiDepth[NeedWorkNum] = depth;
                NeedWorkNum++;
            }
            DingweiDist[NeedWorkNum] = start;
            DingweiDepth[NeedWorkNum] = depth;
            NeedWorkNum++;
        }
        else
        { // 从左往右
            OverLcttmp = UserParam->TW_HoleDist;
            OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);

            DingweiDist[NeedWorkNum] = start;
            DingweiDepth[NeedWorkNum] = depth;
            NeedWorkNum++;
            if (GETBIT(DISP_TW_HOLE_X))
            {
                DingweiDist[NeedWorkNum] = start;
                DingweiDist[NeedWorkNum] += OverLcttmp;
                DingweiDepth[NeedWorkNum] = depth;
                NeedWorkNum++;

                DingweiDist[NeedWorkNum] = end;
                DingweiDist[NeedWorkNum] -= OverLcttmp;
                DingweiDepth[NeedWorkNum] = depth;
                NeedWorkNum++;
            }
            DingweiDist[NeedWorkNum] = end;
            DingweiDepth[NeedWorkNum] = depth;
            NeedWorkNum++;
        }

        if (GETBIT(DISP_TW_HOLE_M))
        {
            if (GETBIT(DISP_TW_HOLE_X))
            { // 分别取中间值
                DingweiDist[NeedWorkNum] = DingweiDist[NeedWorkNum - 2];
                DingweiDepth[NeedWorkNum] = DingweiDepth[NeedWorkNum - 2];

                DingweiDist[NeedWorkNum - 2] += DingweiDist[NeedWorkNum - 4];
                DingweiDist[NeedWorkNum - 2] /= 2;
                NeedWorkNum++;

                DingweiDist[NeedWorkNum] = DingweiDist[NeedWorkNum - 2];
                DingweiDepth[NeedWorkNum] = DingweiDepth[NeedWorkNum - 2];

                DingweiDist[NeedWorkNum - 2] += DingweiDist[NeedWorkNum - 4];
                DingweiDist[NeedWorkNum - 2] /= 2;
                NeedWorkNum++;
            }
            else
            {
                DingweiDist[NeedWorkNum] = DingweiDist[NeedWorkNum - 1];
                DingweiDepth[NeedWorkNum] = DingweiDepth[NeedWorkNum - 1];

                DingweiDist[NeedWorkNum - 1] += DingweiDist[NeedWorkNum - 2];
                DingweiDist[NeedWorkNum - 1] /= 2;
                NeedWorkNum++;
            }
        }

        currentHoleNum = NeedWorkNum;
        currentWorkHoleNum = 0;
        cTongCao2RunStep = 12;

        SlotWorkData.yorign = yoffset;
        break;
    case 12:
        if (currentHoleNum <= currentWorkHoleNum)
        {
            cTongCaoStartFlag &= ~0x08;
            cTongCao2RunStep = 202;
            break;
        }

        SlotWorkData.xstart = DingweiDist[currentWorkHoleNum];
        SlotWorkData.ydepth = DingweiDepth[currentWorkHoleNum];

        MoveAction_Pulse(X_AXIS, SlotWorkData.xstart, SpeedParam->XMoveSpeed);
        cTongCao2RunStep = 13;
        HoleCurrrentDrillDepth = 0;
        break;
    case 13:
        cDrillRunStep = 1;
        HoleDrillDepthProcess(SlotWorkData.ydepth);
        cTongCao2RunStep = 14;
        break;
    case 14:
        DrillingHoleProc(SlotWorkData.yorign, HoleCurrrentDrillDepth);
        if (cDrillRunStep == 0)
        {
            if (HoleCurrrentDrillDepth >= SlotWorkData.ydepth)
            {
                currentWorkHoleNum++;
                cTongCao2RunStep = 12;
            }
            else
            {
                cTongCao2RunStep = 13;
            }
        }
        break;
    default:
        break;
    }
}

static unsigned short duancaodirflag = 0;

void DuanCaoXMoveAction(long xorign, long xend)
{
    long OverLcttmp = 0;

    if (cCaoMoveDir < 1)
    { // 从 xorign 走向 xend
        cCaoMoveDir = 1;
        OverLcttmp = xend;

        if (duancaodirflag == 1)
        {
            OverLcttmp += PositionToPluse(X_AXIS, FactoryParam->SlowMotorBuchang);
            duancaodirflag = 2; // ����
        }
    }
    else
    { // xend 走向 xorign
        cCaoMoveDir = 0;
        OverLcttmp = xorign;
        OverLcttmp -= PositionToPluse(X_AXIS, FactoryParam->SlowMotorBuchang);
        duancaodirflag = 1;
    }

    //OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
    MoveAction_Pulse2(X_AXIS, OverLcttmp, SpeedParam->XSlotSpeed);
}

// �ϵ�ϳ�ϲ�
void DuanCaoWorkPro(void)
{
    long OverLcttmp;
    //static unsigned char firstflag = 0;
    static unsigned long delaytick = 0;
    static unsigned long _lqigangfdelay = 0;
    static unsigned long _lalldepth = 0;

    if (cDuanCaoRunStep == 1)
    {
        if (DelayTime2 < dwTickCount)
        {
            delaytick = 0;
            _lalldepth = 0;
            cCaoMoveDir = 0;
            duancaodirflag = 0;
            cDuanCaoRunStep = 2;
            //      firstflag = 1;
        }
    }
    else if (cDuanCaoRunStep == 2)
    { // �ȴ�ɨ����ͷ
        if (currentWorkHoleNum < currentHoleNum)
        {
            cDuanCaoRunStep = 20;
            bCuiQi = 0;
            delaytick = dwTickCount + 50; // ������ʱ50ms
            MV_Dec_Stop(X_AXIS);
        }
    }
    else if (cDuanCaoRunStep == 20)
    {
        if (X_DRV)
        {
            delaytick = dwTickCount + 100;
            return;
        }

        if (delaytick < dwTickCount)
        {
            cDuanCaoRunStep = 3;
        }
    }
    else if (cDuanCaoRunStep == 3)
    { // �ȴ�X��ֹͣ
        uint8 flag = 0;

        if (DelayTime2 > dwTickCount)
        {
            return;
        }

        if (SystemParam->SystemMode != SYSTEM_TYPE0)
        {
            if (Y_Orign && !(Y_DRV)) flag = 1;
        }
        else
        {
            if (bQiGangBackSgn) flag = 1;
        }

        if (flag > 0)
        {
            cDuanCaoRunStep = 4;
        }
    }
    else if (cDuanCaoRunStep == 4)
    {
        OverLcttmp = BoardHead;
        OverLcttmp += DingWeiCao[currentWorkHoleNum];
        if (UserParam->DingSlotType)
        {
            OverLcttmp += DingCaoWitch[currentWorkHoleNum] / 2;
        }
        MoveAction_Pulse2(X_AXIS, OverLcttmp, SpeedParam->XMoveSpeed);
        cDuanCaoRunStep = 5;
    }
    else if (cDuanCaoRunStep == 5)
    {
        if (X_DRV)
        {
            delaytick = dwTickCount + 50;
        }

        if (delaytick < dwTickCount)
        {
            if (SystemParam->SystemMode > 0)
            { // ���ߵ����
                OverLcttmp = FactoryParam->YOrignDist;
                OverLcttmp = PositionToPluse(Y_AXIS, OverLcttmp);
                MoveAction_Pulse(Y_AXIS, OverLcttmp, SpeedParam->YIdleSpeed);
                CurDepth = 0;
                _lalldepth = DingCaoDepth[currentWorkHoleNum];

                //cDuanCaoRunStep = 6;
                cDuanCaoRunStep = 60;
            }
            else
            { // ���ŷ���ֱ�ӽ���
                bDrillValve = 1;
                _lalldepth = DingCaoDepth[currentWorkHoleNum];
                CurDepth = _lalldepth;
                cDuanCaoRunStep = 7;
            }
        }
    }
    else if (cDuanCaoRunStep == 60)
    {
        if (UserParam->DingSlotType)
        {
            cDuanCaoRunStep = 61;

        }
        else
        {
            cDuanCaoRunStep = 6;
        }
    }
    else if (cDuanCaoRunStep == 61)
    {
        if (Y_DRV)
        {
            return;
        }
        OverLcttmp = FactoryParam->YOrignDist;
        OverLcttmp += UserParam->KSlotPicDepth;
        OverLcttmp = PositionToPluse(Y_AXIS, OverLcttmp);
        MoveAction_Pulse(Y_AXIS, OverLcttmp, SpeedParam->YDrillSpeed);
        cDuanCaoRunStep = 62;
    }
    else if (cDuanCaoRunStep == 62)
    {
        if (Y_DRV)
        {
            delaytick = 10 * FactoryParam->DrillCycleTime;
            delaytick += dwTickCount;

            return;
        }

        if (delaytick < dwTickCount)
        {
            OverLcttmp = BoardHead;
            OverLcttmp += DingWeiCao[currentWorkHoleNum];
            MoveAction_Pulse(X_AXIS, OverLcttmp, SpeedParam->XSlotSpeed);
            cDuanCaoRunStep = 6;
        }
    }
    else if (cDuanCaoRunStep == 6)
    { // ˫˽�������
        if (Y_DRV && X_DRV)
        {
            return;
        }

        if (CurDepth < _lalldepth)
        { // ��ǰ���С�ڲ۵������
            OverLcttmp = _lalldepth - CurDepth;
            if (OverLcttmp > FactoryParam->CaoPicDepth)
            { // ��ʣ�����ȴ���ÿ�ν������
                CurDepth = CurDepth + FactoryParam->CaoPicDepth;
            }
            else
            { // ��ʣ������С�ڵ���ÿ�ν������
                CurDepth = CurDepth + OverLcttmp;
            }
            OverLcttmp = FactoryParam->YOrignDist + CurDepth;
            cDuanCaoRunStep = 7;
            OverLcttmp = PositionToPluse(Y_AXIS, OverLcttmp); //UserParam->TongCaoDepth);
            MoveAction_Pulse2(Y_AXIS, OverLcttmp, SpeedParam->YDrillSpeed);
        }
        else
        { // ��ǰ��ȴ��ڵ��ڲ۵������
          //OverLcttmp = 0;
          //MoveAction_Pulse2(Y_AXIS, OverLcttmp, SpeedParam->YIdleSpeed);
            cDuanCaoRunStep = 10;
        }
    }
    else if (cDuanCaoRunStep == 7)
    { //�ȴ���ȵ�λ
        uint8 flag = 0;

        if (SystemParam->SystemMode > 0)
        {
            if (!Y_DRV)
            {
                flag = 1;
            }
        }
        else
        {
            if (bQiGangComeSgn)
            {
                flag = 1;
            }
        }

        if (flag > 0)
        {
            if (_lqigangfdelay < dwTickCount)
            {
                cDuanCaoRunStep = 8;
            }
        }
        else
        {
            _lqigangfdelay = 10 * FactoryParam->DrillCycleTime;
            _lqigangfdelay += dwTickCount;
        }
    }
    else if (cDuanCaoRunStep == 8)
    {
        long xorign, xend;

        xorign = BoardHead;
        xorign += DingWeiCao[currentWorkHoleNum];
        xend = xorign;
        xend += DingCaoWitch[currentWorkHoleNum];
        DuanCaoXMoveAction(xorign, xend);
        cDuanCaoRunStep = 9;
    }
    else if (cDuanCaoRunStep == 9)
    { // X�� ��������
        if (!X_DRV)
        {
            if (delaytick < dwTickCount)
            {
                cDuanCaoRunStep = 90;
            }
        }
        else
        {
            delaytick = 10 * FactoryParam->XBackDelayTime;
            delaytick += dwTickCount;
        }
    }
    else if (cDuanCaoRunStep == 90)
    {
        if (SystemParam->SystemMode > 0)
        {
            cDuanCaoRunStep = 6;

            if (duancaodirflag == 1)
            {
                OverLcttmp = FactoryParam->SlowMotorBuchang;
                OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
                tXAxisStepper.cRealPosi += OverLcttmp;
            }
            else if (duancaodirflag == 2)
            {
                OverLcttmp = FactoryParam->SlowMotorBuchang;
                OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
                tXAxisStepper.cRealPosi -= OverLcttmp;
            }
        }
        else
        {
            cDuanCaoRunStep = 10;
        }
    }
    else if (cDuanCaoRunStep == 10)
    { //�˵�
        if (X_DRV || Y_DRV)
        {
            return;
        }
        if (UserParam->DingSlotType)
        {
            OverLcttmp = BoardHead;
            OverLcttmp += DingWeiCao[currentWorkHoleNum];
            MoveAction_Pulse2(X_AXIS, OverLcttmp, SpeedParam->XSlotSpeed);
        }
        cDuanCaoRunStep = 100;
    }
    else if (cDuanCaoRunStep == 100)
    {
        if (X_DRV || Y_DRV)
        {
            return;
        }
        OverLcttmp = 0;
        MoveAction_Pulse2(Y_AXIS, OverLcttmp, SpeedParam->YIdleSpeed);
        cDuanCaoRunStep = 101;
    }
    else if (cDuanCaoRunStep == 101)
    {
        if (X_DRV || Y_DRV)
        {
            return;
        }

        if (SystemParam->SystemMode > 0)
        {
            if (!Y_DRV)
            {
                cDuanCaoRunStep = 11;
            }
        }
        else
        {
            bDrillValve = 0;
            cDuanCaoRunStep = 11;
        }

    }
    else if (cDuanCaoRunStep == 11)
    { // �˵����
        if (SystemParam->SystemMode > 0)
        {
            if (Y_Orign)
            {
                cDuanCaoRunStep = 12;
            }
        }
        else
        {
            if (bQiGangBackSgn)
            {
                if (delaytick < dwTickCount)
                {
                    cDuanCaoRunStep = 12;
                }
            }
            else
            {
                delaytick = 10 * FactoryParam->QiGangReBackDelay;
                delaytick += dwTickCount;
            }
        }
    }
    else if (cDuanCaoRunStep == 12)
    {
        currentWorkHoleNum++;
        cCaoMoveDir = 0;
        if (currentWorkHoleNum < currentHoleNum)
        {
            cDuanCaoRunStep = 3;
        }
        else cDuanCaoRunStep = 0;
    }
}

void DingWorkScanProc(void)
{
    switch (cWorkScanStep)
    {
    case 1:
        if (FactoryParam->Hole_Probe > 0)
        {
            cWorkScanStep = 2;
        }
        else
        {
            cWorkScanStep = 3;
        }

        break;
    case 2:
        if (ToolValveOutDelay < dwTickCount)
        {
            VariableInit_Int();
            StartFlag = 1;
            MV_Set_Startv(X_AXIS, 3);
            MV_AccDec_Set(X_AXIS, 165, 100);
            MV_Const_Move(X_AXIS, SpeedParam->ScanSpeed2, XCW);

            cWorkScanStep = 4;
        }
        break;
    case 3:
        if (ToolValveOutDelay < dwTickCount)
        {
            //BoardHead = 0;
            BoardHead = XWorkOrignPos;
            currentHoleNum = NeedWorkNum;

            cWorkScanStep = 10;
        }
        break;
    case 4:
        if (flgSgn > 0)
        {
            long TempData = 0;

            MV_Dec_Stop(X_AXIS);
            cWorkScanStep = 10;
            TempData = HoleinfoB[0] / XEncodeDivide;
            BoardHead = TempData;
            currentHoleNum = NeedWorkNum;
            StartFlag = 0;
        }
        break;
    case 10:
        cWorkScanStep = 0;
        break;
    default:
        break;
    }
}
// 扫码 扫描班头
void CodeWorkScanProc(void)
{
    unsigned long temp = 0;

    if (cDingHoleScanStep == 1)
    {
        if (flgSgn > 0)
        {
            cDingHoleScanStep = 5;
            temp = HoleinfoB[0] / XEncodeDivide;
            BoardHead = temp;
            _gDingHoleDist = temp;
            currentHoleNum = CodeNumber;
        }
    }
    else if (cDingHoleScanStep == 2)
    {
        if (flgSgn == 0)
        {
            temp = HoleinfoF[CheckHoleNum[1] - 1];
            if (ulEncoderCount > temp)
            {
                long dist; // = PositionToPluse(X_AXIS, 8000);
                temp = HoleinfoF[CheckHoleNum[1] - 1];
                temp = ulEncoderCount - temp;
                temp /= XEncodeDivide;

                dist = PositionToPluse(X_AXIS, 8000);
                //if (temp > TempDist.MaxHoleR)
                if (temp > dist)
                {
                    StartFlag = 0;
                    MV_Dec_Stop(X_AXIS);
                    cDingHoleScanStep = 4;
                }
            }
        }
    }
    else if (cDingHoleScanStep == 4)
    {
        if (!X_DRV)
        {
            cDingHoleScanStep = 100;
        }
    }
    else if (cDingHoleScanStep == 5)
    {}
}

/**
 * 定位打孔动作部分
 * 
 * @author xt (2019-02-23)
 */
void DingHoleWorkPro(void)
{
    long OverLcttmp = 0;
    static unsigned long delaytick = 0;

    if (cDingHoleScanStep == 4)
    {
        cDingHoleWorkStep = 10;
    }

    if (cDingHoleWorkStep == 1)
    {
        currentWorkHoleNum = 0;
        if (ToolValveOutDelay < dwTickCount)
        {
            cDingHoleWorkStep = 2;
        }
    }
    else if (cDingHoleWorkStep == 2)
    { // ɨ����ͷ�󣬸�ֵҪ��Ŀ���
        if (currentWorkHoleNum < currentHoleNum)
        {
            cDingHoleWorkStep = 100;
            bCuiQi = 0;
            delaytick = dwTickCount + 50;
            if (X_DRV) MV_Dec_Stop(X_AXIS);
        }
    }
    else if (cDingHoleWorkStep == 100)
    {
        if (!X_DRV)
        {
            if (delaytick < dwTickCount)
            {
                cDingHoleWorkStep = 3;
            }
        }
        else
        {
            delaytick = dwTickCount + 100;
        }
    }
    else if (cDingHoleWorkStep == 3)
    {
        uint8 flag = 0;

        if (SystemParam->SystemMode != SYSTEM_TYPE0)
        {
            if (Y_Orign && !(Y_DRV))
            {
                flag = 1;
            }
        }
        else
        {
            if (bQiGangBackSgn)
            {
                flag = 1;
            }
        }

        if (flag > 0)
        {
            cDingHoleWorkStep = 4;

        }
    }
    else if (cDingHoleWorkStep == 4)
    {
        OverLcttmp = PositionToPluse(X_AXIS, DingweiDist[currentWorkHoleNum]);
        OverLcttmp += BoardHead;

        //HolePos->Pos_Hole[currentWorkHoleNum] = PluseToPosition(X_AXIS, OverLcttmp);
        MoveAction_Pulse(X_AXIS, OverLcttmp, SpeedParam->XMoveSpeed);

        cDingHoleWorkStep = 5;
        HoleCurrrentDrillDepth = 0;
    }
    else if (cDingHoleWorkStep == 5)
    {
        HoleDrillDepthProcess(DingweiDepth[currentWorkHoleNum]);
        cDingHoleWorkStep = 6;
        cDrillRunStep = 1;
    }
    else if (cDingHoleWorkStep == 6)
    {
        DrillingHoleProc(UserParam->YOrignDist_Left, HoleCurrrentDrillDepth); //TimeParam->Depth);
        if (cDrillRunStep == 0)
        {
            if (HoleCurrrentDrillDepth >= DingweiDepth[currentWorkHoleNum])
            {
                cDingHoleWorkStep = 7;
                HoleCurrrentDrillDepth = 0;
            }
            else
            {
                cDingHoleWorkStep = 5;
            }
        }
    }
    else if (cDingHoleWorkStep == 7)
    {
        currentWorkHoleNum++;
        if (currentWorkHoleNum >= currentHoleNum)
        {
            cDingHoleWorkStep = 0;
        }
        else
        {
            cDingHoleWorkStep = 4;
        }
    }
    else if (cDingHoleWorkStep == 10)
    {
        if (Y_DRV)
        {
            MV_Dec_Stop(Y_AXIS);
        }
        if (X_DRV)
        {
            MV_Dec_Stop(X_AXIS);
        }

        cDingHoleWorkStep = 11;
    }
    else if (cDingHoleWorkStep == 11)
    {
        if (!X_DRV && !Y_DRV)
        {
            if (SystemParam->SystemMode == SYSTEM_TYPE0)
            {
                if (bQiGangComeSgn || bDrillValve)
                {
                    bDrillValve = 0;
                }
            }
            else
            {
                if (!Y_Orign)
                {
                    MoveAction_Pulse(Y_AXIS, 0, SpeedParam->YDrillSpeed);
                }
            }

            cDingHoleWorkStep = 12;
        }
    }
    else if (cDingHoleWorkStep == 12)
    {
        if (SystemParam->SystemMode == SYSTEM_TYPE0)
        {
            if (bQiGangBackSgn)
            {
                cDingHoleWorkStep = 0;
            }
        }
        else
        {
            if (!Y_DRV && Y_Orign)
            {
                cDingHoleWorkStep = 0;
            }
        }
    }
}

// 扫码模块动作部分
void ScanCodeWorkProc(void)
{
    long OverLcttmp = 0;
    static unsigned long delaytick = 0;
    static unsigned char slotworkflag = 0;
    static unsigned char holeworkflag = 0;

    if (cDingHoleScanStep == 4)
    {
        cDingHoleWorkStep = 10;
    }

    if (cDingHoleWorkStep == 1)
    {
        SlotTypeFlag = 0;
        slotworkflag = bOneToolMode_Slot;
        holeworkflag = bOneToolMode_Hole;
        VariableInit_Int();

        if (FactoryParam->Code_Probe > 0)
        {
            if (ToolValveOutDelay < dwTickCount)
            {
                StartFlag = 1;
                cDingHoleScanStep = 1; // ��ʼɨ��
                cDingHoleWorkStep = 2;
                MV_Set_Startv(X_AXIS, 3);
                MV_AccDec_Set(X_AXIS, 165, 100);
                MV_Const_Move(X_AXIS, SpeedParam->ScanSpeed2, XCW);
            }
        }
        else
        {
            StartFlag = 0;
            cDingHoleWorkStep = 2;
            cDingHoleScanStep = 0;
            BoardHead = 0;
            _gDingHoleDist = XWorkOrignPos;
            currentHoleNum = CodeNumber;
        }
    }
    else if (cDingHoleWorkStep == 2)
    { // ɨ检测到板头
        if (currentWorkHoleNum < currentHoleNum)
        {
            cDingHoleWorkStep = 100;
            bCuiQi = 0;

            delaytick = dwTickCount + 50;
            MV_Dec_Stop(X_AXIS);
        }
    }
    else if (cDingHoleWorkStep == 100)
    {
        if (!X_DRV)
        {
            if (delaytick < dwTickCount)
            {
                cDingHoleWorkStep = 3;
            }
        }
        else
        {
            delaytick = FactoryParam->XBackDelayTime * 10;
            delaytick += dwTickCount;
        }
    }
    else if (cDingHoleWorkStep == 3)
    {
        if (!Y_DRV)
        {
            if (Y_Orign)
            {
                cDingHoleWorkStep = 31;
            }
            else
            {
                cDingHoleWorkStep = 30;
            }
        }
    }
    else if (cDingHoleWorkStep == 30)
    {
        if (SystemParam->SystemMode != SYSTEM_TYPE0)
        {
            MoveAction_Pulse(Y_AXIS, 0, SpeedParam->YIdleSpeed);
        }
        else
        {
            bDrillValve = 0;
        }
        cDingHoleWorkStep = 3;
    }
    else if (cDingHoleWorkStep == 31)
    {
        if (FactoryParam->WorkToolNumber == TOOLNUM_2)
        { // 双刀具出刀
            if (slotworkflag > 0)
            {
                cDingHoleWorkStep = 32;
                MotorCoilSelectStep = 0;
                cToolOutFlag = TOOLSELECT_R;

                if (SystemParam->SystemMode == SYSTEM_TYPE3)
                {
                    MotorCoilSelectStep = 1;
                }
            }
            else if (holeworkflag)
            {
                cDingHoleWorkStep = 33;
                MotorCoilSelectStep = 0;
                cToolOutFlag = TOOLSELECT_L;

                if (SystemParam->SystemMode == SYSTEM_TYPE3)
                {
                    MotorCoilSelectStep = 1;
                }
            }
        }
        else
        { // 单刀
            MotorCoilSelectStep = 0;
            if (SystemParam->SystemMode < SYSTEM_TYPE2)
            {
                cToolOutFlag = TOOLSELECT_L;

                if (slotworkflag > 0)
                {
                    cDingHoleWorkStep = 32;
                }
                else if (holeworkflag > 0)
                {
                    cDingHoleWorkStep = 33;
                }
            }
            else if (SystemParam->SystemMode == SYSTEM_TYPE2)
            {
                cToolOutFlag = TOOLSELECT_R;
                cDingHoleWorkStep = 32;
            }
            else if (SystemParam->SystemMode == SYSTEM_TYPE3)
            {
                if (slotworkflag > 0)
                {
                    cToolOutFlag = TOOLSELECT_R;
                    MotorCoilSelectStep = 1;
                    cDingHoleWorkStep = 32;
                }
                else if (holeworkflag > 0)
                {
                    cToolOutFlag = TOOLSELECT_L;
                    MotorCoilSelectStep = 1;
                    cDingHoleWorkStep = 33;
                }
            }
            else if (SystemParam->SystemMode == SYSTEM_TYPE4)
            {
                if (slotworkflag > 0)
                {
                    cDingHoleWorkStep = 32;
                }
                else if (holeworkflag > 0)
                {
                    cDingHoleWorkStep = 33;
                }
            }
        }
    }

    else if (cDingHoleWorkStep == 32)
    { // 铣槽
        if (SystemParam->SystemMode < SYSTEM_TYPE2)
        {
            ToolOutProc();
            if (MotorCoilSelectStep == 0)
            {
                CurrentToolOffsetY = UserParam->YOrignDist_Left;
                CurrentToolOffsetX = FactoryParam->Code_LeftToolOffSet;
                cDingHoleWorkStep = 4;
            }
        }
//      else if (SystemParam->SystemMode == SYSTEM_TYPE3)
//      {
//          ToolOutProc();
//          if (MotorCoilSelectStep == 0)
//          {
//              CurrentToolOffsetY = FactoryParam->YOrignDist;
//              CurrentToolOffsetX = FactoryParam->DrillOffset_Code;
//              cDingHoleWorkStep = 4;
//          }
//      }
//      else if (SystemParam->SystemMode == SYSTEM_TYPE2)
//      {
//          ToolOutProc();
//          if (MotorCoilSelectStep == 0)
//          {
//              CurrentToolOffsetY = FactoryParam->YOrignDist;
//              CurrentToolOffsetX = FactoryParam->DrillOffset_Code;
//              cDingHoleWorkStep = 4;
//          }
//      }
//      else if (SystemParam->SystemMode == SYSTEM_TYPE4)
//      {
//          if (slotworkflag > 0)
//          {
//              ToolOutProc();
//              if (MotorCoilSelectStep == 0)
//              {
//                  CurrentToolOffsetY = FactoryParam->YOrignDist;
//                  CurrentToolOffsetX = FactoryParam->DrillOffset_Code;
//                  cDingHoleWorkStep = 4;
//              }
//          }
//      }
        else
        {
            ToolOutProc();
            if (MotorCoilSelectStep == 0)
            {
                CurrentToolOffsetY = FactoryParam->YOrignDist;
                CurrentToolOffsetX = FactoryParam->DrillOffset_Code;
                cDingHoleWorkStep = 4;
            }
        }
    }
    else if (cDingHoleWorkStep == 33)
    { // 打孔的刀具
        ToolOutProc();
        if (MotorCoilSelectStep == 0)
        {
            //LeftToolOutProc();

            CurrentToolOffsetY = UserParam->YOrignDist_Left;
            CurrentToolOffsetX = FactoryParam->Code_LeftToolOffSet;
            cDingHoleWorkStep = 4;
        }

//      if (SystemParam->SystemMode == SYSTEM_TYPE3)
//      {
//          ToolOutProc();
//          if (MotorCoilSelectStep == 0)
//          {
//              //LeftToolOutProc();
//
//              CurrentToolOffsetY = UserParam->YOrignDist_Left;
//              CurrentToolOffsetX = FactoryParam->Code_LeftToolOffSet;
//              cDingHoleWorkStep = 4;
//          }
//      }
//      else if (SystemParam->SystemMode == SYSTEM_TYPE2 || SystemParam->SystemMode == SYSTEM_TYPE4)
//      {
//          //LeftToolOutProc();
//          ToolOutProc();
//          if (MotorCoilSelectStep == 0)
//          {
//              CurrentToolOffsetY = UserParam->YOrignDist_Left;
//              CurrentToolOffsetX = FactoryParam->Code_LeftToolOffSet;
//              cDingHoleWorkStep = 4;
//          }
//      }
    }
    else if (cDingHoleWorkStep == 4)
    {
        CurrentBoardOrign = _gDingHoleDist;

        CurrentWorkOrign = CodeWorkOrign[currentWorkHoleNum];
        CurrentWorkOrign += CurrentToolOffsetX;
        CurrentWorkLength = CodeWorkLength[currentWorkHoleNum];
        CurrentWorkDepth = CodeWorkDepth[currentWorkHoleNum];

        cDingHoleWorkStep = 40;
    }
    else if (cDingHoleWorkStep == 40)
    {
        if (FactoryParam->WorkToolNumber == TOOLNUM_2)
        { // 双刀具
            cDingHoleWorkStep = 41;
        }
        else
        { // 单刀具
            cDingHoleWorkStep = 45;
        }
    }
    else if (cDingHoleWorkStep == 41)
    { // 双刀
        if (slotworkflag)
        {
            cDingHoleWorkStep = 42;
        }
        else if (holeworkflag)
        {
            CurrentWorkLength = 0;
            cDingHoleWorkStep = 43;
        }
    }
    else if (cDingHoleWorkStep == 42)
    { // 铣槽
        HoleCurrrentDrillDepth = 0;
        cDingHoleWorkStep = 8;
        cDrillRunStep = 1;

        SlotTypeFlag = 0;
        if (UserParam->CodeSlotType)
        {
            SlotTypeFlag = 2;
        }
        //OverLcttmp = PositionToPluse(X_AXIS, CurrentWorkOrign);
        //OverLcttmp += CurrentBoardOrign;
        //MoveAction_Pulse(X_AXIS, OverLcttmp, SpeedParam->XMoveSpeed);
    }
    else if (cDingHoleWorkStep == 43)
    { // 打孔
        HoleCurrrentDrillDepth = 0;
        cDingHoleWorkStep = 5;

        OverLcttmp = PositionToPluse(X_AXIS, CurrentWorkOrign);
        OverLcttmp += CurrentBoardOrign;
        MoveAction_Pulse(X_AXIS, OverLcttmp, SpeedParam->XMoveSpeed);
    }
    else if (cDingHoleWorkStep == 44)
    { // 跳过
        cDingHoleWorkStep = 7;
    }
    else if (cDingHoleWorkStep == 45)
    { // 单刀
        if (FactoryParam->WorkDataSource == WORKDATASOURCE_PARA)
        { // 长度深度来自数据
            cDingHoleWorkStep = 41;
        }
        else
        { // 长度深度 来自文件
            cDingHoleWorkStep = 46;
        }
    }
    else if (cDingHoleWorkStep == 46)
    {
        if (bOneToolMode_Slot && bOneToolMode_Hole)
        { // 铣槽打孔一起加工
            cDingHoleWorkStep = 47;
        }
        else if (bOneToolMode_Slot)
        { // 铣槽
            cDingHoleWorkStep = 48;
        }
        else
        { // 打孔
            cDingHoleWorkStep = 49;
        }
    }
    else if (cDingHoleWorkStep == 47)
    { // 铣槽打孔一起加工
        if (CurrentWorkLength > 0)
        { // 铣槽
            cDingHoleWorkStep = 42;
        }
        else
        { // 打孔
            cDingHoleWorkStep = 43;
        }
    }
    else if (cDingHoleWorkStep == 48)
    { // 铣槽
        if (CurrentWorkLength == 0)
        { // 跳过
            cDingHoleWorkStep = 44;
        }
        else
        { // 铣槽
            cDingHoleWorkStep = 42;
        }
    }
    else if (cDingHoleWorkStep == 49)
    { // 打孔
        if (CurrentWorkLength > 0)
        { // 跳过
            cDingHoleWorkStep = 44;
        }
        else
        { // 打孔
            cDingHoleWorkStep = 43;
        }
    }
    else if (cDingHoleWorkStep == 5)
    {
        if (ToolValveOutDelay < dwTickCount)
        {
            HoleDrillDepthProcess(CurrentWorkDepth);
            cDingHoleWorkStep = 6;
            cDrillRunStep = 1;
        }
    }
    else if (cDingHoleWorkStep == 6)
    {
        DrillingHoleProc(UserParam->YOrignDist_Left, HoleCurrrentDrillDepth);
        if (cDrillRunStep == 0)
        {
            if (HoleCurrrentDrillDepth >= CurrentWorkDepth)
            {
                cDingHoleWorkStep = 7;
                HoleCurrrentDrillDepth = 0;
            }
            else
            {
                cDingHoleWorkStep = 5;
            }
        }
    }
    else if (cDingHoleWorkStep == 7)
    {
        currentWorkHoleNum++;
        if (currentWorkHoleNum >= currentHoleNum)
        {
            if (slotworkflag > 0)
            {
                slotworkflag = 0;
            }
            else if (holeworkflag > 0)
            {
                holeworkflag = 0;
            }

            if (holeworkflag) // && (UserParam->WorkDataSource == 1))
            {                 //双刀具打孔
                cDingHoleWorkStep = 9;
            }
            else
            {
                cDingHoleWorkStep = 10;
            }
        }
        else
        {
            cDingHoleWorkStep = 4;
        }
    }
    else if (cDingHoleWorkStep == 8)
    {
        SlotWorkData.yorign = CurrentToolOffsetY;
        SlotWorkData.ydepth = CurrentWorkDepth;

        SlotWorkData.xstart = CurrentBoardOrign;
        SlotWorkData.xstart += PositionToPluse(X_AXIS, CurrentWorkOrign);

        SlotWorkData.xstop = SlotWorkData.xstart;
        SlotWorkData.xstop += PositionToPluse(X_AXIS, CurrentWorkLength);

        DrillingSlotProc(&SlotWorkData);
        if (cDrillRunStep == 0)
        {
            cDingHoleWorkStep = 7;
        }
    }
    else if (cDingHoleWorkStep == 9)
    {
        if (SystemParam->SystemMode == SYSTEM_TYPE3 || SystemParam->SystemMode == SYSTEM_TYPE4)
        {
            int i = 0;
            int j = 0;
            int k = currentHoleNum * 2;

            for (i = currentHoleNum; i > 0; i--)
            {
                j = (i * 2) - 1;
                CodeWorkOrign[j] = CodeWorkOrign[i - 1];
                CodeWorkOrign[j] += CodeWorkLength[i - 1];
                CodeWorkOrign[j - 1] = CodeWorkOrign[i - 1];
                //CodeWorkOrign[j - 1] -= CodeWorkLength[i - 1];
            }

            i = 0;
            j = k - i - 1;
            while (i < j)
            {
                CodeWorkOrign[i] = CodeWorkOrign[i] ^ CodeWorkOrign[j];
                CodeWorkOrign[j] = CodeWorkOrign[i] ^ CodeWorkOrign[j];
                CodeWorkOrign[i] = CodeWorkOrign[i] ^ CodeWorkOrign[j];
                i++;
                j = k - i - 1;
            }

            for (i = 0; i < k; i++)
            {
                CodeWorkLength[i] = 0;
                CodeWorkDepth[i] = UserParam->HoleDepth_Left;
            }

            currentHoleNum = k;
            currentWorkHoleNum = 0;
            cDingHoleWorkStep = 3;
        }
        else if (SystemParam->SystemMode == SYSTEM_TYPE2)
        {
            int i = 0;
            int j = currentHoleNum - i - 1;

            currentWorkHoleNum = 0;
            cDingHoleWorkStep = 3;
            while (i < j)
            {
                CodeWorkOrign[i] = CodeWorkOrign[i] ^ CodeWorkOrign[j];
                CodeWorkOrign[j] = CodeWorkOrign[i] ^ CodeWorkOrign[j];
                CodeWorkOrign[i] = CodeWorkOrign[i] ^ CodeWorkOrign[j];
                i++;
                j = currentHoleNum - i - 1;
            }

            for (i = 0; i < currentHoleNum; i++)
            {
                CodeWorkLength[i] = 0;
                CodeWorkDepth[i] = UserParam->HoleDepth_Left;
            }
        }
    }
    else if (cDingHoleWorkStep == 10)
    {
        if (Y_DRV)
        {
            MV_Dec_Stop(Y_AXIS);
        }
        if (X_DRV)
        {
            MV_Dec_Stop(X_AXIS);
        }

        cDingHoleWorkStep = 11;
    }
    else if (cDingHoleWorkStep == 11)
    {
        if (!X_DRV && !Y_DRV)
        {
            if (SystemParam->SystemMode == SYSTEM_TYPE0)
            {
                if (bQiGangComeSgn || bDrillValve)
                {
                    bDrillValve = 0;
                }
            }
            else
            {
                if (!Y_Orign)
                {
                    MoveAction_Pulse(Y_AXIS, 0, SpeedParam->YDrillSpeed);
                }
            }

            cDingHoleWorkStep = 12;
        }
    }
    else if (cDingHoleWorkStep == 12)
    {
        if (SystemParam->SystemMode == SYSTEM_TYPE0)
        {
            if (bQiGangBackSgn)
            {
                cDingHoleWorkStep = 0;
            }
        }
        else
        {
            if (!Y_DRV && Y_Orign)
            {
                cDingHoleWorkStep = 0;
            }
        }
    }
}

void CalTempDist(void)
{
    TempDist.Dist32 = PositionToPluse(X_AXIS, FactoryParam->ChaXiaoDist);     //
    TempDist.MinHoleR = PositionToPluse(X_AXIS, FactoryParam->MinHoleRaidus); //
    TempDist.MaxHoleR = PositionToPluse(X_AXIS, FactoryParam->MaxHoleRaidus); //

    TempDist.XMaxLength = PositionToPluse(X_AXIS, SystemParam->XMaxLength); //
    TempDist.Work1Dist = PositionToPluse(X_AXIS, SystemParam->Work1Dist);   //
    TempDist.Work2Dist = PositionToPluse(X_AXIS, SystemParam->Work2Dist);   //
    TempDist.Work3Dist = PositionToPluse(X_AXIS, SystemParam->Work3Dist);
    TempDist.DrillRaidus = PositionToPluse(X_AXIS, FactoryParam->DrillRaidus);
    TempDist.SlowMotorBuchang = PositionToPluse(X_AXIS, FactoryParam->SlowMotorBuchang);

    if (SystemParam->SystemMode != SYSTEM_TYPE0)
    {
        TempDist.YMaxLength = PositionToPluse(Y_AXIS, SystemParam->YMaxLength);
        TempDist.CaoPicDepth = PositionToPluse(Y_AXIS, FactoryParam->CaoPicDepth);
        TempDist.YOrignDist = PositionToPluse(Y_AXIS, FactoryParam->YOrignDist);
        TempDist.HoleDepth = PositionToPluse(Y_AXIS, FactoryParam->HoleDepth);
        TempDist.ChaXiaoDepth = PositionToPluse(Y_AXIS, FactoryParam->ChaXiaoDepth);
        TempDist.HoleFeedDepth = PositionToPluse(Y_AXIS, FactoryParam->HoleFeedDepth);
    }
}

uint8 GotoWorkOrign(void)
{
    uint8 flag = 0;

    if (SystemParam->SystemMode > 0) // 1 ˫˽��
    {
        if (Y_Orign && !Y_DRV)
        {
            flag = RunCheck();
        }
    }
    else
    {
        if (bQiGangBackSgn)
        {
            flag = RunCheck();
        }
    }

    return flag;
}

uint8 LookForBoardHeadCheck(void)
{
    uint8 flag = 0;
    unsigned long temp = 0;

    if (WorkingFlag == 0)
    {
        if (SysCurrentParam->XDistance < 0)
        {
            flag = 1;
        }
    }
    else if (WorkingFlag == 1)
    {
        if (SystemParam->Work2Dist > 20000)
        {
            temp = SystemParam->Work2Dist - 20000;
            if ((SysCurrentParam->XDistance < temp))
            { // ���߽�
                flag = 1;
            }
        }
    }
    else if (WorkingFlag == 2)
    {
        if (SystemParam->Work3Dist > 20000)
        {
            temp = SystemParam->Work3Dist - 20000;
            if ((SysCurrentParam->XDistance < temp))
            { // ���߽�
                flag = 1;
            }
        }
    }

    return flag;
}

void LookForBoardHead(void)
{
    static uint32 delay = 0;
    uint8 flag = 0;

    if (LookForBoardHeadstep == 1)
    {
        if (!CheckObjectSgn)
        {
            LookForBoardHeadstep = 0;
        }
        else
        { // �а��ź�
            LookForBoardHeadstep = 2;
        }
    }
    else if (LookForBoardHeadstep == 2)
    {
        MV_Set_Startv(X_AXIS, 0);
        MV_AccDec_Set(X_AXIS, XIDEACC3, XIDEACC3);
        MV_Const_Move(X_AXIS, (SpeedParam->XIdleSpeed / 2), XCCW); // 1);
        delay = dwTickCount + 50;
        LookForBoardHeadstep = 3;
    }
    else if (LookForBoardHeadstep == 3)
    {
        if (_gcXZero == 0) // �������
        {
            if (!CheckObjectSgn)
            { // û���ź�
                if (delay < dwTickCount)
                {
                    LookForBoardHeadstep = 4; // �ҵ���ͷ
                    MV_Dec_Stop(X_AXIS);
                }
            }
            else
            {
                delay = dwTickCount + 50;
                flag = LookForBoardHeadCheck();
                if (flag > 0)
                { // �����߽�
                    AlarmNum = 14;
                    LookForBoardHeadstep = 6;
                    MV_Dec_Stop(X_AXIS);
                }
            }
        }
        else
        { // ��⵽ԭ��
            AlarmNum = 14;
            LookForBoardHeadstep = 6;
            MV_Dec_Stop(X_AXIS);
        }
    }
    else if (LookForBoardHeadstep == 4)
    {
        if (!X_DRV)
        {
            if (delay < dwTickCount)
            {
                LookForBoardHeadstep = 1;
            }
        }
        else
        {
            delay = dwTickCount + 10 * FactoryParam->XBackDelayTime;
        }
    }
    else if (LookForBoardHeadstep == 6)
    {
        if (!X_DRV)
        {
            LookForBoardHeadstep = 100;
        }
    }
}

/************************************************/
/*
�ռ�����ֹͣ

*/
/************************************************/
void OutSideStop(void)
{
    if (bRunning)
    {
        if (bLongBoard)
        {
            return;
        }

        if (WorkingFlag == 0 && bRunning1)
        {
            if ((SysCurrentParam->XDistance > SystemParam->Work2Dist + 20000) &&
                (cScanHoleStep == 1 ||
                 cWorkScanStep == 1 ||
                 cTongCao2ScanStep == 1))
            {
                AlarmNum = 14; //
                if (X_DRV)
                {
                    MV_Dec_Stop(X_AXIS);
                }
                cRunStep = 0;
            }
        }
        else if (WorkingFlag == 1 && bRunning2)
        {
            if ((SysCurrentParam->XDistance > SystemParam->Work3Dist + 20000) &&
                (cScanHoleStep == 1 ||
                 cWorkScanStep == 1 ||
                 cTongCao2ScanStep == 1))
            {
                AlarmNum = 14; //
                if (X_DRV)
                {
                    MV_Dec_Stop(X_AXIS);
                }
                cRunStep = 0;
            }
        }
    }
}
//static unsigned temptick = 0;

void DepthCheckProc(void)
{
    unsigned long temp = 0;

    temp = SystemParam->YMaxLength;
    temp -= FactoryParam->YOrignDist;

    if (FactoryParam->CodeSlotDepth > temp)
    { // 右刀铣槽
        FactoryParam->CodeSlotDepth = temp - 1;
    }
    if (FactoryParam->CodeHoleDepth > temp)
    { // 右刀打孔
        FactoryParam->CodeHoleDepth = temp - 1;
    }
    if (UserParam->TW_SlotDepth > temp)
    {
        UserParam->TW_SlotDepth = temp - 1;
    }
    if (UserParam->TW_SlotOneDepth > temp)
    {
        UserParam->TW_SlotOneDepth = temp - 1;
    }

    if (GETBIT(DISP_T_SLOT_TOOL))
    {
        if (FactoryParam->TongCaoDepth > temp)
        {
            FactoryParam->TongCaoDepth = temp - 1;
        }
    }

    temp = UserParam->YMaxLength_Left;
    temp -= UserParam->YOrignDist_Left;

    if (UserParam->HoleDepth_Left > temp)
    { // 左刀打孔
        UserParam->HoleDepth_Left = temp - 1;
    }
    if (FactoryParam->HoleDepth > temp)
    {
        FactoryParam->HoleDepth = temp - 1;
    }

    if (FactoryParam->ChaXiaoDepth > temp)
    {
        FactoryParam->ChaXiaoDepth = temp - 1;
    }
    if (UserParam->TW_HoleDepth > temp)
    {
        UserParam->TW_HoleDepth = temp - 1;
    }

    if (GETBIT(DISP_T_SLOT_TOOL) == 0)
    {
        if (FactoryParam->TongCaoDepth > temp)
        {
            FactoryParam->TongCaoDepth = temp - 1;
        }
    }
}

void AutoWorkStopCheck(void)
{
    if (!bLongBoard)
    {
        if (WorkingFlag == 0)
        {
            Work1StopCheck();
        }
        else if (WorkingFlag == 1)
        {
            Work2StopCheck();
        }
        else if (WorkingFlag == 2)
        {
            Work3StopCheck();
        }
    }
    else
    {
        bRunning1 = 0;
        bCuiQi = 0;

        MoveAction_Pulse(X_AXIS, TempDist.Work1Dist, SpeedParam->XIdleSpeed);
        // ScanCodeWorkFlagClear();
    }
}

void PressValveOutClear(void)
{
    if (SystemParam->DemoFunction == 0)
    {
        if (!bLongBoard)
        { // 短板
            bPress1_Vavle = bRunning1;
            bPress2_Vavle = bRunning2;
            bPress3_Vavle = bRunning3;
        }
        else
        { // 长板
            bPress1_Vavle = bRunning1;
            bPress2_Vavle = bRunning1;
            bPress3_Vavle = bRunning1;
        }
    }
}

void CodeWorkLoad(void)
{
    if (bCodeWorkFlag1)
    {
        if (bCodeWorkDataPos)
        {
            bCodeLoadFlag1 = 0;
        }
        else
        {
            bCodeLoadFlag1 = 1;
        }

        cRunStep = 102;
    }
    else if (bCodeWorkFlag2)
    {
        bCodeLoadFlag2 = 1;
        cRunStep = 102;
    }
    else if (bCodeWorkFlag3)
    {
        bCodeLoadFlag3 = 1;
        cRunStep = 102;
    }
    else if (bCodeWorkFlag4)
    {
        bCodeLoadFlag4 = 1;
        cRunStep = 102;
    }
}

/**
 * 清除标识符
 */
void ScanCodeWorkFlagClear(void)
{
    if (bCodeWorkFlag1 == 1)
    {
        bCodeWorkFlag1 = 0;
        if (bCodeWorkFlag2)
        {
            bCodeLoadFlag2 = 1;
        }
        else if (bCodeWorkFlag3)
        {
            bCodeLoadFlag3 = 1;
        }
        else if (bCodeWorkFlag4 == 1)
        {
            bCodeLoadFlag4 = 1;
        }
    }
    else if (bCodeWorkFlag2 == 1)
    {
        bCodeWorkFlag2 = 0;
        if (bCodeWorkFlag3)
        {
            bCodeLoadFlag3 = 1;
        }
        else if (bCodeWorkFlag4 == 1)
        {
            bCodeLoadFlag4 = 1;
        }
    }
    else if (bCodeWorkFlag3 == 1)
    {
        bCodeWorkFlag3 = 0;
        if (bCodeWorkFlag4)
        {
            bCodeLoadFlag4 = 1;
        }
    }
    else if (bCodeWorkFlag4 == 1)
    {
        bCodeWorkFlag4 = 0;
    }
}

/**
 * ��һ��
 * 
 * @author xt (2019-04-07)
 * 
 * @param void 
 * 
 * @return uint8 
 */
//uint8 ScanCodeLoadOver(void)
//{
//    if (bCodeWorkFlag2 == 1)
//    {
//        if (bCodeLoadFlag2 == 0)
//        {
//            return 1;
//        }
//    }
//    else if (bCodeWorkFlag3 == 1)
//    {
//        if (bCodeLoadFlag3 == 0)
//        {
//            return 1;
//        }
//    }
//    else if (bCodeWorkFlag4 == 1)
//    {
//        if (bCodeLoadFlag4 == 0)
//        {
//            return 1;
//        }
//    }
//    else
//    {
//        return 1;
//    }
//
//    return 0;
//}

/**
 * 扫码模式加工数据加载完成
 * 
 * @author xt (2019-05-06)
 * 
 * @param void 
 * 
 * @return uint8 
 */
uint8 ScanCodeLoadOver(void)
{
    if (bCodeWorkFlag1)  //有数据
    {
        if (!bCodeLoadFlag1)  //已加工
        {
            return 1;
        }
    }
    else if (bCodeWorkFlag2)
    {
        if (!bCodeLoadFlag2)
        {
            return 1;
        }
    }
    else if (bCodeWorkFlag3)
    {
        if (!bCodeLoadFlag3)
        {
            return 1;
        }
    }
    else if (bCodeWorkFlag4)
    {
        if (!bCodeLoadFlag4)
        {
            return 1;
        }
    }

    return 0;
}

void InitToolOutProc(void)
{
    MotorCoilSelectStep = 0;
    cToolOutFlag = 0;
    if (FactoryParam->WorkMode == WorkMode_Slot)
    {
        if (SystemParam->SystemMode == SYSTEM_TYPE2 || SystemParam->SystemMode == SYSTEM_TYPE1 || SystemParam->SystemMode == SYSTEM_TYPE0)
        {
            cToolOutFlag = TOOLSELECT_L;
            return;
        }
        else if (SystemParam->SystemMode == SYSTEM_TYPE3 || SystemParam->SystemMode == SYSTEM_TYPE4)
        {
            if (SystemParam->SystemMode == SYSTEM_TYPE3)
            {
                MotorCoilSelectStep = 1;
            }

            if (GETBIT(DISP_T_SLOT))
            { // 通槽
                if (GETBIT(DISP_T_SLOT_TOOL))
                {
                    cToolOutFlag = TOOLSELECT_R;
                    return;
                }
                else
                {
                    cToolOutFlag = TOOLSELECT_L;
                    return;
                }
            }
            else if (GETBIT(DISP_TW_SLOT) || GETBIT(DISP_TW_SLOT_ONE))
            { // 头尾定长段槽 和单槽
                cToolOutFlag = TOOLSELECT_R;
                return;
            }
            else if (GETBIT(DISP_TW_HOLE))
            { // 头尾定长孔
                cToolOutFlag = TOOLSELECT_L;
                return;
            }
        }
    }
    else if (FactoryParam->WorkMode == WorkMode_Hole)
    {
        if (SystemParam->SystemMode == SYSTEM_TYPE2 || SystemParam->SystemMode == SYSTEM_TYPE1 || SystemParam->SystemMode == SYSTEM_TYPE0)
        {
            cToolOutFlag = TOOLSELECT_L;
            return;
        }
        else if (SystemParam->SystemMode == SYSTEM_TYPE3 || SystemParam->SystemMode == SYSTEM_TYPE4)
        {
            if (SystemParam->SystemMode == SYSTEM_TYPE3)
            {
                MotorCoilSelectStep = 1;
            }
            if (DingWorkFlag == 1)
            {
                cToolOutFlag = TOOLSELECT_R;
                return;
            }
            else if (DingWorkFlag == 2)
            {
                cToolOutFlag = TOOLSELECT_L;
                return;
            }
        }
    }
    else if (FactoryParam->WorkMode == WorkMode_Scan)
    {
        MotorCoilSelectStep = 1;
        cToolOutFlag = TOOLSELECT_L;
        return;
    }
    else if (FactoryParam->WorkMode == WorkMode_Code)
    {
        if (SystemParam->SystemMode == SYSTEM_TYPE2 || SystemParam->SystemMode == SYSTEM_TYPE1 || SystemParam->SystemMode == SYSTEM_TYPE0)
        {
            cToolOutFlag = TOOLSELECT_L;
            return;
        }
        else if (SystemParam->SystemMode == SYSTEM_TYPE4 || SystemParam->SystemMode == SYSTEM_TYPE3)
        {
            if (SystemParam->SystemMode == SYSTEM_TYPE3)
            {
                MotorCoilSelectStep = 1;
            }
            if (FactoryParam->WorkToolNumber == TOOLNUM_2)
            {
                cToolOutFlag = TOOLSELECT_R;
                return;
            }

            if (bOneToolMode_Slot)
            {
                cToolOutFlag = TOOLSELECT_R;
                return;
            }
            else if (bOneToolMode_Hole)
            {
                cToolOutFlag = TOOLSELECT_L;
                return;
            }
        }
    }

    //return 0;
}

/**
 * 出刀动作
 * 
 * @author zhang (2019/8/12)
 * 
 * @param void 
 */
void ToolOutProc(void)
{
    switch (SystemParam->SystemMode)
    {
    case SYSTEM_TYPE0:
    case SYSTEM_TYPE1:
        LeftToolOutProc();
        MotorCoilSelectStep = 0;
        break;
    case SYSTEM_TYPE3:
        MotorCoilSelect();
        break;
    case SYSTEM_TYPE2:
    case SYSTEM_TYPE4:
        MotorCoilSelectStep = 0;
        if (cToolOutFlag == TOOLSELECT_L)
        {
            LeftToolOutProc();
        }
        else if (cToolOutFlag == TOOLSELECT_R)
        {
            RightToolOutProc();
        }
        break;
    }
}

/**
 * 定位孔槽
 * 
 * @author zhang (2019/8/12)
 */
void DingWorkProc(void)
{
    long temp;

    switch (cWorkRunStep)
    {
    case 1:
        if (FactoryParam->Hole_Probe > 0)
        { // 使用探头的
            temp = MV_Get_Command_Pos(X_AXIS);
            ulEncoderCount = temp * XEncodeDivide;
            XWorkOrignPos = temp;
        }
        else
        {
            temp = SystemParam->Work1Dist;
            XWorkOrignPos = PositionToPluse(X_AXIS, temp);
        }
        cWorkScanStep = 1;
        cWorkRunStep = 2;
        break;
    case 2:
        DingWorkScanProc();
        if (cWorkScanStep == 0)
        {
            cWorkRunStep = 3;
        }
        break;
    case 3:
        if (DingWorkFlag == 1)   //铣槽
        {
            cDuanCaoRunStep = 1;
            cWorkRunStep = 4;
        }
        else if (DingWorkFlag == 2)   //打孔
        {
            cWorkRunStep = 7;
            cDingHoleWorkStep = 1;
        }
        break;
    case 4:
        DuanCaoWorkPro();
        if (cDuanCaoRunStep == 0)
        {
            cWorkRunStep = 5;
        }
        break;
    case 5:
        VariableInit_Hole();
        if (DingWorkFlag == 2)
        {
            cToolOutFlag = TOOLSELECT_L;
            if (SystemParam->SystemMode == SYSTEM_TYPE3)
            {
                MotorCoilSelectStep = 1;
            }
            else
            {
                MotorCoilSelectStep = 0;
            }
            cWorkRunStep = 6;
        }
        else
        {
            cWorkRunStep = 8;
        }
        break;
    case 6:
        ToolOutProc();
        if (MotorCoilSelectStep == 0)
        {
            cDingHoleWorkStep = 1;
            currentHoleNum = NeedWorkNum;
            cWorkRunStep = 7;
        }
        break;
    case 7:
        DingHoleWorkPro(); //
        if (cDingHoleWorkStep == 0)
        {
            if (!X_DRV)
            {
                cWorkRunStep = 8;
            }
        }
        break;
    case 8:
        cWorkRunStep = 0;
        break;
    default:
        break;
    }
}

/**
 *  
 * 自动工作大函数 
 * 
 * @author xt (2019-03-02)
 */
void AutoWorkProc(void)
{
    uint8 flag = 0;
    //uint8 _ltooloutflag = 0;

    if (cRunStep == 100)
    {
        if (RstPphlDlay == 0)
        {
            cRunStep = 101;
        }
    }
    else if (cRunStep == 101)
    {
        if (FactoryParam->WorkMode == WorkMode_Code)
        {
            CodeWorkLoad();

            if (cRunStep == 101)
            {
                bAlarmFlag = 1;
                AlarmNum = 19;
                cRunStep = 0;
            }
        }
        else
        {
            cRunStep = 1;
        }
    }
    else if (cRunStep == 102)
    {
        flag = ScanCodeLoadOver();
        if (flag > 0)
        {
            cRunStep = 1;
        }
    }
    else if (cRunStep == 1)
    { // 初始化
        RunInit();
        InitToolOutProc();

        cRunStep = 103;
        //cRunStep = 255;
        RunCheckFlag = 0;
    }
    else if (cRunStep == 103)
    {
        ToolOutProc();
        if (MotorCoilSelectStep == 0)
        {
            cRunStep = 2;
        }
    }
    else if (cRunStep == 2)
    { // 检查启动和 走到工作原点
        flag = GotoWorkOrign();
        if (flag > 0)
        {
            cRunStep = 3;
            LookForBoardHeadstep = 1;
        }
    }
    else if (cRunStep == 3)
    { // 在工作原点时检孔开关有信号的处理
        LookForBoardHead();
        if (LookForBoardHeadstep == 0)
        { // �ҵ���ͷ
            cRunStep = 4;
        }
        else if (LookForBoardHeadstep == 100)
        { // û�ҵ���ͷ
            bAlarmFlag = 1;
        }
    }
    else if (cRunStep == 4)
    { // 分功能开始
        if (FactoryParam->WorkMode == WorkMode_Scan)
        {
            cScanDrillRunStep = 1;
            cScanHoleStep = 0;
            cRunStep = 5;
        }
        else if (FactoryParam->WorkMode == WorkMode_Hole)
        { // 定位孔槽
            cWorkRunStep = 1;
            cRunStep = 6;
        }
        else if (FactoryParam->WorkMode == WorkMode_Slot)
        { // 扫描孔槽
            cTongCao2RunStep = 1;
            cRunStep = 15;
        }
        else if (FactoryParam->WorkMode == WorkMode_Code)
        { // 扫码模式
            long temp = 0;
            if (FactoryParam->Code_Probe > 0)
            { // 使用探头的
                temp = MV_Get_Command_Pos(X_AXIS);
                ulEncoderCount = temp * XEncodeDivide;
                XWorkOrignPos = temp;
            }
            else
            {
                temp = SystemParam->Work1Dist;
                XWorkOrignPos = PositionToPluse(X_AXIS, temp);
            }

            cRunStep = 11;
            cDingHoleWorkStep = 1;
            cDingHoleScanStep = 0;
        }
    }
    else if (cRunStep == 5)
    { // 扫描打孔
        ScanDrillHoleRun();
        if (cScanDrillRunStep == 0)
        {
            cRunStep = 22;
        }
    }
    else if (cRunStep == 6)
    { // 定位孔槽
        DingWorkProc();
        if (cWorkRunStep == 0)
        {
            cRunStep = 22;
        }
    }
    else if (cRunStep == 11)
    { // 扫码模式
        CodeWorkScanProc();
        ScanCodeWorkProc();
        if (cDingHoleWorkStep == 0 || cDingHoleScanStep == 100)
        {
            if (!X_DRV)
            {
                cRunStep = 22;
            }
            if (cDingHoleWorkStep == 0)
            {}
        }
    }
    else if (cRunStep == 15)
    { // 铣槽模式
        TWSlotWorkProc();
        if (cTongCao2RunStep == 0)
        {
            if (!X_DRV)
            {
                cRunStep = 22;
            }
        }
    }
    else if (cRunStep == 20)
    { // ͨ��

    }
    else if (cRunStep == 22)
    {
        if (Y_Orign)
        {
            cRunStep = 23;
        }
        else
        {
            if (!Y_DRV)
            {
                MoveAction_Pulse(Y_AXIS, 0, SpeedParam->XIdleSpeed);
            }
        }
    }
    else if (cRunStep == 23)
    {
        flgSgn = 0;
        StartFlag = 0;
        currentHoleNum = 0;

        if (!X_DRV)
        {
            AutoWorkStopCheck();
            PressValveOutClear();
            if (FactoryParam->WorkMode == WorkMode_Code)
            {
                ScanCodeWorkFlagClear();
            }
            cRunStep = 24;
        }
    }
    else if (cRunStep == 24)
    {
        if (!X_DRV)
        {
            NoworkStopdelay = 100 * FactoryParam->NoWorktime + 1000;
            NoworkStopdelay += dwTickCount;

            if (FactoryParam->WorkMode == WorkMode_Code)
            {
                if (FactoryParam->WorkToolNumber == TOOLNUM_2)
                {
                    cToolOutFlag = TOOLSELECT_R;
                    MotorCoilSelectStep = 1;
                }
                else
                {
                    if (bOneToolMode_Slot)
                    {
                        cToolOutFlag = TOOLSELECT_R;
                        MotorCoilSelectStep = 1;
                    }
                    else if (bOneToolMode_Hole)
                    {
                        cToolOutFlag = TOOLSELECT_L;
                        MotorCoilSelectStep = 1;
                    }
                }

                if (bCodeWorkDataPos)
                { // two tool manual
                    bCodeWorkFlag1 = 1;
                }

                cRunStep = 25;
            }
            else
            {
                cRunStep = 1;
            }
        }
    }
    else if (cRunStep == 25)
    {
        ToolOutProc();
        if (MotorCoilSelectStep == 0)
        {
            cRunStep = 26;
        }
    }
    else if (cRunStep == 26)
    {
        if (bRunning1 || bRunning2 || bRunning3)
        {
            if (bCodeWorkFlag1 || bCodeWorkFlag2 || bCodeWorkFlag3 || bCodeWorkFlag4)
            {
                cRunStep = 27;
            }
            else
            {
                bAlarmFlag = 1;
                AlarmNum = 20;
                cRunStep = 0;
            }
        }
        if (NoworkStopdelay < dwTickCount)
        {
            M43 = 1;
        }
    }
    else if (cRunStep == 27)
    {
        if (ScanCodeLoadOver() == 1)
        {
            cRunStep = 1;
        }
    }
}

void AutoRun(void)
{
    UserCurrentParam->debugdata1 = cRunStep;
    UserCurrentParam->debugdata2 = cDuanCaoRunStep;
    UserCurrentParam->debugdata3 = SlotTypeFlag;

    AutoWorkProc();
    OutSideStop();

    if ((!(X_DRV || Y_DRV)) && !bKeyEnable)
    {
        CalGearRatio();
        DepthCheckProc();
        CalTempDist();
        MV_Set_Limit_P(X_AXIS, TempDist.XMaxLength);
    }
}

/************************************************/
/*
定时器中断，
*/
/************************************************/
char tim7_temp = 0;
void TIM7_IRQHandler(void)
{ // 10us一次
    if (StartFlag > 0)
    {
        if (GPIOG->IDR & PIN2)
        {
            CheckSgnFilterOff++;
            if (CheckSgnFilterOff > 1)
            {
                flgSgn = 0;
                CheckSgnFilter = 0;
            }
        }
        else
        {
            CheckSgnFilter++;
            if (CheckSgnFilter > 1)
            {
                flgSgn = 1;
                CheckSgnFilterOff = 0;
            }
        }

        if (flgSgn != flgSgnOld)
        {
            flgSgnOld = flgSgn;
            if (flgSgn > 0)
            { // 上升沿
                tim7_temp = CheckHoleNum[0];
                HoleinfoB[tim7_temp] = ulEncoderCount;
                CheckHoleNum[0]++;
                flgSgn = 1;
            }
            else
            { // 下降沿
                tim7_temp = CheckHoleNum[1];
                HoleinfoF[tim7_temp] = ulEncoderCount;
                CheckHoleNum[1]++;
                flgSgn = 0;
            }
            if (CheckHoleNum[0] > 99) CheckHoleNum[0] = 99;
            if (CheckHoleNum[1] > 99) CheckHoleNum[1] = 99;
        }
    }

    TIM7->SR = 0;
}

// ���жϰ�ͷ�����жϰ�β
//static unsigned long _lxstopdelay2 = 0;
void ScanCheckHoleProc(void)
{
    long temp = 0;
    static unsigned long _lxstopdelay = 0;

    if (cScanHoleStep == 1)
    {
        if (flgSgn > 0)
        {
            BoardHead = HoleinfoB[0] / XEncodeDivide;
            cScanHoleStep = 2;
        }
    }
    else if (cScanHoleStep == 2)
    { // ����ֻ��Ҫ����β
        uint8 flag = 0;

        if (flgSgn < 1)
        {
            temp = ulEncoderCount;
            temp -= HoleinfoF[CheckHoleNum[1] - 1];
            temp /= XEncodeDivide;

            if (temp > 0)
            {
                if (temp > TempDist.MaxHoleR)
                {
                    cScanHoleStep = 4;
                    StartFlag = 0;
                    MV_Dec_Stop(X_AXIS);
                }
                else
                {
                    flag = 1;
                }
            }
        }
        else
        {
            flag = 1;
        }

        if (flag > 0)
        {
            if (!X_DRV)
            {
                if (_lxstopdelay < dwTickCount)
                {
                    AlarmNum = 17;
                }
            }
            else
            {
                _lxstopdelay = 10 * FactoryParam->XBackDelayTime;
                _lxstopdelay += dwTickCount;
            }
        }

        temp = SystemParam->XMaxLength - FactoryParam->PreDecDistance;
        if (SysCurrentParam->XDistance > temp)
        {
            temp = PositionToPluse(X_AXIS, SystemParam->XMaxLength);
            MV_Set_Dec(X_AXIS, 165);
            MV_Move_To_Position(X_AXIS, temp);
            cScanHoleStep = 4;
        }
    }
    else if (cScanHoleStep == 4)
    {
        if (!X_DRV)
        {
            _lxstopdelay = dwTickCount + 100;
            cScanHoleStep = 5;
        }
    }
    else if (cScanHoleStep == 5)
    {
        if (_lxstopdelay < dwTickCount)
        {
            StartFlag = 0;
            cScanHoleStep = 0;
        }
    }
}

/************************************************/
/*
����ѡ��
*/
/************************************************/
void CheckForOnePlug(void)
{
    long temp1 = 0;

    OneHolePlugFlag = 0;

    temp1 = endposition + CurWoodLength;
    temp1 = temp1 / 2;
    if (temp1 > Winfos[0])
    {
        OneHolePlugFlag = 1; // �ڿ�ǰ��
    }
    else
    {
        OneHolePlugFlag = 2; // �ڿ׺��
    }
}

char PlugBeforHolePro(void)
{
    char flag = 0;

    if (FactoryParam->ChaXiaoMode == 0)
    {
        return flag;
    }

    if (FactoryParam->ChaXiaoMode == PlugTWNei)
    {
        if (currentHoleNum < 2)
        {
            return flag;
        }

        if (currentWorkHoleNum == 0)
        {
            flag = 1;
        }
        else if ((CHAXIAO_Index > 1) && (CHAXIAO_Index != currentHoleNum))
        {
            if (currentWorkHoleNum != (CHAXIAO_Index - 1))
            {
                return flag;
            }

            if (bMiddleChaXiao)
            {
                flag = 1;
            }
        }
    }
    else if (FactoryParam->ChaXiaoMode == PlugTWWai)
    {
        if (currentHoleNum < 2)
        {
            return flag;
        }

        if (currentWorkHoleNum == (currentHoleNum - 1))
        {
            flag = 1;
        }
        else if ((CHAXIAO_Index > 1) && (CHAXIAO_Index != currentHoleNum))
        {
            if (currentWorkHoleNum != (CHAXIAO_Index - 1))
            {
                return flag;
            }

            if (bMiddleChaXiao)
            {
                flag = 1;
            }
        }
    }
    else if (FactoryParam->ChaXiaoMode == PlugBeforHole)
    {
        flag = 1;
    }
    else if (FactoryParam->ChaXiaoMode == PlugAll)
    {
        flag = 1;
    }
    else if (FactoryParam->ChaXiaoMode == PlugCustom)
    {
        WORD_BITS temp2;

        temp2.v[0] = cMidleCoil[6].Val;
        temp2.v[1] = cMidleCoil[7].Val;

        if ((temp2.Val) & (1 << currentWorkHoleNum))
        {
            flag = 1;
        }
    }
    else if (FactoryParam->ChaXiaoMode == PlugforOneHole)
    {
        if (currentHoleNum != 1)
        {
            return flag;
        }

        CheckForOnePlug();
        if (OneHolePlugFlag == 1)
        {
            flag = 1;
        }
    }
    else if (FactoryParam->ChaXiaoMode == PlugforTwoHole)
    {}

    return flag;
}

char PlugAfterHolePro(void)
{
    char flag = 0;

    if (FactoryParam->ChaXiaoMode == PlugTWWai)
    {
        if (currentHoleNum < 2)
        {
            return flag;
        }

        if (currentWorkHoleNum == 0)
        {
            flag = 1;
        }
        else if ((CHAXIAO_Index > 1) && (CHAXIAO_Index != currentHoleNum))
        {
            if (currentWorkHoleNum != (CHAXIAO_Index - 1))
            {
                return flag;
            }

            if (!bMiddleChaXiao)
            {
                flag = 1;
            }
        }
    }
    else if (FactoryParam->ChaXiaoMode == PlugTWNei)
    {
        if (currentHoleNum < 2)
        {
            return flag;
        }

        if (currentWorkHoleNum == (currentHoleNum - 1))
        {
            flag = 1;
        }
        else if ((CHAXIAO_Index > 1) && (CHAXIAO_Index != currentHoleNum))
        {
            if (currentWorkHoleNum != (CHAXIAO_Index - 1))
            {
                return flag;
            }

            if (!bMiddleChaXiao)
            {
                flag = 1;
            }
        }
    }
    else if (FactoryParam->ChaXiaoMode == PlugafterHole)
    {
        flag = 1;
    }
    else if (FactoryParam->ChaXiaoMode == PlugAll)
    {
        flag = 1;
    }
    else if (FactoryParam->ChaXiaoMode == PlugCustom)
    {
        WORD_BITS temp2;

        temp2.v[0] = cMidleCoil[8].Val;
        temp2.v[1] = cMidleCoil[9].Val;

        if (temp2.Val & (1 << currentWorkHoleNum))
        {
            flag = 1;
        }
    }
    else if (FactoryParam->ChaXiaoMode == PlugforOneHole)
    {
        if (currentHoleNum != 1)
        {
            return flag;
        }

        if (OneHolePlugFlag == 2)
        {
            flag = 1;
        }
    }
    else if (FactoryParam->ChaXiaoMode == PlugforTwoHole)
    {
        if (currentHoleNum != 2)
        {
            return flag;
        }

        if (currentWorkHoleNum == 1)
        {
            flag = 1;
        }
    }

    return flag;
}

long DrillHoleLocation = 0; // ���λ��
long DrillPlugLocation = 0; // �����λ��
void CalDrillLocation(void)
{
    long OverLcttmp; //,Datatmp;
                     // long disttemp = 0;
    if (SystemParam->XCalculate > 0)
    { // ������
        OverLcttmp = Winfos[currentWorkHoleNum];
        OverLcttmp += infos[currentWorkHoleNum];
        OverLcttmp /= 2;
        OverLcttmp += PositionToPluse(X_AXIS, FactoryParam->DrillOffset_Scan);
        DrillHoleLocation = OverLcttmp;
    }
    else
    { // �ױ���
        OverLcttmp = Winfos[currentWorkHoleNum];
        OverLcttmp += PositionToPluse(X_AXIS, FactoryParam->DrillOffset_Scan);
        OverLcttmp -= PositionToPluse(X_AXIS, 1500);
        DrillHoleLocation = OverLcttmp;
    }
}

void HoleDrillDepthProcess(long depth)
{
    if (FactoryParam->HoleDrillMode > 0)
    {
        if (depth > FactoryParam->HoleFeedDepth)
        {
            unsigned long Datatmp = 0;

            Datatmp = depth - HoleCurrrentDrillDepth;
            if (Datatmp > FactoryParam->HoleFeedDepth)
            {
                HoleCurrrentDrillDepth += FactoryParam->HoleFeedDepth;
            }
            else
            {
                HoleCurrrentDrillDepth += Datatmp;
            }
        }
        else
        {
            HoleCurrrentDrillDepth = depth;
        }
    }
    else
    {
        HoleCurrrentDrillDepth = depth;
    }
}

void DataCompare(void)
{ // ���ɨ�����ݵ�����ҳ��
    uint16 i = 0;
    uint16 j = 0;
    long temp = 0;
#if 1
    for (i = 0; i < 20; i++)
    {
        HolePos->PosB[i] = 0;
        HolePos->PosF[i] = 0;
        HolePos->Pos_Up[i] = 0;
        HolePos->Pos_Hole[i] = 0;
    }
#endif

    UserCurrentParam->Signal_Up = CheckHoleNum[0];
    UserCurrentParam->Signal_Down = CheckHoleNum[1];

    if (CheckHoleNum[0] > 20) j = 20;
    else j = CheckHoleNum[0];

    for (i = 0; i < j; i++)
    {
        temp = HoleinfoB[i] / XEncodeDivide;
        HolePos->PosB[i] = PluseToPosition(X_AXIS, temp);
    }

    if (CheckHoleNum[1] > 20) j = 20;
    else j = CheckHoleNum[1];
    for (i = 0; i < (j); i++)
    {
        temp = HoleinfoF[i] / XEncodeDivide;
        HolePos->PosF[i] = PluseToPosition(X_AXIS, temp);
    }

    j = CheckHoleNum[0];
    HolePos->Pos_Up[0] = HolePos->PosF[0] - HolePos->PosB[0];
    for (i = 1; i < (j); i++)
    {
        //HolePos->Pos_Up[i] = HolePos->PosB[i] - HolePos->PosB[i - 1];
        HolePos->Pos_Up[i] = HolePos->PosF[i] - HolePos->PosB[i];
        HolePos->Pos_Hole[i] = HolePos->PosB[i] - HolePos->PosF[i - 1];
    }

    for (i = 1; i < CheckHoleNum[0]; i++)
    {
        Winfos[i - 1] = HoleinfoB[i] / XEncodeDivide;
    }
    j = CheckHoleNum[1] - 1;
    for (i = 0; i < j; i++)
    {
        infos[i] = HoleinfoF[i] / XEncodeDivide;
    }
}

unsigned char CheckHoleInfo(void)
{
    unsigned char HoleNumber = 0;
    unsigned char WorkHoleNumber = 0;
    unsigned char i = 0;
    long _lHoleData[4] = { 0, 0, 0, 0 };
    //    long base = 0;

    //if(CheckHoleNum[0] == CheckHoleNum[1])
    {
        HoleNumber = CheckHoleNum[0];
    }
    //HoleNumber = currentHoleNum;
    CurWoodLength = HoleinfoB[0] / XEncodeDivide;
    endposition = HoleinfoF[CheckHoleNum[1] - 1] / XEncodeDivide;

    if (HoleNumber > 1)
    { // ��һ�������ذ�ͷ ���һ���½��Ӱ�β
        for (i = 1; i < HoleNumber; i++)
        {
            _lHoleData[0] = HoleinfoB[i] / XEncodeDivide;
            _lHoleData[1] = HoleinfoF[i - 1] / XEncodeDivide;
            _lHoleData[2] = _lHoleData[0] - _lHoleData[1];
            if (_lHoleData[2] >= TempDist.MinHoleR && _lHoleData[2] <= TempDist.MaxHoleR)
            {
                // _lHoleData[3] = _lHoleData[0] - XWorkOrignPos;
                // if (_lHoleData[3] < TempDist.XBuChangRange)
                // {
                //     if (TempDist.XBuChangBase > 0)
                //     {
                //         base = TempDist.XBuChangBase;
                //         _lHoleData[3] = TempDist.XBuChangRange - _lHoleData[3];
                //         _lHoleData[3] = _lHoleData[3] * base;
                //         _lHoleData[3] = _lHoleData[3] / TempDist.XBuChangRange;
                //         _lHoleData[0] = _lHoleData[0] + _lHoleData[3];
                //         _lHoleData[0] = _lHoleData[0] + base;
                //     }
                //     else
                //     {
                //         base = 0 - TempDist.XBuChangBase;
                //         _lHoleData[3] = TempDist.XBuChangRange - _lHoleData[3];
                //         _lHoleData[3] = _lHoleData[3] * base;
                //         _lHoleData[3] = _lHoleData[3] / TempDist.XBuChangRange;
                //         _lHoleData[0] = _lHoleData[0] + _lHoleData[3];
                //         _lHoleData[0] = _lHoleData[0] - base;
                //     }
                // }
                Winfos[WorkHoleNumber] = _lHoleData[0];
                infos[WorkHoleNumber] = _lHoleData[1];
                WorkHoleNumber++;
            }
        }
    }

    return WorkHoleNumber;
}

void ScanDrillHoleRun(void)
{
    long OverLcttmp; //,Datatmp;
    long disttemp = 0;

    if (cScanDrillRunStep == 1)
    {
        if (ToolValveOutDelay < dwTickCount)
        {
            VariableInit_Int();

            HoleCurrrentDrillDepth = 0;

            StartFlag = 1;
            cScanHoleStep = 1;
            MV_Set_Startv(X_AXIS, 3);
            MV_AccDec_Set(X_AXIS, 165, 100); //165);
            MV_Const_Move(X_AXIS, SpeedParam->CheckHoleSpeed2, 0);
            cScanDrillRunStep = 200;
        }
    }
    else if (cScanDrillRunStep == 200)
    { // ��ʱ���ж����ȡ�������ݣ�����ȴ���β�ź�
        ScanCheckHoleProc();
        if (cScanHoleStep == 0)
        { // ɨ�����
            bCuiQi = 0;
            StartFlag = 0;
            flgSgn = 0;
            flgSgnOld = 0;
            DataCompare();
            cScanDrillRunStep = 2;
        }
    }
    else if ((cScanDrillRunStep == 2))
    {
        currentHoleNum = CheckHoleInfo();
        CheckHoleNum[0] = 0;
        CheckHoleNum[1] = 0;

        if (currentHoleNum > 0)
        {
            currentWorkHoleNum = currentHoleNum - 1;
            cScanDrillRunStep = 11;
        }
        else
        {
            cScanDrillRunStep = 10;
        }
    }
    else if (cScanDrillRunStep == 11)
    {
        cScanDrillRunStep = 5;
        if (FactoryParam->ChaXiaoMode > 0)
        {
            char flag = 0;
            flag = PlugBeforHolePro();
            if (flag > 0)
            {
                cScanDrillRunStep = 3;
            }
        }
    }
    else if ((cScanDrillRunStep == 3))
    { // ͷβ�������
        if (!X_DRV)
        {
            CalDrillLocation();
            OverLcttmp = PositionToPluse(X_AXIS, FactoryParam->ChaXiaoDist);
            OverLcttmp += DrillHoleLocation;
            MoveAction_Pulse(X_AXIS, OverLcttmp, SpeedParam->XMoveSpeed);

            cScanDrillRunStep = 4;
            HoleCurrrentDrillDepth = 0;
        }
    }
    else if ((cScanDrillRunStep == 4))
    {
        if (!X_DRV && !Y_DRV)
        {
            cScanDrillRunStep = 40;
            cDrillRunStep = 1;
            HoleDrillDepthProcess(FactoryParam->ChaXiaoDepth);
        }
    }
    else if (cScanDrillRunStep == 40)
    { // ǰ����
        DrillingHoleProc(UserParam->YOrignDist_Left, HoleCurrrentDrillDepth);
        if (cDrillRunStep == 0)
        {
            if (HoleCurrrentDrillDepth >= FactoryParam->ChaXiaoDepth)
            {
                cScanDrillRunStep = 5;
                HoleCurrrentDrillDepth = 0;
            }
            else
            {
                cScanDrillRunStep = 4;
            }
        }
    }
    else if ((cScanDrillRunStep == 5))
    {
        if (!X_DRV && !Y_DRV)
        {
            CalDrillLocation();
            MoveAction_Pulse(X_AXIS, DrillHoleLocation, SpeedParam->XMoveSpeed);

            if (currentWorkHoleNum > 0)
            {
                OverLcttmp = Winfos[currentWorkHoleNum];
                OverLcttmp -= Winfos[currentWorkHoleNum];
                UserCurrentParam->HoleDist = PluseToPosition(X_AXIS, OverLcttmp);

                OverLcttmp = Winfos[currentWorkHoleNum];
                OverLcttmp -= infos[currentWorkHoleNum];
                UserCurrentParam->CheckHoleRadius = PluseToPosition(X_AXIS, OverLcttmp);
            }

            cDrillRunStep = 1;
            cScanDrillRunStep = 6;
            HoleCurrrentDrillDepth = 0;
        }
    }
    else if ((cScanDrillRunStep == 6))
    {
        if (!X_DRV && !Y_DRV)
        {
            HoleDrillDepthProcess(FactoryParam->HoleDepth);
            cScanDrillRunStep = 20;
        }
    }
    else if (cScanDrillRunStep == 20)
    { // ���
        DrillingHoleProc(UserParam->YOrignDist_Left, HoleCurrrentDrillDepth);
        if (cDrillRunStep == 0)
        {
            if (HoleCurrrentDrillDepth >= FactoryParam->HoleDepth)
            {
                cScanDrillRunStep = 12;
                HoleCurrrentDrillDepth = 0;
            }
            else
            {
                cDrillRunStep = 1;
                cScanDrillRunStep = 6;
            }
        }
    }
    else if (cScanDrillRunStep == 12)
    {
        cScanDrillRunStep = 9;

        if (FactoryParam->ChaXiaoMode > 0)
        {
            char flag = 0;
            flag = PlugAfterHolePro();
            if (flag > 0)
            {
                cScanDrillRunStep = 7;
            }
        }
    }
    else if ((cScanDrillRunStep == 7) && (!Y_DRV))
    { // �׺�
        if (FactoryParam->ChaXiaoMode == PlugforTwoHole)
        { // ֻ��������ͬʱ�ǵڶ�����ʱ��ִ�������,
          //�����˫���м������п״���2���򲻻�ִ�иò�
            if (SystemParam->XCalculate > 0)
            { // ������
                OverLcttmp = Winfos[1] + infos[1];
                disttemp = Winfos[0] + infos[0];
                disttemp = OverLcttmp - disttemp;
                disttemp = disttemp / 4;
            }
            else
            {
                disttemp = Winfos[1] - Winfos[0];
                disttemp = disttemp / 2;
            }
        }
        else
        { //
          //disttemp = TempDist.Dist32;
            disttemp = PositionToPluse(X_AXIS, FactoryParam->ChaXiaoDist);
        }

        OverLcttmp = DrillHoleLocation - disttemp;
        MoveAction_Pulse(X_AXIS, OverLcttmp, SpeedParam->XMoveSpeed);

        cDrillRunStep = 1;
        cScanDrillRunStep = 8;
        HoleCurrrentDrillDepth = 0;
    }
    else if ((cScanDrillRunStep == 8))
    { // ���Ǵ�׺�����ģ��������ﲻӦ�ó��ַ��򲹳���ص�
        if (!X_DRV && !Y_DRV)
        {
            HoleDrillDepthProcess(FactoryParam->ChaXiaoDepth);
            cScanDrillRunStep = 30;
        }
    }
    else if (cScanDrillRunStep == 30)
    {
        DrillingHoleProc(UserParam->YOrignDist_Left, HoleCurrrentDrillDepth);
        if (cDrillRunStep == 0)
        {
            if (HoleCurrrentDrillDepth >= FactoryParam->ChaXiaoDepth)
            {
                cScanDrillRunStep = 9;
                HoleCurrrentDrillDepth = 0;
            }
            else
            {
                cDrillRunStep = 1;
                cScanDrillRunStep = 8;
            }
        }
    }
    else if ((cScanDrillRunStep == 9))
    {
        if (currentWorkHoleNum > 0)
        {
            currentWorkHoleNum--;
            cScanDrillRunStep = 11;
        }
        else
        {
            cScanDrillRunStep = 10;
        }
    }
    else if (cScanDrillRunStep == 10)
    {
        cScanDrillRunStep = 0;
    }

    else if (cScanDrillRunStep == 13)
    {}
}
/************************************************/
/*
打孔动作函数
*/
/************************************************/
void DrillingHoleProc(long yoffset, long depth)
{
    long OverLcttmp = 0;
    static unsigned long BackDelay = 0;

    if ((cDrillRunStep == 1))
    {
        if (!(X_DRV || Y_DRV))
        {
            BackDelay = 10 * FactoryParam->XBackDelayTime;
            BackDelay += dwTickCount;
            cDrillRunStep = 2;

            if (FactoryParam->DrillwithBlow > 0)
            {
                bCuiQi = 1;
            }
        }
    }
    else if ((cDrillRunStep == 2))
    { //��ʼ���
        if (BackDelay < dwTickCount)
        {
            if (DelayTime2 < dwTickCount)
            {
                if (SystemParam->SystemMode != SYSTEM_TYPE0)
                {
                    OverLcttmp = yoffset;
                    OverLcttmp += depth;
                    OverLcttmp = PositionToPluse(Y_AXIS, OverLcttmp);
                    MoveAction_Pulse(Y_AXIS, OverLcttmp, SpeedParam->YDrillSpeed);
                }
                else
                {
                    bDrillValve = 1;
                }
                cDrillRunStep = 3;
            }
        }
    }
    else if ((cDrillRunStep == 3))
    {
        if (SystemParam->SystemMode != SYSTEM_TYPE0)
        {
            if (!Y_DRV)
            {
                DrillRunStepDelay = 10UL * FactoryParam->DrillCycleTime;
                DrillRunStepDelay += dwTickCount;
                cDrillRunStep = 4;
            }
        }
        else
        {
            if (bQiGangComeSgn)
            {
                DrillRunStepDelay = 10UL * FactoryParam->DrillCycleTime;
                DrillRunStepDelay += dwTickCount;
                cDrillRunStep = 4;
            }
        }
    }
    else if ((cDrillRunStep == 4))
    {
        if (DrillRunStepDelay < dwTickCount)
        {
            if (SystemParam->SystemMode != SYSTEM_TYPE0)
            {
                OverLcttmp = 0;
                MoveAction_Pulse(Y_AXIS, OverLcttmp, SpeedParam->YIdleSpeed);
            }
            else
            {
                bDrillValve = 0;
            }
            cDrillRunStep = 5;
        }
    }
    else if ((cDrillRunStep == 5))
    {
        if (SystemParam->SystemMode != SYSTEM_TYPE0)
        {
            if (!Y_DRV)
            {
                cDrillRunStep = 6;
                BackDelay = dwTickCount + 5000;
            }
        }
        else
        {
            if (bQiGangBackSgn)
            {
                BackDelay = 10UL * FactoryParam->QiGangReBackDelay;
                BackDelay += dwTickCount;
                cDrillRunStep = 7;
            }
        }
    }
    else if ((cDrillRunStep == 6))
    {
        if (Y_Orign)
        {
            cDrillRunStep = 8;
        }
        else
        { // ����
            if (BackDelay < dwTickCount)
            {
                AlarmNum = 15;
                bAlarmFlag = 1;
            }
        }
    }
    else if (cDrillRunStep == 7)
    { // ���������Ѿ������׺��Ʋ���λ�ı�����
        if (BackDelay < dwTickCount)
        {
            cDrillRunStep = 8;
        }
    }
    else if (cDrillRunStep == 8)
    {
        bCuiQi = 0;
        cDrillRunStep = 0;
        //bLeftToolValve = 0;
    }
}

/**
 * @brief  
 * @note   
 * @param  xstart: 
 * @param  xstop: 
 * @param  yorign: 
 * @param  ydepth: 
 * @retval None
 */
void DrillingSlotProc(SlotWorkData_str *pdata)
{
    static unsigned long delaytick = 0;
    //static unsigned short CurDepth = 0;
    long OverLcttmp = 0;

    if (pdata->ydepth <= 0)
    {
        cDrillRunStep = 0;
    }

    if (cDrillRunStep == 1)
    {
        if ((X_DRV || Y_DRV))
        {
            return;
        }

        if (SlotTypeFlag == 1)
        { // 头尾定长段槽需要交换头尾
            OverLcttmp = pdata->xstop;
            pdata->xstop = pdata->xstart;
            pdata->xstart = OverLcttmp;

            OverLcttmp = pdata->xstop;
            OverLcttmp += pdata->xstart;
            OverLcttmp /= 2;
        }
        else if (SlotTypeFlag == 2)
        { //
            OverLcttmp = pdata->xstop;
            OverLcttmp += pdata->xstart;
            OverLcttmp /= 2;
        }
        else
        {
            //SlotTypeFlag = 3;
            OverLcttmp = pdata->xstart;
        }

        MoveAction_Pulse(X_AXIS, OverLcttmp, SpeedParam->XMoveSpeed);
        cDrillRunStep = 2;
    }
    else if (cDrillRunStep == 2)
    {
        if ((X_DRV || Y_DRV))
        {
            delaytick = 10 * FactoryParam->XBackDelayTime;
            delaytick += dwTickCount;
            return;
        }
        if (delaytick < dwTickCount)
        {
            cDrillRunStep = 3;
        }
    }
    else if (cDrillRunStep == 3)
    {
        if (ToolValveOutDelay > dwTickCount)
        {
            return;
        }
        if (DelayTime2 > dwTickCount)
        {
            return;
        }

        OverLcttmp = PositionToPluse(Y_AXIS, pdata->yorign);
        MoveAction_Pulse(Y_AXIS, OverLcttmp, SpeedParam->YIdleSpeed);
        cDrillRunStep = 30;
        CurDepth = 0;
        cCaoMoveDir = 0;
    }
    else if (cDrillRunStep == 30)
    {
        if (SlotTypeFlag)
        {
            cDrillRunStep = 31;
        }
        else
        {
            cDrillRunStep = 4;
        }
    }
    else if (cDrillRunStep == 31)
    {
        if (Y_DRV)
        {
            return;
        }

        OverLcttmp = pdata->yorign + UserParam->KSlotPicDepth;
        OverLcttmp = PositionToPluse(Y_AXIS, OverLcttmp);
        MoveAction_Pulse(Y_AXIS, OverLcttmp, SpeedParam->YDrillSpeed);
        cDrillRunStep = 32;
    }
    else if (cDrillRunStep == 32)
    {
        if (Y_DRV)
        {
            delaytick = 10 * FactoryParam->DrillCycleTime;
            delaytick += dwTickCount;

            return;
        }

        if (delaytick < dwTickCount)
        {
            OverLcttmp = SlotWorkData.xstart;
            MoveAction_Pulse(X_AXIS, OverLcttmp, SpeedParam->XSlotSpeed);
            cDrillRunStep = 4;
        }
    }
    else if (cDrillRunStep == 4)
    {
        if (Y_DRV || X_DRV)
        {
            delaytick = dwTickCount + 100;
            return;
        }

        if (delaytick < dwTickCount)
        {
            cDrillRunStep = 10;

            DepthCal(pdata->ydepth, FactoryParam->CaoPicDepth);
            OverLcttmp = CurDepth;

            if (CurDepth > 0)
            {
                OverLcttmp += pdata->yorign;
                cDrillRunStep = 5;

                OverLcttmp = PositionToPluse(Y_AXIS, OverLcttmp);
                MoveAction_Pulse(Y_AXIS, OverLcttmp, SpeedParam->YDrillSpeed);
            }
        }
    }
    else if (cDrillRunStep == 5)
    {
        uint8 flag = 0;

        if (SystemParam->SystemMode > 0)
        {
            if (!Y_DRV)
            {
                flag = 1;
            }
        }
        else
        {
            if (bQiGangComeSgn)
            {
                flag = 1;
            }
        }

        if (flag > 0)
        {
            if (delaytick < dwTickCount)
            {
                cDrillRunStep = 6;
            }
        }
        else
        {
            delaytick = 10 * FactoryParam->DrillCycleTime;
            delaytick += dwTickCount;
        }
    }
    else if (cDrillRunStep == 6)
    {
        DuanCaoXMoveAction(pdata->xstart, pdata->xstop);
        cDrillRunStep = 7;
    }
    else if (cDrillRunStep == 7)
    {
        if (!X_DRV)
        {
            if (delaytick < dwTickCount)
            {
                cDrillRunStep = 8;
            }
        }
        else
        {
            delaytick = 10 * FactoryParam->XBackDelayTime;
            delaytick += dwTickCount;
        }
    }
    else if (cDrillRunStep == 8)
    {
        if (SystemParam->SystemMode == SYSTEM_TYPE0)
        {
            cDrillRunStep = 9;
        }
        else
        {
            cDrillRunStep = 4;

            if (duancaodirflag == 1)
            {
                OverLcttmp = FactoryParam->SlowMotorBuchang;
                OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
                tXAxisStepper.cRealPosi += OverLcttmp;
            }
            else if (duancaodirflag == 2)
            {
                OverLcttmp = FactoryParam->SlowMotorBuchang;
                OverLcttmp = PositionToPluse(X_AXIS, OverLcttmp);
                tXAxisStepper.cRealPosi -= OverLcttmp;
            }
        }
    }
    else if (cDrillRunStep == 9)
    {
        if (SystemParam->SystemMode != SYSTEM_TYPE0)
        {
            if (!Y_DRV)
            {
                cDrillRunStep = 10;
            }
        }
        else
        {
            bDrillValve = 0;
            cDrillRunStep = 10;
        }
    }
    else if (cDrillRunStep == 10)
    { //�˵�
        if (X_DRV || Y_DRV)
        {
            return;
        }
        if (SlotTypeFlag)
        {
            OverLcttmp = SlotWorkData.xstart;
            MoveAction_Pulse(X_AXIS, OverLcttmp, SpeedParam->XSlotSpeed);
        }
        cDrillRunStep = 12;
    }
    else if (cDrillRunStep == 12)
    {
        if (X_DRV || Y_DRV)
        {
            return;
        }

        MoveAction_Pulse(Y_AXIS, 0, SpeedParam->YIdleSpeed);
        cDrillRunStep = 13;
    }
    else if (cDrillRunStep == 13)
    {
        if (SystemParam->SystemMode != SYSTEM_TYPE0)
        {
            if (Y_DRV)
            {
                return;
            }
            if (Y_Orign)
            {
                cDrillRunStep = 11;
            }
        }
        else
        {
            if (bQiGangBackSgn)
            {
                if (delaytick < dwTickCount)
                {
                    cDingHoleWorkStep = 11;
                }
            }
            else
            {
                delaytick = 10 * FactoryParam->QiGangReBackDelay;
                delaytick += dwTickCount;
            }
        }
    }
    else if (cDrillRunStep == 11)
    {
        cDrillRunStep = 0;
    }
}
