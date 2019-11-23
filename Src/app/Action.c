/*
 * @Author: ccc 
 * @Date: 2019-07-26 18:22:41 
 * @Last Modified by:   ccc 
 * @Last Modified time: 2019-07-26 18:22:41 
 */

#include "config.h"
#include "touch.h"

double XGearRatio, YGearRatio;
unsigned short XEncodeDivide; //���Ӵ��ֱ�

WORD_BITS cAxisRunState;

unsigned char cResumeStep;                //���鲽��
uint16 cChaXiaoBackup = 0;                //

unsigned char RebackXYFLG; //��һ���ϵ��־

unsigned short RstPphlDlay; //��λ������ʱ

unsigned long Press1time=0;
unsigned long Press2time=0;
unsigned long Press3time=0;
unsigned long Press4time=0;

bool KeyPress1Flag = 0;
bool KeyPress2Flag = 0;
bool KeyPress3Flag = 0;
bool KeyPress4Flag = 0;

void ReadRobotCurrent(void);
extern void KeyBoardProc(void);
void RebackXYOrign(void);
extern void ResumeLocation(void);
void SaveAndLoadParams(void);
extern void AlarmProtect(void);

// 根据距离计算脉冲
long PositionToPluse(unsigned short cAxisNo, long dist)
{
    double PulseNumber = 0;

    if (cAxisNo == Y_AXIS)
    { // ���
        PulseNumber = ((double)dist * YGearRatio);
    }
    else if (cAxisNo == X_AXIS)
    { // λ��
        PulseNumber = ((double)dist * XGearRatio);
    }

    if (dist > 0)
    {
        PulseNumber += 0.5;
    }
    else
    {
        PulseNumber -= 0.5;
    }

    return (long)PulseNumber;
}

// 根据脉冲计算距离
long PluseToPosition(unsigned short cAxisNo, long pulse)
{
    double dist = 0;

    if (cAxisNo == Y_AXIS)
    { // ���
        dist = ((double)pulse / YGearRatio);
    }
    else if (cAxisNo == X_AXIS)
    { // λ��
        dist = ((double)pulse / XGearRatio);
    }

    if (pulse > 0)
    {
        dist += 0.5;
    }
    else
    {
        dist -= 0.5;
    }

    return (long)dist;
}

/************************************************/
/*
��ǰ����״̬

*/
/************************************************/
extern  unsigned short NeedWorkNum ;

void ReadRobotCurrent(void)
{
    long OverLcttmp;

    ReadAxisStatus();

    UserCurrentParam->ulEncoderCount = getEncoderCount();

    OverLcttmp = MV_Get_Command_Pos(X_AXIS);
    SysCurrentParam->XDistance = PluseToPosition(X_AXIS, OverLcttmp);

    if (SystemParam->SystemMode > 0)
    {
        OverLcttmp = MV_Get_Command_Pos(Y_AXIS);
        SysCurrentParam->YDistance = PluseToPosition(Y_AXIS, OverLcttmp);
    }

    UserCurrentParam->DrillNum = NeedWorkNum;
}

/************************************************/
/*
��ֹͣλʱ�����踴λ


*/
/************************************************/
void ResetPeripheral(void)
{
    RstPphlDlay = 500;
}

/************************************************/
/*
������ֱȼ��ٶȱ���
*/
/************************************************/
void CalGearRatio(void)
{
    double gearratiotmp;
    gearratiotmp = (double)SystemParam->XCycleLength; // ��λС��
    gearratiotmp /= 10;                               // һ���������붼����λС����
    gearratiotmp = SystemParam->XCyclePulse / gearratiotmp;
    XGearRatio = gearratiotmp; // X ��

    XEncodeDivide = 10000 / SystemParam->XCyclePulse;

    gearratiotmp = (double)SystemParam->YCycleLength;
    gearratiotmp /= 10;
    gearratiotmp = SystemParam->YCyclePulse / gearratiotmp;
    YGearRatio = gearratiotmp; // Y��

    if (XGearRatio == 0.0)
        XGearRatio = 10.0;
    if (YGearRatio == 0.0)
        YGearRatio = 10.0;
}

/************************************************/
/*
��������

*/
/************************************************/

void Action(void)
{
    ReadRobotCurrent();
    KeyBoardProc();

    ManualAction();
    AutoRun();
    ResumeLocation();
    AlarmProtect();
    SaveAndLoadParams();
}
