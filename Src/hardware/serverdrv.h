#ifndef _SERVODRV_H_
#define _SERVODRV_H_

#include "../mylib/typedefine.h"
#include "../stm32f4lib/stm32f407xx.h"

#define A_AXIS 0x10
#define B_AXIS 0x20

#define RATIO 100

#define CURVEACC 1

#define ACCDEC_BUF_SIZE 1000

#define AAxis_PWMInit() XAxis_PWM_Init()
#define BAxis_PWMInit() YAxis_PWM_Init()

#define XAxis_PWMProc() APwmInterruptProgram()
#define YAxis_PWMProc() BPwmInterruptProgram()

//#define AAxis_TCNT X_ARR
//#define BAxis_TCNT TIM8->CNT

#define A_TGRA X_ARR
#define A_TGRB X_CCR

#define B_TGRA Y_ARR
#define B_TGRB Y_CCR

typedef __packed struct _RunSpeedStrct_
{
	unsigned long StartSpeed;
	unsigned long OverSpeed;
	unsigned long AccSpdTime;
	unsigned long DecSpdTime;
	unsigned long CurSpeed;
	unsigned long MaxSpeed;

} RunSpeedStrct;

typedef __packed struct _SpeedPusleStrct_
{
	unsigned short AccStepPulse[ACCDEC_BUF_SIZE];
	unsigned short AccStep;
	unsigned short AccStepTime;
	unsigned short AccSetcionNum;

	unsigned short DecStepPulse[ACCDEC_BUF_SIZE];
	unsigned short DecStep;
	unsigned short DecStepTime;
	unsigned short DecSetcionNum;

	unsigned long AccDotDistance;
	unsigned long DecDotDistance;
	unsigned long ContSpeed;
} SpeedPusleStrct;

typedef __packed struct
{
	uint8 Dir;
	uint8 Cnter;
	uint8 Frecy;
	uint8 Acc;
	uint8 Dec;
	uint8 CRun;
	uint8 DStop;
	uint8 PStart;
	uint8 LimitDec;
	uint8 ALARMOC;
	uint8 SLMTNEN;
	uint8 SLMTPEN;
	uint8 ALARMEN;
	uint8 ArMStop;
	uint8 LimitStop;
	uint8 HLimitFlg;
} AxisCtrl_str;

typedef __packed struct
{
	uint8 SLMTP;
	uint8 SLMTN;
	uint8 ALARM;
	uint8 EMG;
} Axis_RR2_str;

typedef __packed struct
{
	uint8 DRV1;
	uint8 DRV2;
	uint8 DRV3;
	uint8 DRV4;
} AxisState_str;

typedef __packed struct
{
	uint8 HLMTP;
	uint8 HLMTN;
	uint8 ALARM;
	uint8 reg1;
} ServerAlarm_str;

extern AxisState_str cAxisState;
extern Axis_RR2_str AAxis_RR2, BAxis_RR2;
//RR0
//为0表示驱动结束，为1表示正在驱动；
#define A_DRV cAxisState.DRV1
#define B_DRV cAxisState.DRV2

#define A_SLMTP AAxis_RR2.SLMTP
#define A_SLMTN AAxis_RR2.SLMTN
#define A_ALARM AAxis_RR2.ALARM
#define A_EMG AAxis_RR2.EMG

#define B_SLMTP BAxis_RR2.SLMTP
#define B_SLMTN BAxis_RR2.SLMTN
#define B_ALARM BAxis_RR2.ALARM
#define B_EMG BAxis_RR2.EMG

extern AxisCtrl_str AAxis_Ctrl, BAxis_Ctrl;

#define A_Dir AAxis_Ctrl.Dir
#define A_Cnter AAxis_Ctrl.Cnter
#define A_Frecy AAxis_Ctrl.Frecy
#define A_ACC AAxis_Ctrl.Acc
#define A_DeC AAxis_Ctrl.Dec
#define A_CRun AAxis_Ctrl.CRun
#define A_DStop AAxis_Ctrl.DStop
#define A_PStart AAxis_Ctrl.PStart
#define A_LimitDec AAxis_Ctrl.LimitDec
#define A_ALARMOC AAxis_Ctrl.ALARMOC
#define A_SLMTNEN AAxis_Ctrl.SLMTNEN
#define A_SLMTPEN AAxis_Ctrl.SLMTPEN
#define A_ALARMEN AAxis_Ctrl.ALARMEN
#define A_ArMStop AAxis_Ctrl.ArMStop
#define A_LimitStop AAxis_Ctrl.LimitStop
#define A_HLimitFlg AAxis_Ctrl.HLimitFlg

#define B_Dir BAxis_Ctrl.Dir
#define B_Cnter BAxis_Ctrl.Cnter
#define B_Frecy BAxis_Ctrl.Frecy
#define B_ACC BAxis_Ctrl.Acc
#define B_DeC BAxis_Ctrl.Dec
#define B_CRun BAxis_Ctrl.CRun
#define B_DStop BAxis_Ctrl.DStop
#define B_PStart BAxis_Ctrl.PStart
#define B_LimitDec BAxis_Ctrl.LimitDec
#define B_ALARMOC BAxis_Ctrl.ALARMOC
#define B_SLMTNEN BAxis_Ctrl.SLMTNEN
#define B_SLMTPEN BAxis_Ctrl.SLMTPEN
#define B_ALARMEN BAxis_Ctrl.ALARMEN
#define B_ArMStop BAxis_Ctrl.ArMStop
#define B_LimitStop BAxis_Ctrl.LimitStop
#define B_HLimitFlg BAxis_Ctrl.HLimitFlg

extern ServerAlarm_str AServerAlarm, BServerAlarm;

#define A_HLMTP AServerAlarm.HLMTP
#define A_HLMTN AServerAlarm.HLMTN
#define A_SAlarmIn AServerAlarm.ALARM

#define B_HLMTP BServerAlarm.HLMTP
#define B_HLMTN BServerAlarm.HLMTN
#define B_SAlarmIn BServerAlarm.ALARM

extern long A_DistanceCount, B_DistanceCount;
extern unsigned long APluseMoveCount, BPluseMoveCount;
extern unsigned long APluseDecCount, BPluseDecCount;
extern SpeedPusleStrct ACurSpeedPusel;
extern SpeedPusleStrct BCurSpeedPusel;
extern RunSpeedStrct A_CurSpeed, B_CurSpeed;
extern unsigned short A_AccStep, A_DecStep;
extern unsigned short B_AccStep, B_DecStep;
extern unsigned short APwmStepcounter, BPwmStepcounter;
extern unsigned short ASpeedRatio, BSpeedRatio; //速度比率

extern void ResetServoDrv_CPU(unsigned short axis);
extern void MV_Set_Range_CPU(unsigned short axis, int wdata);
extern void MV_Set_Acc_CPU(unsigned short axis, unsigned short wdata);
extern void MV_Set_Dec_CPU(unsigned short axis, unsigned short wdata);
extern void MV_Set_Startv_CPU(unsigned short axis, unsigned short wdata);
extern void MV_Set_Speed_CPU(unsigned short axis, unsigned short wdata);
extern void MV_Pmov_CPU(unsigned short axis, int wdata);
extern void MV_Continue_Move_CPU(unsigned short axis, unsigned short dir);
extern void MV_Set_Command_Pos_CPU(unsigned short axis, int wdata);
extern long MV_Get_Command_Pos_CPU(unsigned short axis);
extern void MV_Dec_Stop_CPU(unsigned short axis);
extern void MV_Sudden_Stop_CPU(unsigned short axis);
extern void MV_Limit_Stop_CPU(unsigned short axis);
extern void MV_Set_Pulse_CPU(unsigned short axis, int wdata);
extern void MV_Set_Command_SlMTP_CPU(unsigned short axis, int wdata);
extern void MV_Set_Command_SlMTN_CPU(unsigned short axis, int wdata);
extern void CalAccDecSpeed(unsigned long Pulsemov, RunSpeedStrct currunspeed, SpeedPusleStrct *curspeedpusel);
extern void CalDecStop(RunSpeedStrct currunspeed, SpeedPusleStrct *curspeedpusel);
extern void CalAccRun(RunSpeedStrct currunspeed, SpeedPusleStrct *curspeedpusel);
extern void A_Pulse_Start();
extern void B_Pulse_Start();

extern void A_Pulse_Stop();
extern void B_Pulse_Stop();

extern void APwmInterruptProgram(void);
extern void BPwmInterruptProgram(void);

extern void AAxisHLimitProc(void);
extern void BAxisHLimitProc(void);

extern void AAxisDrvArmProc(void);
extern void BAxisDrvArmProc(void);

extern void ServoDrvProct(void);
extern void TimerPulseStart(void);

#endif
