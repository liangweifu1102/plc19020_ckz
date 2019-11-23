/*
 * @Description: 
 * @Author: huacong
 * @LastEditors: ccc
 * @Date: 2019-04-17 14:32:41
 * @LastEditTime: 2019-05-06 20:23:47
 */
#ifndef USERMEMORY_H
#define USERMEMORY_H

#include "typedefine.h"
//#include "sys.h"

#define MEMSIZE 4096
#define MEMSIZE_REG 4096
#define MEMSIZE_COIL 1024

#define Page_of_4K 4096 // 一页  4096

extern BYTE_BITS cMidleCoil[MEMSIZE_COIL / 8];
extern unsigned short cDataArea[MEMSIZE_REG]; // 只有0 --- 1023 是保证能通信的

#define SETBIT(x) cMidleCoil[x >> 3].Val |= (0x01 << (x & 0x0007))
#define CLRBIT(x) cMidleCoil[x >> 3].Val &= (~(0x01 << (x & 0x0007)))
#define GETBIT(x) (cMidleCoil[x >> 3].Val & (1 << (x & 0x0007)))

// 其中100 - 180 定位铣槽 180 - 237 为定位打孔
typedef enum
{
    SystemParam_offset = 1,
    SpeedParamStr_Offset = 30,
    FactoryParam_Offset = 42,
    UserParam_offset = 65,
    TestData_offset = 300,
    CurrentParam_offset = 1000,
} UserMemoryoffset;

#define AlarmNum cDataArea[800]

typedef __packed struct
{
    unsigned short InverterAlarm_L; // 1 左变频器
    unsigned long Work4Dist; // 2 第四工位位置
    unsigned short EncodeDirection;        // 4 X编码器方向
    unsigned short XResetSpeed;     // 5 X轴复位速度
    unsigned short YResetSpeed;     // 6 Y轴复位速度
    unsigned short XDir;                   //7 X轴伺服方向
    unsigned short YDir;                   // 8 Y轴伺服方向

    unsigned short DemoFunction;       // 9 演示功能
    unsigned short XSevoAlarmState;    //10 X伺服报警
    unsigned short YSevoAlarmState;    // 11 Y伺服报警
    unsigned short InverterAlarm_R; // 12 变频器
    unsigned short SystemMode;         // 13 系统模式
    unsigned short UseProbe;           // 14 扫描探头是否使用 0 不使用  1 使用
    unsigned short XCalculate;         // 15 扫描打孔位置计算方式

    unsigned short XCycleLength; //指定脉冲距离  16
    unsigned short XCyclePulse;  //  17  X轴一圈脉冲数
    long XMaxLength;    //18  X轴最大形成

    unsigned short YCycleLength; //Y轴 每圈距离  20
    unsigned short YCyclePulse;  //Y轴每圈脉冲数  21
    long YMaxLength_R;    // Y 轴最大行程  22

    unsigned long Work1Dist; // 24  第一工位
    unsigned long Work2Dist; // 26  第二工位
    unsigned long Work3Dist; //28  第三工位
}
SystemParam_str;

typedef __packed struct
{
    unsigned short ManualSpeed; // 30
    unsigned short ScanSpeed2;  // 31非扫描打孔检测速度
    unsigned short XIdleSpeed;  //32 X轴空走
    unsigned short YIdleSpeed;  //33 Y轴空走
    unsigned short YDrillSpeed;
    unsigned short XSlotSpeed;
    unsigned short XMoveSpeed;      //36
    unsigned short CheckHoleSpeed2; // 先扫后打检孔速度  37
    unsigned short YManualSpeed;    // 先扫后打检孔速度  38

}
SpeedParam_Str;

#define CHAXIAO_MODE FactoryParam->ChaXiaoMode
#define CHAXIAO_Index FactoryParam->ChaXiaoIndex // 第几个孔打插销
//#define LocationAddrArray cDataArea[45]          // 定位打孔第几组位置参数

typedef __packed struct
{
    unsigned short ArrayNumber; // 定位打孔第几组位置参数  45

    unsigned short CustomPlug1; //任意插销  46
    unsigned short CustomPlug2; // 47

    unsigned long YMaxLength_L;      //48 左刀Y 轴最大行程

    unsigned short MinHoleRaidus; // 50
    unsigned short MaxHoleRaidus; //51

    unsigned short YOrignDist_R; // 52
    unsigned short YOrignDist_L; // 53 左刀原点到板边的距离

    unsigned short ChaXiaoDist;  // 54
    unsigned short ChaXiaoMode;  // 55
    unsigned short ChaXiaoIndex; // 第几个孔打插销  56
    unsigned short ChaXiaoDepth; // 定位孔  57

    unsigned short Hole_Probe;       //58
    unsigned short Code_Probe;       //59
    unsigned short PreDecDistance;   // 60

    unsigned short MotorStartDelay_R; //61 电机启动时间
    unsigned short MotorStopDelay_R; //62 电机启动时间

    unsigned short MotorStopDelay_L;  //63 电机启动时间
    unsigned short MotorStartDelay_L; //64 左电机启动时间

    unsigned short NoWorktime; //钻孔圆整时间 65

    unsigned short DrillCycleTime;       //钻孔圆整时间  66
    unsigned short DrillStsrtDelay;      // 67  压料到位延时
    unsigned short ToolValveDelay;      //68 刀具气阀动作时间

    unsigned short QiGangReBackDelay; // 69 气缸到位延时
    unsigned short XBackDelayTime;    // 70  X轴到位演示

    unsigned short HoleFeedDepth;   // 71  打孔每次进刀深度
    unsigned short HoleDrillMode;   // 72 Y轴打孔方式
    unsigned short HoleDepth;  //73 打孔深度

    unsigned short WorkMode;    // 74 工作模式
    unsigned short DotDistance; // 75
    unsigned short DrillRaidus; //76

    unsigned short TSlotMode; // 77铣槽模式

    unsigned short CaoPicDepth;  //78
    unsigned short TongCaoDepth; // 79

    unsigned long TW_HeadDist; // 80 槽起点
    unsigned long TW_EndDist;  // 82　槽长度
    short TW_EndOffset; //84

    unsigned short TWHoleMode;         //85
    unsigned short TWSlotMode;         //86

    unsigned short ToolValveOrignEnable; // 87 刀具气缸原点信号是否使用
    unsigned short DrillwithBlow; // 88打孔时吹气

    unsigned short Axis_Position_Flag; // 89 G代码位置轴设定
    unsigned short Axis_Depth_Flag;    // 90 G代码深度轴设定

    short ToolOffSet_Scan; // 91扫描打孔刀具偏置

    short ToolOffSet_Hole_L; // 92 定位模式左刀的偏移
    short ToolOffSet_Hole_R; // 93 定位打孔道具偏置

    short ToolOffSet_TW_L; // 94 扫描孔槽模式左刀的偏移
    short ToolOffSet_TW_R;  // 95 扫描孔槽模式右刀的偏移

    short ToolOffSet_Code_L; // 96 扫码模式左刀的偏移
    short ToolOffSet_Code_R; // 97 扫码

    short SlowMotorBuchang; //98 减速机反向间隙补偿
}
FactoryParam_str;

typedef __packed struct _IOConfigParamStrct_
{
    unsigned short XPin[50]; //100
    unsigned short YPin[50]; //150
}
IOConfigParamStrct;
// 200 -- 255
typedef __packed struct
{
    unsigned long ProductNum; // 加工次数
    short XOrignOffset;//202
    short YOrignOffset;//203
    unsigned short XAccDec;// 204
    unsigned short res[5];
    unsigned long TW_HoleDist;          // 210 头尾定长模式插销长度
    unsigned long TW_SlotLen;           // 212 头尾定长模式段槽长度
    unsigned short TW_HoleDepth;        // 214 头尾定长模式打孔深度
    unsigned short TW_SlotDepth;        // 215 头尾定长模式段槽深度
    unsigned short TW_SlotOneDepth;     // 216 头尾定长模式通槽深度
    unsigned short TSlotTool;           // 217 头尾定长模式桶槽刀具选择

    unsigned short ScanSlotType; // 219 扫描孔操段槽加工类型
    unsigned short DingSlotType; // 220 定位孔操段槽加工类型
    unsigned short CodeSlotType; // 221 扫码孔操段槽加工类型
    unsigned short KSlotPicDepth; // 222 扫码孔操段槽加工类型
}
UserParam_str;
//
//
// 180 -- 240 定位打孔
typedef __packed struct
{
    unsigned long PosF[20];     // 300
    unsigned long res1[5];      //
    unsigned long PosB[20];     // 350
    unsigned long res2[5];      //
    unsigned long Pos_Up[20];   // 400 上升沿间距
    unsigned long res3[5];      //
    unsigned long Pos_Hole[20]; // 450 孔径
}
HolePos_str;
//500-599扫码数据
typedef __packed struct
{
    long Orign[20];           // 500
    long Length[20];          // 540
    unsigned short Depth[20]; // 580
}
ScanCodeData_str;

typedef __packed struct
{
    unsigned short name[96];           // 600
    unsigned short readok;
    unsigned short len;
    unsigned short index;
}
Filename_str;

typedef __packed struct
{
    unsigned short Writeok; // 700
    unsigned short len;

    unsigned short res[8];

    unsigned short name[80];           // 710

}
ManualFilename_str;

#define ALARM_REG cDataArea[800]

typedef __packed struct
{
    unsigned short cAlarmNum;       //800
    unsigned short CheckHoleRadius; //801

    long WoodLength;     //802
    long ulEncoderCount; //804

    unsigned short Signal_Up;    // 上升沿 806
    unsigned short Signal_Down;  // 下降沿
    unsigned short DrillNum;     //808
    unsigned short HoleDist;     //809
    unsigned short TopFlag;      //810
    unsigned short RightFlag;    //811
    unsigned short BottomFlag;   //812
    unsigned short LeftFlag;     //813
    unsigned short debugdata1;   //814
    unsigned short debugdata2;   //815
    unsigned short CurrentArray; //816
    unsigned short debugdata3;   //817
    unsigned short Tool1OutFlag; //818
    unsigned short Tool2OutFlag; //819

    uint16 BoardMode;
}
UserCurrentPara_Str;

// 900 -- 999 定位数据
typedef __packed struct
{
    unsigned long Orign[20];  // 900
    unsigned long Length[20]; // 940
    unsigned short Depth[20]; // 980
}
DingData_str;

// 1024
typedef __packed struct
{
    long XDistance; //X轴距离	1000
    long YDistance; //Y轴距离  1002
    long ZDistance; //Z轴距离	1004
    long UDistance; //U轴距离  1006
    long AxisPosRes[6];

    unsigned short Ver1; // 1020大版本号
    unsigned short Ver2; // 1021 小版本
    unsigned short Ver3; // 1022 小版本的子版本号

}
CurAxisDistStr;

typedef __packed struct
{
    unsigned short read[64];
    unsigned short write[32];
}
FileNameStr;

// typedef enum
// {
//     SYSTEM_NORMAL = 600,
//     SYSTEM_NEED_RESET = 601,
//     X_HARDWARE_LIMIT = 602,
//     X_SERVO_ALARM = 603,
//     INVERT_ALARM = 604,
//     TOOL_OUT1_ALARM = 605,
//     TOOL_OUT2_ALARM = 606,
//     SYSTEM_RESET_NOW = 607,

//     EMERGENCY_STOP = 609,
//     AUTOWORKING = 610,
//     Y_SERVO_ALARM = 612,
//     Y_SOFTWARE_LIMIT = 613,
//     ALARM14 = 614,
//     ALARM15 = 615,
//     X_SOFTWARE_LIMIT = 616,
//     ALARM17 = 617,
//     USB_NO_CONNECT = 618,
//     NO_WORK_DATA = 619,
//     LEFT_INVERT_ALARM = 620,
//     RIGHT_TOOL_VALAVE = 621,
//     LEFT_TOOL_VALAVE = 622,
// } User_Error_Coil;

typedef enum
{
    SET_TW_HOLE = 700,
    SET_TW_HOLE_M = 701,
    SET_TW_HOLE_X = 702,
    SET_TW_SLOT = 703,
    SET_TW_SLOT_M = 704,
    SET_TW_SLOT_ONE = 705,
    //SET_TW_HOLE_TOOL = 707,
    //SET_TW_SLOT_TOOL = 708,
    SET_T_SLOT_TOOL = 709,
    SET_T_SLOT = 130,
} USER_SET_COIL;

typedef enum
{
    DISP_T_SLOT = 131,
    SYSTEMMODE_0 = 800,
    SYSTEMMODE_1 = 801,
    SYSTEMMODE_2 = 802,
    SYSTEMMODE_3 = 803,
    SYSTEMMODE_4 = 804,

    DISP_TW_HOLE = 805,
    DISP_TW_HOLE_M = 806,
    DISP_TW_HOLE_X = 807,
    DISP_TW_SLOT = 808,
    DISP_TW_SLOT_M = 809,
    DISP_TW_SLOT_ONE = 810,
    DISP_T_SLOT_TOOL = 811,
    DISP_READFILE_OK = 812,
    DISP_U_CONNECT_OK = 813,
    DISP_BARCODE_OK = 814,
} USER_CURRENT_COIL;

typedef enum
{
    USER_USB_CONNECT = 1000,
    USER_USB_READOK,
    USER_USB_GETDATAFLAG,
} User_USB_Coil;

#define CONNECT_FAIL 0
#define CONNECT_OK 1

#define READ_OK 1
#define READ_FAIL 0

extern SystemParam_str *SystemParam;
extern SpeedParam_Str *SpeedParam;
extern FactoryParam_str *FactoryParam;
extern UserParam_str *UserParam;
extern HolePos_str *HolePos;
extern CurAxisDistStr *SysCurrentParam;
extern UserCurrentPara_Str *UserCurrentParam;
extern ScanCodeData_str *ScanCodeWorkData;
//extern CodeModeParam_str *CodeModeParam;
extern IOConfigParamStrct *IOConfigParam;
extern DingData_str *DingData;

extern Filename_str *FileNameData;
extern ManualFilename_str *ManualFilename;

extern uint8 buffertmp[Page_of_4K];

extern void Save16BitDate(unsigned short data, unsigned short addr);
extern void Save32BitDate(unsigned long data, unsigned short addr);

extern void InitVariable(void);
extern void SaveAndLoadParams(void);

#define M00 cMidleCoil[0].bits.b0
#define M01 cMidleCoil[0].bits.b1
#define M02 cMidleCoil[0].bits.b2
#define M03 cMidleCoil[0].bits.b3
#define M04 cMidleCoil[0].bits.b4
#define M05 cMidleCoil[0].bits.b5
#define M06 cMidleCoil[0].bits.b6
#define M07 cMidleCoil[0].bits.b7

#define M08 cMidleCoil[1].bits.b0
#define M09 cMidleCoil[1].bits.b1
#define M10 cMidleCoil[1].bits.b2
#define M11 cMidleCoil[1].bits.b3
#define M12 cMidleCoil[1].bits.b4
#define M13 cMidleCoil[1].bits.b5
#define M14 cMidleCoil[1].bits.b6
#define M15 cMidleCoil[1].bits.b7

#define M16 cMidleCoil[2].bits.b0
#define M17 cMidleCoil[2].bits.b1
#define M18 cMidleCoil[2].bits.b2
#define M19 cMidleCoil[2].bits.b3
#define M20 cMidleCoil[2].bits.b4
#define M21 cMidleCoil[2].bits.b5
#define M22 cMidleCoil[2].bits.b6
#define M23 cMidleCoil[2].bits.b7

#define M24 cMidleCoil[3].bits.b0
#define M25 cMidleCoil[3].bits.b1
#define M26 cMidleCoil[3].bits.b2
#define M27 cMidleCoil[3].bits.b3
#define M28 cMidleCoil[3].bits.b4
#define M29 cMidleCoil[3].bits.b5
#define M30 cMidleCoil[3].bits.b6
#define M31 cMidleCoil[3].bits.b7

#define M32 cMidleCoil[4].bits.b0
#define M33 cMidleCoil[4].bits.b1
#define M34 cMidleCoil[4].bits.b2
#define M35 cMidleCoil[4].bits.b3
#define M36 cMidleCoil[4].bits.b4
#define M37 cMidleCoil[4].bits.b5
#define M38 cMidleCoil[4].bits.b6
#define M39 cMidleCoil[4].bits.b7

#define M40 cMidleCoil[5].bits.b0
#define M41 cMidleCoil[5].bits.b1
#define M42 cMidleCoil[5].bits.b2
#define M43 cMidleCoil[5].bits.b3
#define M44 cMidleCoil[5].bits.b4
#define M45 cMidleCoil[5].bits.b5
#define M46 cMidleCoil[5].bits.b6
#define M47 cMidleCoil[5].bits.b7

#define M48 cMidleCoil[6].bits.b0
#define M49 cMidleCoil[6].bits.b1
#define M50 cMidleCoil[6].bits.b2
#define M51 cMidleCoil[6].bits.b3
#define M52 cMidleCoil[6].bits.b4
#define M53 cMidleCoil[6].bits.b5
#define M54 cMidleCoil[6].bits.b6
#define M55 cMidleCoil[6].bits.b7

#define M56 cMidleCoil[7].bits.b0
#define M57 cMidleCoil[7].bits.b1
#define M58 cMidleCoil[7].bits.b2
#define M59 cMidleCoil[7].bits.b3
#define M60 cMidleCoil[7].bits.b4
#define M61 cMidleCoil[7].bits.b5
#define M62 cMidleCoil[7].bits.b6
#define M63 cMidleCoil[7].bits.b7

#define M64 cMidleCoil[8].bits.b0
#define M65 cMidleCoil[8].bits.b1
#define M66 cMidleCoil[8].bits.b2
#define M67 cMidleCoil[8].bits.b3
#define M68 cMidleCoil[8].bits.b4
#define M69 cMidleCoil[8].bits.b5
#define M70 cMidleCoil[8].bits.b6
#define M71 cMidleCoil[8].bits.b7

#define M72 cMidleCoil[9].bits.b0
#define M73 cMidleCoil[9].bits.b1
#define M74 cMidleCoil[9].bits.b2
#define M75 cMidleCoil[9].bits.b3
#define M76 cMidleCoil[9].bits.b4
#define M77 cMidleCoil[9].bits.b5
#define M78 cMidleCoil[9].bits.b6
#define M79 cMidleCoil[9].bits.b7

#define M80 cMidleCoil[10].bits.b0
#define M81 cMidleCoil[10].bits.b1
#define M82 cMidleCoil[10].bits.b2
#define M83 cMidleCoil[10].bits.b3
#define M84 cMidleCoil[10].bits.b4
#define M85 cMidleCoil[10].bits.b5
#define M86 cMidleCoil[10].bits.b6
#define M87 cMidleCoil[10].bits.b7

#define M88 cMidleCoil[11].bits.b0
#define M89 cMidleCoil[11].bits.b1
#define M90 cMidleCoil[11].bits.b2
#define M91 cMidleCoil[11].bits.b3
#define M92 cMidleCoil[11].bits.b4
#define M93 cMidleCoil[11].bits.b5
#define M94 cMidleCoil[11].bits.b6
#define M95 cMidleCoil[11].bits.b7

#define M96 cMidleCoil[12].bits.b0
#define M97 cMidleCoil[12].bits.b1
#define M98 cMidleCoil[12].bits.b2
#define M99 cMidleCoil[12].bits.b3
#define M100 cMidleCoil[12].bits.b4
#define M101 cMidleCoil[12].bits.b5
#define M102 cMidleCoil[12].bits.b6
#define M103 cMidleCoil[12].bits.b7

#define M104 cMidleCoil[13].bits.b0
#define M105 cMidleCoil[13].bits.b1
#define M106 cMidleCoil[13].bits.b2
#define M107 cMidleCoil[13].bits.b3
#define M108 cMidleCoil[13].bits.b4
#define M109 cMidleCoil[13].bits.b5
#define M110 cMidleCoil[13].bits.b6
#define M111 cMidleCoil[13].bits.b7

#define M112 cMidleCoil[14].bits.b0
#define M113 cMidleCoil[14].bits.b1
#define M114 cMidleCoil[14].bits.b2
#define M115 cMidleCoil[14].bits.b3
#define M116 cMidleCoil[14].bits.b4
#define M117 cMidleCoil[14].bits.b5
#define M118 cMidleCoil[14].bits.b6
#define M119 cMidleCoil[14].bits.b7

#define M120 cMidleCoil[15].bits.b0
#define M121 cMidleCoil[15].bits.b1
#define M122 cMidleCoil[15].bits.b2
#define M123 cMidleCoil[15].bits.b3
#define M124 cMidleCoil[15].bits.b4
#define M125 cMidleCoil[15].bits.b5
#define M126 cMidleCoil[15].bits.b6
#define M127 cMidleCoil[15].bits.b7

#define M128 cMidleCoil[16].bits.b0
#define M129 cMidleCoil[16].bits.b1
#define M130 cMidleCoil[16].bits.b2
#define M131 cMidleCoil[16].bits.b3
#define M132 cMidleCoil[16].bits.b4
#define M133 cMidleCoil[16].bits.b5
#define M134 cMidleCoil[16].bits.b6
#define M135 cMidleCoil[16].bits.b7

#define M136 cMidleCoil[17].bits.b0
#define M137 cMidleCoil[17].bits.b1
#define M138 cMidleCoil[17].bits.b2
#define M139 cMidleCoil[17].bits.b3
#define M140 cMidleCoil[17].bits.b4
#define M141 cMidleCoil[17].bits.b5
#define M142 cMidleCoil[17].bits.b6
#define M143 cMidleCoil[17].bits.b7

#define M144 cMidleCoil[18].bits.b0
#define M145 cMidleCoil[18].bits.b1
#define M146 cMidleCoil[18].bits.b2
#define M147 cMidleCoil[18].bits.b3
#define M148 cMidleCoil[18].bits.b4
#define M149 cMidleCoil[18].bits.b5
#define M150 cMidleCoil[18].bits.b6
#define M151 cMidleCoil[18].bits.b7

#define M152 cMidleCoil[19].bits.b0
#define M153 cMidleCoil[19].bits.b1
#define M154 cMidleCoil[19].bits.b2
#define M155 cMidleCoil[19].bits.b3
#define M156 cMidleCoil[19].bits.b4
#define M157 cMidleCoil[19].bits.b5
#define M158 cMidleCoil[19].bits.b6
#define M159 cMidleCoil[19].bits.b7

#define M160 cMidleCoil[20].bits.b0
#define M161 cMidleCoil[20].bits.b1
#define M162 cMidleCoil[20].bits.b2
#define M163 cMidleCoil[20].bits.b3
#define M164 cMidleCoil[20].bits.b4
#define M165 cMidleCoil[20].bits.b5
#define M166 cMidleCoil[20].bits.b6
#define M167 cMidleCoil[20].bits.b7

#define M168 cMidleCoil[21].bits.b0
#define M169 cMidleCoil[21].bits.b1
#define M170 cMidleCoil[21].bits.b2
#define M171 cMidleCoil[21].bits.b3
#define M172 cMidleCoil[21].bits.b4
#define M173 cMidleCoil[21].bits.b5
#define M174 cMidleCoil[21].bits.b6
#define M175 cMidleCoil[21].bits.b7

#define M192 cMidleCoil[24].bits.b0
#define M193 cMidleCoil[24].bits.b1
#define M194 cMidleCoil[24].bits.b2
#define M195 cMidleCoil[24].bits.b3
#define M196 cMidleCoil[24].bits.b4
#define M197 cMidleCoil[24].bits.b5
#define M198 cMidleCoil[24].bits.b6
#define M199 cMidleCoil[24].bits.b7

#define M200 cMidleCoil[25].bits.b0
#define M201 cMidleCoil[25].bits.b1
#define M202 cMidleCoil[25].bits.b2
#define M203 cMidleCoil[25].bits.b3
#define M204 cMidleCoil[25].bits.b4
#define M205 cMidleCoil[25].bits.b5
#define M206 cMidleCoil[25].bits.b6
#define M207 cMidleCoil[25].bits.b7

#define M208 cMidleCoil[26].bits.b0
#define M209 cMidleCoil[26].bits.b1
#define M210 cMidleCoil[26].bits.b2
#define M211 cMidleCoil[26].bits.b3
#define M212 cMidleCoil[26].bits.b4
#define M213 cMidleCoil[26].bits.b5
#define M214 cMidleCoil[26].bits.b6
#define M215 cMidleCoil[26].bits.b7

#define M216 cMidleCoil[27].bits.b0
#define M217 cMidleCoil[27].bits.b1
#define M218 cMidleCoil[27].bits.b2
#define M219 cMidleCoil[27].bits.b3
#define M220 cMidleCoil[27].bits.b4
#define M221 cMidleCoil[27].bits.b5
#define M222 cMidleCoil[27].bits.b6
#define M223 cMidleCoil[27].bits.b7

#define M224 cMidleCoil[28].bits.b0
#define M225 cMidleCoil[28].bits.b1
#define M226 cMidleCoil[28].bits.b2
#define M227 cMidleCoil[28].bits.b3
#define M228 cMidleCoil[28].bits.b4
#define M229 cMidleCoil[28].bits.b5
#define M230 cMidleCoil[28].bits.b6
#define M231 cMidleCoil[28].bits.b7

#define M232 cMidleCoil[29].bits.b0
#define M233 cMidleCoil[29].bits.b1
#define M234 cMidleCoil[29].bits.b2
#define M235 cMidleCoil[29].bits.b3
#define M236 cMidleCoil[29].bits.b4
#define M237 cMidleCoil[29].bits.b5
#define M238 cMidleCoil[29].bits.b6
#define M239 cMidleCoil[29].bits.b7

#endif
