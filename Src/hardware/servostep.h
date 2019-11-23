#ifndef _SERVOSTEP_H
#define _SERVOSTEP_H

#include "pwm.h"
#include "../app/usermemory.h"

typedef struct
{
    unsigned short cAxisNo; //
    unsigned short cSetSpeed;

    unsigned short cCurSpeed;
    unsigned short cCalSpeed;

    unsigned short cSetAccVal;
    unsigned short cSetDecVal;

    unsigned short cCalAccPulse;
    unsigned short cCalDecPulse;

    unsigned short cSetStartSpeed;
    unsigned short cAccDecVal;

    unsigned short cMinSpeed;
    unsigned short cMaxSpeed;

    unsigned short cRstStep;
    unsigned short cZeroFilter;

    unsigned short State;
    unsigned short bAuto_Mov;

    long cRealPosi;
    long cDestPosi;
    unsigned long cPmovPulse;
    long cMinLen;
    long cMaxLen;
} StepperDataInfo;

#define STARTSPEED_0 0
#define STARTSPEED_3 3

#define XIDEACC 450//UserParam->XAccDec//  200
#define YIDEACC 100
#define XIDEACC2 120
#define XIDEACC3 150

extern WORD_BITS cAxisDriveState;

#define X_DRV cAxisDriveState.bits.b0
#define Y_DRV cAxisDriveState.bits.b1
#define Z_DRV cAxisDriveState.bits.b2
#define U_DRV cAxisDriveState.bits.b3

#define bLimitAlarmEn_X cAxisDriveState.bits.b4
#define bLimitAlarmEn_Y cAxisDriveState.bits.b5
#define bLimitAlarmEn_Z cAxisDriveState.bits.b6
#define bLimitAlarmEn_U cAxisDriveState.bits.b7

extern WORD_BITS cAxisAlarmState;

#define bLimitP_X_Old cAxisAlarmState.bits.b0
#define bLimitP_Y_Old cAxisAlarmState.bits.b1
#define bLimitP_Z_Old cAxisAlarmState.bits.b2
#define bLimitP_U_Old cAxisAlarmState.bits.b3

#define bLimitP_X cAxisAlarmState.bits.b4
#define bLimitP_Y cAxisAlarmState.bits.b5
#define bLimitP_Z cAxisAlarmState.bits.b6
#define bLimitP_U cAxisAlarmState.bits.b7

#define bLimitN_X_Old cAxisAlarmState.bits.b8
#define bLimitN_Y_Old cAxisAlarmState.bits.b9
#define bLimitN_Z_Old cAxisAlarmState.bits.b10
#define bLimitN_U_Old cAxisAlarmState.bits.b11

#define bLimitN_X cAxisAlarmState.bits.b12
#define bLimitN_Y cAxisAlarmState.bits.b13
#define bLimitN_Z cAxisAlarmState.bits.b14
#define bLimitN_U cAxisAlarmState.bits.b15

extern StepperDataInfo tXAxisStepper;
extern StepperDataInfo tYAxisStepper;
extern StepperDataInfo tZAxisStepper;
extern StepperDataInfo tUAxisStepper;

extern void XAxis_PWMProc(void);
extern void YAxis_PWMProc(void);
extern void ZAxis_PWMProc(void);

extern void PWMTest(void);

extern void CalRunPulse(StepperDataInfo *tCurSetStpper);
extern void MV_Set_Startv(unsigned short cAxisNo, unsigned short cSpeed);
extern void MV_Set_Speed(unsigned short cAxisNo, unsigned short cSpeed);
extern long MV_Get_Command_Pos(unsigned short cAxisNo);
extern void MV_Set_Command_Pos(unsigned short cAxisNo,long pos);
extern void MV_Set_Limit_P(unsigned short cAxisNo, unsigned long dwMaxLen);
extern void StartPWM(unsigned short cAxisNo);
extern void ReadAxisStatus(void);
extern void MV_Suddent_Stop(unsigned short cAxisNo);
extern void MV_Dec_Stop(unsigned short cAxisNo);
extern void MV_Move_To_Position(unsigned short axis, long position);
extern void MV_Pmove(unsigned short cAxisNo, unsigned long dwPosi);
extern void MV_AccDec_Set(unsigned short cAxisNo, unsigned short cSetAcc, unsigned short cSetDec);
extern void MV_Set_Acc(unsigned short cAxisNo, unsigned short cSetAcc);
extern void MV_Set_Dec(unsigned short cAxisNo, unsigned short cSetAcc);
extern void MV_Const_Move(unsigned short cAxisNo, unsigned short cSpeed, unsigned short dir);
extern void MV_Set_Limit_P(unsigned short cAxisNo, unsigned long dwMaxLen);
extern void ReadAxisStatus(void);
extern void ServoInit(void);
extern void ServoMotorEnable(void);
extern void AxisOrign_PWM(uint16 cAxisNo);
extern void ServoSoftProtect(void);

extern void MoveAction_Const(unsigned short cAxisNo, unsigned short dir, unsigned short cSpeed);
extern void MoveAction_Pulse(unsigned short cAxisNo, long dwPosi, unsigned short cSpeed);
extern void MoveAction_Dist(unsigned short cAxisNo, unsigned long dwPosi, unsigned short cSpeed);

extern void SetPulse_X(unsigned short startv, unsigned short cSpeed, long dwPosi);
extern void SetPulse_Y(unsigned short startv, unsigned short cSpeed, long dwPosi);

#endif