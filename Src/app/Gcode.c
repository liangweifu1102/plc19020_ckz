/*
 * @Description: 
 * @Author: ccc
 * @Date: 2019-07-05 16:47:21
 * @LastEditTime: 2019-07-05 16:47:21
 * @LastEditors: ccc
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "usermemory.h"
#include "Gcode.h"
#include "touch.h"
//#include "../hardware/serverdrv.h"
#include "../hardware/servostep.h"
#include "../usbapp/usb_host.h"
#include "AutoRun.h"

static const int multiTable[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 100000000, 1000000000 };

GcodeA_str GCodeRunParam;
static long CurToolOffsetX = 0;
static long CurToolOffsetY = 0;
/**
 * G代码解析初始化
 * 
 * @author xt (2019/7/3)
 * 
 * @param void 
 */
void DecodeInitProc(void)
{
    GCodeRunParam.CmdNum = 0;
    GCodeRunParam.CmdType = 0;

    GCodeRunParam.xRefresh = 0;
    GCodeRunParam.yRefresh = 0;
    GCodeRunParam.zRefresh = 0;

    GCodeRunParam.xpos = 0;
    GCodeRunParam.ypos = 0;
    GCodeRunParam.zpos = 0;

    GCodeRunParam.cGRunStep = 0;
    GCodeRunParam.cTRunStep = 0;
}

/************************************************/
/*
获取G代码行数
*/
/************************************************/
int GCodeLineNumber(uint8 *GCode)
{
    int i, sum = 0;
    for (i = 0; i < strlen((char *)GCode); i++)
    {
        if (GCode[i] == '\n') sum++;
    }
    return sum;
}

/************************************************/
/*
从文件中读取当前行

*buffer  : 所有G代码长度
offsetyy   : 减去偏置长度
*dst     : 得到当前行的代码
length   : 当前行代码的长度
*/
/************************************************/
int GetLine(uint8 *buffer, uint32 offset, uint8 *dst)
{
    char bufftmp[100];
    int length = 0;
    buffer += offset;
    memcpy(bufftmp, buffer, 100);
    while (bufftmp[length] != '\0' && bufftmp[length] != '\n')
    {
        *dst++ = bufftmp[length++];
    }
    if (buffer[0] == '\0')
    {
        length = 0;
    }
    else
    {
        length++;
    }
    *dst = '\0';
    return length;
}

unsigned char slot_flg_step=0;

/**
 * �析当前行数据
 * 
 * @param step
 * 
 * @return 
 */
int GCodeLineDecode(char *step)
{
    double valueD = 0.0; //浮点参数
    int valueI = 0;      //整数参数

    int numberBit = 0;
    // char actionFlg = 1; //设置位置标志是否出现
    int index = 0;
    char byte;
    //无效指令 开头不是字母
    if (step[0] != ' ' && !(step[0] >= 'a' && step[0] <= 'z' || step[0] >= 'A' && step[0] <= 'Z')) return 1;
    //初始化当前的位置参数

    index = strlen(step);
    if (index == 0) return 1;

    while (index > 0)
    {
        index--;
        byte = step[index];
        switch (byte)
        {
            //步数描述
        case 'N':
            break;
            //不解析多余的符号
        case ' ':
        case ',':
        case ';':
            break;
            //数值读取部分
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            valueD += multiTable[numberBit] * (step[index] - '0');
            valueI = (int)valueD;
            numberBit++;
            break;
        case '-':
            valueD = -valueD;
            valueI = (int)valueD;
            break;
        case '.':
            valueD /= multiTable[numberBit];
            valueI = (int)valueD;
            numberBit = 0;
            break;
            //几个关键坐标轴
        case 'X':
            GCodeRunParam.xpos = (int)(valueD * 1000 / 10);
            GCodeRunParam.xRefresh = 1;
            break;
        case 'Y':
            GCodeRunParam.ypos = (int)(valueD * 1000 / 10);
            GCodeRunParam.yRefresh = 1;
            break;
        case 'Z':
            GCodeRunParam.zpos = (int)(valueD * 1000 / 10);
            GCodeRunParam.zRefresh = 1;
            break;
        case 'D':
            //                  curTempPara.D = valueI;
            //actionFlg = 0;
            break;
            //刀具补偿地址
        case 'H':
            //                  curTempPara.H = valueI;
            //actionFlg = 0;
            break;
        case 'P':
            //                  curTempPara.P = valueD;
            //actionFlg = 0;
            break;
            //M码指令
        case 'G':
            GCodeRunParam.CmdType = 'G';
            if (valueI == 90)
            {
            }
            else
            {
                GCodeRunParam.CmdNum = valueI;
                if (slot_flg_step == 0)
                {
                    if (valueI==0) //G00
                    {
                        slot_flg_step=1;
                    }
                }
                else if(slot_flg_step==1)
                {
                    if (valueI==0)
                    {
                        slot_flg_step=0;
                    }
                    else if (valueI ==1 )
                    {
                        slot_flg_step=2;  //G01
                    }
                }
                else if (slot_flg_step==2)
                {
                    if (valueI==0)
                    {
                        slot_flg_step=0;
                    }
                    else if(valueI == 1)
                    {
                        slot_flg_step=3;  //G01
                    }
                } 
            }
            break;
        case 'M':
            //执行M码
            GCodeRunParam.CmdType = 'M';
            GCodeRunParam.CmdNum = valueI;
            break;
        case 'T':
            //执行M码
            slot_flg_step=0;

            GCodeRunParam.CmdType = 'T';
            GCodeRunParam.CmdNum = valueI;
            break;
        default:
            break;
        }
        //如果是数字部分，就不需要参数清零，不然是需要清零的
        if ((step[index] >= '0' && step[index] <= '9') || step[index] == '-' || step[index] == '.')
        {
            continue;
        }

        valueI = 0;
        valueD = 0;
        numberBit = 0;
    } //

    return -1;
}
/**
 * 获取位置数据
 * 
 * @author zhang (2019/9/18)
 * 
 * @param void 
 * 
 * @return int32 
 */
int32 getPosMove(void)
{
    if (FactoryParam->Axis_Position_Flag == X_AxisConfigFlag)
    {
        if (GCodeRunParam.xRefresh > 0)
        {
            GCodeRunParam.xRefresh = 0;
            return GCodeRunParam.xpos;
        }
    }
    else if (FactoryParam->Axis_Position_Flag == Y_AxisConfigFlag)
    {
        if (GCodeRunParam.yRefresh > 0)
        {
            GCodeRunParam.yRefresh = 0;
            return GCodeRunParam.ypos;
        }
    }
    else if (FactoryParam->Axis_Position_Flag == Z_AxisConfigFlag)
    {
        if (GCodeRunParam.zRefresh > 0)
        {
            GCodeRunParam.zRefresh = 0;
            return GCodeRunParam.zpos;
        }
    }

    return 0;
}
/**
 * 获取深度数据
 * 
 * @author zhang (2019/9/18)
 * 
 * @param void 
 * 
 * @return int32 
 */
int32 getDepthMove(void)
{
    if (FactoryParam->Axis_Depth_Flag == X_AxisConfigFlag)
    {
        if (GCodeRunParam.xRefresh > 0)
        {
            GCodeRunParam.xRefresh = 0;
            return GCodeRunParam.xpos;
        }
    }
    else if (FactoryParam->Axis_Depth_Flag == Y_AxisConfigFlag)
    {
        if (GCodeRunParam.yRefresh > 0)
        {
            GCodeRunParam.yRefresh = 0;
            return GCodeRunParam.ypos;
        }
    }
    else if (FactoryParam->Axis_Depth_Flag == Z_AxisConfigFlag)
    {
        if (GCodeRunParam.zRefresh > 0)
        {
            GCodeRunParam.zRefresh = 0;
            return GCodeRunParam.zpos;
        }
    }

    return 0;
}

/**
 * G代码解析后执行
 * 每个轴行走位置是相对板头位置的绝对位置
 * @author xt (2019/7/2)
 * 
 * @param void 
 */
void GCode_G_Proc(void)
{
    uint8 axis = 0;
    uint16 startv = 0;
    uint16 speed = 0;
    int32 pos = 0;

    switch (GCodeRunParam.cGRunStep)
    {
    case 1:
        if (!X_DRV && !Y_DRV)
        {
            GCodeRunParam.cGRunStep = 2;
        }
        break;
    case 2:
        if (GCodeRunParam.yRefresh == 1)
        {
            axis = Y_AXIS;
            startv = STARTSPEED_3;

            if (GCodeRunParam.CmdNum == 0)
            { //g0
                speed = SpeedParam->YIdleSpeed;
            }
            else if (GCodeRunParam.CmdNum == 1)
            { //g1
                speed = SpeedParam->YDrillSpeed;
            }
            pos = getDepthMove();
            if (pos != 0)
            {
                pos += CurToolOffsetY;
            }
            pos = PositionToPluse(axis, pos);
            SetPulse_Y(startv, speed, pos);
        }
        GCodeRunParam.cGRunStep = 3;
        break;
    case 3:
        if (!X_DRV && !Y_DRV)
        {
            GCodeRunParam.cGRunStep = 4;
        }
        break;
    case 4:
        if (GCodeRunParam.xRefresh == 1)
        {
            axis = X_AXIS;
            startv = STARTSPEED_0;

            if (GCodeRunParam.CmdNum == 0)
            { //g0
                startv = STARTSPEED_3;
                speed = SpeedParam->XMoveSpeed;
                if (speed < startv)
                {
                    speed = startv;
                }
            }
            else if (GCodeRunParam.CmdNum == 1)
            { //g1
                startv = STARTSPEED_0;
                speed = SpeedParam->XSlotSpeed;
            }
            if (slot_flg_step>=3)
            {
                pos = getPosMove();
                pos = PositionToPluse(X_AXIS, pos);
                pos = tXAxisStepper.cRealPosi + pos;
                MoveAction_Pulse(X_AXIS, pos, speed);

                slot_flg_step=0;
            }
            else
            {
                pos = getPosMove();
                pos += CurToolOffsetX;
                pos = PositionToPluse(axis, pos);
                pos += ulBoardHead;
                SetPulse_X(startv, speed, pos);
            }
        }
        GCodeRunParam.cGRunStep = 5;
        break;
    case 5:
        if (!X_DRV && !Y_DRV)
        {
            GCodeRunParam.cGRunStep = 6;
        }
        break;
    case 6:
        GCodeRunParam.cGRunStep = 0;
        break;
    default:
        break;
    }
}

/**
 * M码执行
 * 
 * @author xt (2019/7/2)
 * 
 * @param void 
 */
void GCode_M_Proc(void)
{
    switch (GCodeRunParam.CmdNum)
    {
    case 5:
        GCodeRunParam.StartFlag = 0;
        GCodeRunParam.M05Flag = 1;
        break;
    case 30:
        GCodeRunParam.StartFlag = 0;
        GCodeRunParam.M30Flag = 1;
        break;
    case 601:
        GCodeRunParam.StartFlag = 1;
        GCodeRunParam.M601Flag = 1;
        GCodeRunParam.M602Flag = 0;
        GCodeRunParam.M603Flag = 0;
        GCodeRunParam.M604Flag = 0;
        break;
    case 602:
        GCodeRunParam.StartFlag = 1;
        GCodeRunParam.M601Flag = 0;
        GCodeRunParam.M602Flag = 1;
        GCodeRunParam.M603Flag = 0;
        GCodeRunParam.M604Flag = 0;
        break;
    case 603:
        GCodeRunParam.StartFlag = 1;
        GCodeRunParam.M601Flag = 0;
        GCodeRunParam.M602Flag = 0;
        GCodeRunParam.M603Flag = 1;
        GCodeRunParam.M604Flag = 0;
        break;
    case 604:
        GCodeRunParam.StartFlag = 1;
        GCodeRunParam.M601Flag = 0;
        GCodeRunParam.M602Flag = 0;
        GCodeRunParam.M603Flag = 0;
        GCodeRunParam.M604Flag = 1;
        break;
    default:
        break;
    }

    if (GCodeRunParam.M601Flag == 1)
    {
        UserCurrentParam->TopFlag = 2;
    }
    else if (GCodeRunParam.M602Flag == 1)
    {
        UserCurrentParam->RightFlag = 2;
    }
    else if (GCodeRunParam.M603Flag == 1)
    {
        UserCurrentParam->BottomFlag = 2;
    }
    else if (GCodeRunParam.M604Flag == 1)
    {
        UserCurrentParam->LeftFlag = 2;
    }
}
/**
 * 出刀及换刀
 * 
 * @author xt (2019/7/2)
 * 
 * @param void 
 */
void GCode_T_Proc(void)
{
    if (GCodeRunParam.cTRunStep == 1)
    {
        switch (GCodeRunParam.CmdNum)
        {
        case 1:
            if (SystemParam->SystemMode == SYSTEM_TYPE4)
            {
                LeftToolOutProc();
                GCodeRunParam.cTRunStep = 3;
            }
            else if (SystemParam->SystemMode == SYSTEM_TYPE3)
            {
                cToolOutFlag = TOOLSELECT_L;
                MotorCoilSelectStep = 1;
                GCodeRunParam.cTRunStep = 2;
            }
            else if (SystemParam->SystemMode == SYSTEM_TYPE2)
            {
                LeftToolOutProc();
                GCodeRunParam.cTRunStep = 3;
            }
            else if (SystemParam->SystemMode == SYSTEM_TYPE1)
            {
                LeftToolOutProc();
                GCodeRunParam.cTRunStep = 3;
            }
            CurToolOffsetX = FactoryParam->ToolOffSet_Code_L;
            CurToolOffsetY = FactoryParam->YOrignDist_L;
            break;
        case 2:
            if (SystemParam->SystemMode == SYSTEM_TYPE4)
            {
                RightToolOutProc();
                GCodeRunParam.cTRunStep = 3;
            }
            else if (SystemParam->SystemMode == SYSTEM_TYPE3)
            {
                cToolOutFlag = TOOLSELECT_R;
                MotorCoilSelectStep = 1;
                GCodeRunParam.cTRunStep = 2;
            }
            else if (SystemParam->SystemMode == SYSTEM_TYPE2)
            {
                RightToolOutProc();
                GCodeRunParam.cTRunStep = 3;
            }
            else if (SystemParam->SystemMode == SYSTEM_TYPE1)
            {
                LeftToolOutProc();
                GCodeRunParam.cTRunStep = 3;
            }

            CurToolOffsetX = FactoryParam->ToolOffSet_Code_R;
            CurToolOffsetY = FactoryParam->YOrignDist_R;
            break;
        case 3:
            break;
        case 4:
            break;
        default:
            break;
        }
    }
    else if (GCodeRunParam.cTRunStep == 2)
    {
        MotorCoilSelect();
        if (MotorCoilSelectStep == 0)
        {
            GCodeRunParam.cTRunStep = 3;
        }
    }
    else if (GCodeRunParam.cTRunStep == 3)
    {
        if (MotorStartDelay < dwTickCount)
        {
            GCodeRunParam.cTRunStep = 0;
        }
    }
}

static uint32 offsetRead = 0; //当前行到开头的偏置，拷贝时要减去
void SetGcodeRunStart(void)
{
    if (GCodeRunParam.M05Flag == 1)
    {
        GCodeRunParam.M05Flag = 0;
        GCodeRunParam.cRunStep = 2;
    }
    else
    {
        GCodeRunParam.cRunStep = 1;
    }
}

void ClrGcodeRunOffset(void)
{
    offsetRead = 0;
}

void SetGcodeOver(void)
{
    GCodeRunParam.M30Flag = 1;
}

void ClrGcodeOver(void)
{
    GCodeRunParam.M30Flag = 0;
    GCodeRunParam.M05Flag = 0;
    offsetRead = 0;
}

uint8 GetGcodeOver(void)
{
    return GCodeRunParam.M30Flag;
}

uint8 IsGcodeRunOver(void)
{
    if (GCodeRunParam.cRunStep == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void SetNextWorkFlag(void)
{
    if (UserCurrentParam->TopFlag  == 1)
    {
        UserCurrentParam->TopFlag = 2;
    }
    else if (UserCurrentParam->RightFlag  == 1)
    {
        UserCurrentParam->RightFlag = 2;
    }
    else if (UserCurrentParam->BottomFlag  == 1)
    {
        UserCurrentParam->BottomFlag = 2;
    }
    else if (UserCurrentParam->LeftFlag  == 1)
    {
        UserCurrentParam->LeftFlag = 2;
    }
}

void GcodeRunOverProc(void)
{
    // 加工完标示

    if (UserCurrentParam->TopFlag  == 2)
    {
        UserCurrentParam->TopFlag = 3;
    }
    else if (UserCurrentParam->RightFlag  == 2)
    {
        UserCurrentParam->RightFlag = 3;
    }
    else if (UserCurrentParam->BottomFlag  == 2)
    {
        UserCurrentParam->BottomFlag = 3;
    }
    else if (UserCurrentParam->LeftFlag  == 2)
    {
        UserCurrentParam->LeftFlag = 3;
    }

    if ((UserCurrentParam->TopFlag == 1 || UserCurrentParam->TopFlag == 2) ||
        (UserCurrentParam->RightFlag == 1 || UserCurrentParam->RightFlag == 2) ||
        (UserCurrentParam->BottomFlag == 1 || UserCurrentParam->BottomFlag == 2) ||
        (UserCurrentParam->LeftFlag == 1 || UserCurrentParam->LeftFlag == 2))
    {
    }
    else
    {
        GCodeRunParam.M30Flag = 1;
    }

    if (GCodeRunParam.M30Flag == 1)
    {
        GCodeRunParam.M05Flag = 0;
        UserCurrentParam->TopFlag = 3;
        UserCurrentParam->RightFlag = 3;
        UserCurrentParam->BottomFlag = 3;
        UserCurrentParam->LeftFlag = 3;

        CLRBIT(DISP_READFILE_OK);
    }
    else
    {
        SetNextWorkFlag();
    }
}

/**
 * 
 */
void GCodeRunProc(void)
{
    static uint8 gline[100] = { 0 };
    static uint8 *gcodebuff;
    uint32 lengthRead;

    UserCurrentParam->debugdata3 = GCodeRunParam.cRunStep;

    if (GCodeRunParam.cRunStep == 0)
    {
        return;
    }

    switch (GCodeRunParam.cRunStep)
    {
    case 1:
        offsetRead = 0; //每新读一个G代码文件都要清零，从头开始
        gcodebuff = getGcodeSrcBuff();

        GCodeRunParam.M601Flag = 0;
        GCodeRunParam.M602Flag = 0;
        GCodeRunParam.M603Flag = 0;
        GCodeRunParam.M604Flag = 0;
        GCodeRunParam.M30Flag = 0;
        GCodeRunParam.M05Flag = 0;
        GCodeRunParam.StartFlag = 0;

        GCodeRunParam.cRunStep = 2;
        break;
    case 2:
        DecodeInitProc();
        GCodeRunParam.cRunStep = 3;
        break;
    case 3:
        lengthRead = GetLine(gcodebuff, offsetRead, gline);
        offsetRead += lengthRead;
        GCodeRunParam.cRunStep = 4;
//      if (lengthRead == 0)
//      {
//          GCodeRunParam.cRunStep = 31;
//          break;
//      }
        break;
    case 4:
        GCodeLineDecode((char *)gline); //分析一行
        GCodeRunParam.cRunStep = 5;
        break;
    case 5:
        if (GCodeRunParam.CmdType == 'G')
        {
            GCodeRunParam.cGRunStep = 1;
            GCodeRunParam.cRunStep = 6;
        }
        else if (GCodeRunParam.CmdType == 'M')
        {
            GCodeRunParam.cRunStep = 7;
        }
        else if (GCodeRunParam.CmdType == 'T')
        {
            GCodeRunParam.cTRunStep = 1;
            GCodeRunParam.cRunStep = 8;
        }
        else
        {
            GCodeRunParam.cRunStep = 20;
        }
        break;
    case 6:
        GCode_G_Proc();
        if (GCodeRunParam.cGRunStep == 0)
        {
            GCodeRunParam.cRunStep = 20;
        }
        break;
    case 7:
        GCode_M_Proc();
        if (GCodeRunParam.StartFlag == 1)
        {
            GCodeRunParam.cRunStep = 20;
        }
        else
        {
            GCodeRunParam.cRunStep = 31;
        }

//      if (GCodeRunParam.M30Flag > 0)
//      {
//          offsetRead = 0;
//      }
        break;
    case 8:
        GCode_T_Proc();
        if (GCodeRunParam.cTRunStep == 0)
        {
            GCodeRunParam.cRunStep = 20;
        }
        break;
    case 20:
        GCodeRunParam.CmdNum = 0;
        GCodeRunParam.CmdType = 0;
        GCodeRunParam.xpos = 0;
        GCodeRunParam.ypos = 0;
        GCodeRunParam.zpos = 0;
        GCodeRunParam.cRunStep = 30;
        break;
    case 30:
        GCodeRunParam.cRunStep = 2;
        break;
    case 31:
        bLeftToolValve = 0;
        bRightToolValve = 0;
        //bLeftToolMotor = 0;
        //bRightToolMotor = 0;

        GcodeRunOverProc();

        GCodeRunParam.cRunStep = 0;
        break;
    }
}

void GCodePreRead_T(uint8 *buf)
{

    switch (*buf)
    {
    case '1':
        UserCurrentParam->Tool1OutFlag = 1;
        buf++;
        break;
    case '2':
        UserCurrentParam->Tool2OutFlag = 1;
        buf++;
        break;
    default:
        break;
    }
}

void GCodePreRead_M(uint8 *buf)
{
    if (*buf != '6')
    {
        return;
    }
    buf++;
    if (*buf != '0')
    {
        return;
    }
    buf++;

    switch (*buf)
    {
    case '1':
        UserCurrentParam->TopFlag = 1;
        break;
    case '2':
        UserCurrentParam->RightFlag = 1;
        break;
    case '3':
        UserCurrentParam->BottomFlag = 1;
        break;
    case '4':
        UserCurrentParam->LeftFlag = 1;
        break;
    default:
        break;
    }

    buf++;
}

void CLearWorkFlag(void)
{
    UserCurrentParam->TopFlag = 0;
    UserCurrentParam->RightFlag = 0;
    UserCurrentParam->BottomFlag = 0;
    UserCurrentParam->LeftFlag = 0;
}

/**
  * @brief  G代码预读
  * @note   
  * @param  *buf: 
  * @retval None
  */
void GCodePreRead(uint8 *buf)
{
    long OverLcttmp;

    while (*buf != 0)
    {
//      if (*buf == 'T' || *buf == 't')
//      {
//          buf++;
//          GCodePreRead_T(buf);
//      }
//      else
        if (*buf == 'M' || *buf == 'm')
        {
            buf++;
            GCodePreRead_M(buf);
        }
        else
        {
            buf++;
        }
    }

    SetNextWorkFlag();

    OverLcttmp = UserCurrentParam->TopFlag;
    OverLcttmp += UserCurrentParam->RightFlag;
    OverLcttmp += UserCurrentParam->BottomFlag;
    OverLcttmp += UserCurrentParam->LeftFlag;

    if (OverLcttmp == 0)
    {
        AlarmNum = 19;
        bAlarmFlag = 1;
    }
    else
    {
        ClrGcodeOver();
        if (AlarmNum == 19) AlarmNum = 0;
    }
}
