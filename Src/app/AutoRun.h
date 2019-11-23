/*
 * @Description: 
 * @Author: ccc
 * @LastEditors: ccc
 * @Date: 2019-04-17 14:32:41
 * @LastEditTime: 2019-05-06 20:07:36
 */
#ifndef AUTORUNPROC_H
#define AUTORUNPROC_H

#define STARTSPD 10
#define NSTEPDLAY 3
#define ACCTIME 10
#define XCONCMP 50

//#define CCW 1
//#define CW 0

#define ENCODE_USED 1

#define CODE_HOLE 0
#define CODE_SLOT 1

typedef struct
{
    long Dist32;
    long MinHoleR;
    long MaxHoleR;
    long XMaxLength;
    long Work1Dist;
    long Work2Dist;
    long Work3Dist;

    long DrillRaidus;

    unsigned long SlowMotorBuchang;

    long YMaxLength;
    long HoleDepth;
    long ChaXiaoDepth;
    long YOrignDist;
    long CaoPicDepth;

    long HoleFeedDepth;
} TempDist_str;

typedef struct
{
    char step;
    char Flag;
    char StartFlag;
    unsigned long Delay;
    long CurLength;
    long Length;
    long Depth;
    long Orign;
    long End;
    long Offset;
    long AllLength1; // �����ߵĳ���
    long AllLength2; // �����ߵĳ���
    long R;          // �뾶
    long D;          // ֱ��
    long StopPos;    // ֹͣλ��
    long StartPos;   // ��ʼλ��
    long EndOffset;  // ͷβ�����۳��Ȳ���
    long Remainder;
} TongCao_str;

typedef struct
{
    long hole_point;
} HoleInfomation;

typedef struct
{
    long xstart;
    long xstop;
    long yorign;
    long ydepth;
} SlotWorkData_str;

extern unsigned long MotorStartDealy;
extern unsigned long ToolValveOutDelay;

extern unsigned char cRunStep;

extern unsigned long NoworkStopdelay;
extern unsigned short WorkingFlag;

extern volatile char CheckHoleNum[2];
extern unsigned char StartFlag;

extern long HoleinfoF[100];
extern long HoleinfoB[100];

extern uint8 UseProbeFlag;
extern unsigned short MotorCoilSelectStep;
extern long ulBoardHead;
extern unsigned char cToolOutFlag;
extern unsigned long MotorStartDelay;

extern void MotorCoilSelect(void);
extern void AutoRun(void);

extern void RightToolOutProc(void);
extern void LeftToolOutProc(void);

extern void StopKeyPro(void);
extern void StopKeyPro2(void);
#endif
