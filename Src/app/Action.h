/*
 * @Description: 
 * @Author: huacong
 * @LastEditors: ccc
 * @Date: 2019-04-17 14:32:41
 * @LastEditTime: 2019-05-06 19:37:59
 */
#ifndef ACTION_H
#define ACTION_H

#include "typedefine.h"
#include "../hardware/inout.h"

extern WORD_BITS cAxisRunState;

#define bRunning1 cAxisRunState.bits.b0
#define bRunning2 cAxisRunState.bits.b1     //����׾�״̬
#define bRunning3 cAxisRunState.bits.b2     //��������
#define bRunning4 cAxisRunState.bits.b3     //��������

#define bRBOrignY cAxisRunState.bits.b5
#define bRBOrignXY cAxisRunState.bits.b6

#define bMotorSpeedStopFlag cAxisRunState.bits.b7  // 扫码工作完，电机停止

#define bPress1_Vavle Y00
#define bPress2_Vavle Y01
#define bPress3_Vavle Y02


#define bCuiQi Y03

#define bRightToolValve Y04
#define bRightToolMotor Y05

#define bLeftToolValve Y06
#define bLeftToolMotor Y07
#define bInvertStart Y10 // 变频器

#define bPress4_Vavle Y11

#define bDrillValve Y20 // 单伺服Y轴

#define bEmgStop (!X00)
#define bOldEmgStop (!OldX00)
#define X_Orign X01
#define X_LIMIT_P X02
#define Y_Orign X03

#define CheckObjectSgn I004
#define bRightInvertAlarm X05    //变频器报警
#define bLeftToolOrign X06  // 左刀原点
#define bRightToolOrign X07 // 右刀原点

#define bPress1_IN X11
#define bPress2_IN X12
#define bPress3_IN X13
#define bPress4_IN X14

#define bPrePress1_IN bOldX11
#define bPrePress2_IN bOldX12
#define bPrePress3_IN bOldX13
#define bPrePress4_IN bOldX14

#define bQiGangBackSgn X15
#define bLeftInvertAlarm X20
#define bQiGangComeSgn X17

#define bZServoAlarm X40
#define bUServoAlarm X41
#define bXServoAlarm X42
#define bYServoAlarm X43

//#define bRunning M00 //���в���
//#define bRunning1 M01
//#define bRunning2 M02     //����׾�״̬
//#define bRunning3 M03     //��������
#define bResume M04       //��λ��
#define bWriteFactory M05 //����׾�ѡ��
#define bReadFactory M06  //����׾�ѡ��
#define bReadParam M07    //��ȡ����
#define bWriteParam M08   //

#define bClrProduct M09 //����ѡ��
#define bSelModeST M10 //����ѡ��

#define bRunning M11 //���в���

#define bAlarmFlag M12 //����ѡ��
#define bKeyEnable M13

#define bClearH M14 //
#define bClearC M15 //

//#define bSystemMode_0 M16 //
//#define bSystemMode_1 M17 //
//
//#define bSystemMode_2 M18 //����ѡ��
//#define bSystemMode_3 M19 //

#define bLongBoard_ST M26
#define bLongBoard M27

#define bMVXp M32 //X-
#define bMVXn M33 //X+
#define bMVYp M30 //X-
#define bMVYn M31 //X+
#define bMVZp M35
#define bMVZn M34

#define bDotMVST M36 //�ֶ�����
#define bRest M37    //X��Y�����״̬

#define bStop M43 //X��Y�����״̬

#define bCustomPlug1_1 M48 //
#define bCustomPlug2_1 M49 //
#define bCustomPlug3_1 M50 //
#define bCustomPlug4_1 M51 //
#define bCustomPlug5_1 M52 //
#define bCustomPlug6_1 M53 //
#define bCustomPlug7_1 M54 //
#define bCustomPlug8_1 M55 //

#define bCustomPlug9_1 M56  //
#define bCustomPlug10_1 M57 //
#define bCustomPlug11_1 M58 //
#define bCustomPlug12_1 M59 //
#define bCustomPlug13_1 M60 //
#define bCustomPlug14_1 M61 //
#define bCustomPlug15_1 M62 //
#define bCustomPlug16_1 M63 //

#define bCustomPlug1_2 M64 //
#define bCustomPlug2_2 M65 //
#define bCustomPlug3_2 M66 //
#define bCustomPlug4_2 M67 //
#define bCustomPlug5_2 M68 //
#define bCustomPlug6_2 M69 //
#define bCustomPlug7_2 M70 //
#define bCustomPlug8_2 M71 //

#define bCustomPlug9_2 M72  //
#define bCustomPlug10_2 M73 //
#define bCustomPlug11_2 M74 //
#define bCustomPlug12_2 M75 //
#define bCustomPlug13_2 M76 //
#define bCustomPlug14_2 M77 //
#define bCustomPlug15_2 M78 //
#define bCustomPlug16_2 M79 //


#define bReadOkDisplay M90
#define bWriteOkDisplay M91

#define bNOChaXiao_ST M92
#define bNOChaXiao M93

#define bDingModeSet M94
#define bDingMode_ST M95

#define bScanHole_ST M101
#define bScanHole M102

#define bWorkMode_Code_ST M103
#define bWorkMode_Code M104

#define bLocationHole_ST M105
#define bLocationHole M106

#define bLocationCao_ST M107
#define bLocationCao M108

#define bMiddleChaXiao_ST M109
#define bMiddleChaXiao M110

#define bTWNei_ST M111
#define bTWNei M112

#define bTWWai_ST M113
#define bTWWai M114

//#define bSystemMode_0 M117 //
//#define bSystemMode_1 M118 //

#define bDuanCao_ST M119
#define bDuanCao M120

#define bDisplayChaXiaoDepth M121

#define bCustomPlugBeforHole_ST M122
#define bCustomPlugBeforHole M123
#define bCustomPlugafterHole_ST M124
#define bCustomPlugafterHole M125
#define bCustomPlugAll_ST M126
#define bCustomPlugAll M127
#define bCustomPlugState_ST M128
#define bCustomPlugState M129

#define bTongcaoST M130
#define bTongcao M131
#define bDanCaoST M132
#define bDanCao M133
#define bChaXiaoSave M134
#define bPlugforOneHoleST M135
#define bPlugforOneHole M136
#define bPlugforTwoHoleST M137
#define bPlugforTwoHole M138

#define bReadHParam M139  //��ȡ����
#define bWriteHParam M140 //

#define bTongcao2ST M141
#define bTongcao2 M142

#define bMotorSpeedST M143
#define bMotorSpeed M144

#define bHoleDrillModeST M145
#define bHoleDrillMode M146

#define ScanCode_Slot 1
#define ScanCode_Hole 0

#define bScanCodeWorkMode M147
#define bScanCodeWorkMode_ST M148

#define bProbeModeDisplay M149 // 不使用探头，则触摸屏上的有些东西不显示
#define bToolTypeDisplay M150

#define bCodeWorkFlag1 M152
#define bCodeWorkFlag2 M153
#define bCodeWorkFlag3 M154
#define bCodeWorkFlag4 M155
// ��һ�����ڶ�����Զ����ص�
#define bCodeLoadFlag1 M156
#define bCodeLoadFlag2 M157
#define bCodeLoadFlag3 M158
#define bCodeLoadFlag4 M159

#define WorkMode_Scan 0 // ɨ����
#define WorkMode_TW 1 // ��λϳ��
#define WorkMode_Hole 2 // ��λ���
#define WorkMode_Code 3 // ɨ��ģʽ

#define DuanCao 1
#define TongCao 0
#define DanCao 2
#define TongCao2 3

#define PlugforTwoHole 8
#define PlugforOneHole 7
#define PlugBeforHole 6
#define PlugafterHole 5
#define PlugAll 4
#define PlugCustom 3
#define PlugTWWai 2
#define PlugTWNei 1
#define NoPlug 0
//系统类型
#define SYSTEM_TYPE0 0 //单伺服单主轴
#define SYSTEM_TYPE1 1 //双伺服单主轴
#define SYSTEM_TYPE2 2 //双伺服单主轴加钻包
#define SYSTEM_TYPE3 3 //双伺服双主轴单变频
#define SYSTEM_TYPE4 4 //双伺服双主轴双变频

//#define ERROR 0XFF

#define TOOLSELECT_L 1
#define TOOLSELECT_R 2

#define USB_STATE_CONNECT 2
#define USB_STATE_DISCONNECT 0
#define USB_STATE_RESET 1

//#define SERVORDLY 3000;

extern unsigned long bpq_alarm_time;

extern unsigned char cResumeStep;

extern unsigned long Press1time;
extern unsigned long Press2time;
extern unsigned long Press3time;
extern unsigned long Press4time;

extern unsigned short XEncodeDivide;
extern uint16 cChaXiaoBackup;
extern unsigned char RebackXYFLG; //��һ���ϵ��־
extern uint16 ServoDelay;
extern unsigned short RstPphlDlay; //��λ������ʱ

extern bool KeyPress1Flag;
extern bool KeyPress2Flag;
extern bool KeyPress3Flag;
extern bool KeyPress4Flag;

extern void Action(void);
extern void ResetPeripheral(void);
extern void CalGearRatio(void);
extern void StopAxis(void);
extern long PositionToPluse(unsigned short cAxisNo, long dist);
extern long PluseToPosition(unsigned short cAxisNo, long pulse);
extern void Swap(long a, long b);

#endif
