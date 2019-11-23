#ifndef __GCODEA_H_
#define __GCODEA_H_

#include "typedefine.h"

#define Pos_AxisFlag 1
#define Depth_AxisFlag 2

#define X_AxisFlag 1
#define Y_AxisFlag 2
#define Z_AxisFlag 3

typedef struct
{
    uint8 cRunStep;     //解析成功标志
    uint8 CmdType; // 命令类型 C M T
    uint16 CmdNum; // 命令号

    uint8 cGRunStep;
    uint8 cTRunStep;
    uint8 M05Flag;
    uint8 M30Flag;

    uint8 StartFlag;
    uint8 xRefresh;
    uint8 yRefresh;
    uint8 zRefresh;

    uint8 M601Flag;
    uint8 M602Flag;
    uint8 M603Flag;
    uint8 M604Flag;

    int32 xpos; //保存当前行的数据
    int32 ypos;
    int32 zpos;

} GcodeA_str;

extern GcodeA_str GCodeRunParam;
extern void GCodePreRead(uint8 *buf);
extern void CLearWorkFlag(void);
extern uint8 IsGcodeRunOver(void);
extern void GCodeRunProc(void);
extern void SetGcodeRunReStart(void);
extern void SetGcodeRunStart(void);
extern uint8 GetGcodeOver(void);
extern void SetNextWorkFlag(void);

#endif
