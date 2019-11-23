#include "server.h"

//WORD_BITS cAxisState;
//WORD_BITS AAxis_RR2, BAxis_RR2;
//WORD_BITS AAxis_Ctrl, BAxis_Ctrl;
//WORD_BITS HardLimit, ServoAlarm;

AxisState_str cAxisState
AxisCtrl_str AAxis_Ctrl, BAxis_Ctrl;
Axis_RR2_str AAxis_RR2, BAxis_RR2;
ServerAlarm_str AServerAlarm, BServerAlarm;

long A_DistanceCount, B_DistanceCount;
unsigned long APluseMoveCount, BPluseMoveCount;
unsigned long APluseDecCount, BPluseDecCount;
SpeedPusleStrct ACurSpeedPusel;
SpeedPusleStrct BCurSpeedPusel;
RunSpeedStrct A_CurSpeed, B_CurSpeed;
unsigned short A_AccStep, A_DecStep;
unsigned short B_AccStep, B_DecStep;
unsigned short APwmStepcounter, BPwmStepcounter;
unsigned char A_Pulse_StartFlg, B_Pulse_StartFlg;
unsigned short ASpeedRatio, BSpeedRatio; //速度比率
unsigned long APluseMoveDist, BPluseMoveDist;

long ASoftLinitNeg, ASoftLinitPos;
long BSoftLinitNeg, BSoftLinitPos;
/************************************************/
/*
复位伺服驱动寄存器
*/
/************************************************/
void ResetServoDrv_CPU(unsigned short axis)
{
	cAxisState.Val = 0;
	HardLimit.Val = 0;
	ServoAlarm.Val = 0;
	if (axis == A_AXIS)
	{
		AAxis_PWMInit();
		AAxis_RR2.Val = 0;
		AAxis_Ctrl.Val = 0;
		memset(&ACurSpeedPusel, 0, sizeof(SpeedPusleStrct));
		memset(&A_CurSpeed, 0, sizeof(RunSpeedStrct));
		A_AccStep = 0;
		A_DecStep = 0;
		APluseMoveCount = 0;
		APluseMoveDist = 0;
		ASoftLinitNeg = 0;
		ASoftLinitPos = 0;
		APluseDecCount = 0;
		A_Pulse_StartFlg = 0;
		APwmStepcounter = 0;
		ASpeedRatio = 100;
	}
	else if (axis == B_AXIS)
	{
		BAxis_PWMInit();
		BAxis_RR2.Val = 0;
		BAxis_Ctrl.Val = 0;
		memset(&BCurSpeedPusel, 0, sizeof(SpeedPusleStrct));
		memset(&B_CurSpeed, 0, sizeof(RunSpeedStrct));
		B_AccStep = 0;
		B_DecStep = 0;
		BPluseMoveCount = 0;
		BPluseMoveDist = 0;
		BSoftLinitNeg = 0;
		BSoftLinitPos = 0;
		BPluseDecCount = 0;
		B_Pulse_StartFlg = 0;
		BPwmStepcounter = 0;
		BSpeedRatio = 100;
	}
	AAxis_PWMInit();
}
/************************************************/
/*
设置频率

*/
/************************************************/
void MV_Set_Range_CPU(unsigned short axis, int wdata)
{
	axis = axis;
	wdata = wdata;
}
/************************************************/
/*
设置加速度

*/
/************************************************/
void MV_Set_Acc_CPU(unsigned short axis, unsigned short wdata)
{
	if (axis == A_AXIS)
	{
		A_CurSpeed.AccSpdTime = wdata;
	}
	else if (axis == B_AXIS)
	{
		B_CurSpeed.AccSpdTime = wdata;
	}
}
/************************************************/
/*
设置减速度

*/
/************************************************/
void MV_Set_Dec_CPU(unsigned short axis, unsigned short wdata)
{
	if (axis == A_AXIS)
	{
		A_CurSpeed.DecSpdTime = wdata;
	}
	else if (axis == B_AXIS)
	{
		B_CurSpeed.DecSpdTime = wdata;
	}
}
/************************************************/
/*
设置减起始速度

*/
/************************************************/
void MV_Set_Startv_CPU(unsigned short axis, unsigned short wdata)
{
	if (axis == A_AXIS)
	{
		A_CurSpeed.StartSpeed = wdata;
		A_CurSpeed.OverSpeed = wdata;
	}
	else if (axis == B_AXIS)
	{
		B_CurSpeed.StartSpeed = wdata;
		B_CurSpeed.OverSpeed = wdata;
	}
}
/************************************************/
/*
设置减当前速度

*/
/************************************************/
void MV_Set_Speed_CPU(unsigned short axis, unsigned short wdata)
{
	if (axis == A_AXIS)
	{
		A_CurSpeed.CurSpeed = (wdata * ASpeedRatio) / 100;
	}
	else if (axis == B_AXIS)
	{
		B_CurSpeed.CurSpeed = (wdata * BSpeedRatio) / 100;
	}
}
/************************************************/
/*
设置移动相对位置

*/
/************************************************/
void MV_Pmov_CPU(unsigned short axis, int wdata)
{
	unsigned long pulsemov;
	if (wdata >= 0)
		pulsemov = wdata;
	else
		pulsemov = 0 - wdata;
	if (axis == A_AXIS)
	{
		if (wdata >= 0)
		{
			A_Dir = 0;
			DR_CW_A();
		}
		else
		{
			A_Dir = 1;
			DR_CCW_A();
		}
		if ((A_CurSpeed.CurSpeed != 0) && (pulsemov != 0)) //速度不为零，距离不为零移动
		{
			CalAccDecSpeed(pulsemov, A_CurSpeed, &ACurSpeedPusel);
			APluseMoveDist = pulsemov;
			/* Set TGRA3 */
			A_TGRA = ACurSpeedPusel.AccStepPulse[0];
			/* Set TGRB3 */
			A_TGRB = ACurSpeedPusel.AccStepPulse[0] >> 1; ///2;
			A_Cnter = 1;
			A_Pulse_Start();
		}
	}
	else if (axis == B_AXIS)
	{
		if (wdata >= 0)
		{
			B_Dir = 0;
			DR_CW_B();
		}
		else
		{
			B_Dir = 1;
			DR_CCW_B();
		}
		if ((B_CurSpeed.CurSpeed != 0) && (pulsemov != 0)) //速度不为零，距离不为零移动
		{
			CalAccDecSpeed(pulsemov, B_CurSpeed, &BCurSpeedPusel);
			BPluseMoveDist = pulsemov;
			/* Set TGRA3 */
			B_TGRA = BCurSpeedPusel.AccStepPulse[0];
			/* Set TGRB3 */
			B_TGRB = BCurSpeedPusel.AccStepPulse[0] >> 1; /// 2;
			B_Cnter = 1;
			B_Pulse_Start();
		}
	}
}
/************************************************/
/*
连续移动

*/
/************************************************/
void MV_Continue_Move_CPU(unsigned short axis, unsigned short dir)
{
	if (axis == A_AXIS)
	{
		if ((dir == 0) && (!A_SLMTP) && (!A_HLMTP) && (!A_ALARM))
		{
			A_Dir = 0;
			DR_CW_A();
			A_Cnter = 0;
			A_CRun = 1;
			if (A_CurSpeed.CurSpeed != 0)
			{
				CalDecStop(A_CurSpeed, &ACurSpeedPusel);
				CalAccRun(A_CurSpeed, &ACurSpeedPusel);
				A_TGRA = ACurSpeedPusel.AccStepPulse[0];
				A_TGRB = ACurSpeedPusel.AccStepPulse[0] >> 1; /// 2;
				A_Pulse_Start();
			}
		}
		else if ((dir == 1) && (!A_SLMTN) && (!A_HLMTN) && (!A_ALARM))
		{
			A_Dir = 1;
			DR_CCW_A();
			A_Cnter = 0;
			A_CRun = 1;
			if (A_CurSpeed.CurSpeed != 0)
			{
				CalDecStop(A_CurSpeed, &ACurSpeedPusel);
				CalAccRun(A_CurSpeed, &ACurSpeedPusel);
				A_TGRA = ACurSpeedPusel.AccStepPulse[0];
				A_TGRB = ACurSpeedPusel.AccStepPulse[0] >> 1; /// 2;
				A_Pulse_Start();
			}
		}
	}
	else if (axis == B_AXIS)
	{
		if ((dir == 0) && (!B_SLMTP) && (!B_HLMTP) && (!B_ALARM))
		{
			B_Dir = 0;
			DR_CW_B();
			B_Cnter = 0;
			B_CRun = 1;
			if (B_CurSpeed.CurSpeed != 0)
			{
				CalDecStop(B_CurSpeed, &BCurSpeedPusel);
				CalAccRun(B_CurSpeed, &BCurSpeedPusel);
				B_TGRA = BCurSpeedPusel.AccStepPulse[0];
				B_TGRB = BCurSpeedPusel.AccStepPulse[0] >> 1;
				/// 2;
				B_Pulse_Start();
			}
		}
		else if ((dir == 1) && (!B_SLMTN) && (!B_HLMTN) && (!B_ALARM))
		{
			B_Dir = 1;
			DR_CCW_B();
			B_Cnter = 0;
			B_CRun = 1;
			if (B_CurSpeed.CurSpeed != 0)
			{
				CalDecStop(B_CurSpeed, &BCurSpeedPusel);
				CalAccRun(B_CurSpeed, &BCurSpeedPusel);
				B_TGRA = BCurSpeedPusel.AccStepPulse[0];
				B_TGRB = BCurSpeedPusel.AccStepPulse[0] >> 1;
				/// 2;
				B_Pulse_Start();
			}
		}
	}
}
/************************************************/
/*
设置当前位置

*/
/************************************************/
void MV_Set_Command_Pos_CPU(unsigned short axis, int wdata)
{
	if (axis == A_AXIS)
	{
		A_DistanceCount = wdata;
	}
	else if (axis == B_AXIS)
	{
		B_DistanceCount = wdata;
	}
}
/************************************************/
/*
获取当前位置

*/
/************************************************/
long MV_Get_Command_Pos_CPU(unsigned short axis)
{
	long pos;
	if (axis == A_AXIS)
	{
		pos = A_DistanceCount;
	}
	else if (axis == B_AXIS)
	{
		pos = B_DistanceCount;
	}
	return (pos);
}
/************************************************/
/*
减速停止
*/
/************************************************/
void MV_Dec_Stop_CPU(unsigned short axis)
{
	unsigned short curspeedtmp;
	if (axis == A_AXIS)
	{
		curspeedtmp = A_TGRA;
		A_CurSpeed.CurSpeed = PCLK_FREQUENCY / curspeedtmp;
		A_CurSpeed.CurSpeed /= 100;
		CalDecStop(A_CurSpeed, &ACurSpeedPusel);
		if (A_DRV)
		{
			if ((A_Pulse_StartFlg == 1) && !A_PStart)
			{
				A_DRV = 0;
				A_Pulse_StartFlg = 0;
				A_DecStep = 0;
				A_Cnter = 0;
				A_CRun = 0;
				A_DStop = 0;
				A_LimitStop = 0;
				APluseDecCount = 0;
			}
			if ((A_Pulse_StartFlg == 0) && A_PStart && !A_DStop)
			{
				A_DecStep = 1;
				A_Cnter = 0;
				A_CRun = 0;
				A_DStop = 1;
				A_LimitStop = 0;
				APluseDecCount = 0;
			}
		}
	}
	else if (axis == B_AXIS)
	{
		curspeedtmp = B_TGRA;
		B_CurSpeed.CurSpeed = PCLK_FREQUENCY / curspeedtmp;
		B_CurSpeed.CurSpeed /= 100;
		CalDecStop(B_CurSpeed, &BCurSpeedPusel);
		if (B_DRV)
		{
			if ((B_Pulse_StartFlg == 1) && !B_PStart)
			{
				B_DRV = 0;
				B_Pulse_StartFlg = 0;
				B_DecStep = 0;
				B_Cnter = 0;
				B_CRun = 0;
				B_DStop = 0;
				B_LimitStop = 0;
				BPluseDecCount = 0;
			}
			if ((B_Pulse_StartFlg == 0) && B_PStart && !B_DStop)
			{
				B_DecStep = 1;
				B_Cnter = 0;
				B_CRun = 0;
				B_DStop = 1;
				B_LimitStop = 0;
				BPluseDecCount = 0;
			}
		}
	}
}
/************************************************/
/*
立即停止
*/
/************************************************/
void MV_Sudden_Stop_CPU(unsigned short axis)
{
	if (axis == A_AXIS)
	{
		A_Pulse_Stop();
	}
	else if (axis == B_AXIS)
	{
		B_Pulse_Stop();
	}
}
/************************************************/
/*
限位停止
*/
/************************************************/
void MV_Limit_Stop_CPU(unsigned short axis)
{
	if (axis == A_AXIS)
	{
		A_DecStep = 1;
		A_LimitStop = 1;
		A_DStop = 0;
		A_Cnter = 0;
		A_CRun = 0;
		APluseDecCount = 0;
	}
	else if (axis == B_AXIS)
	{
		B_DecStep = 1;
		B_LimitStop = 1;
		B_DStop = 0;
		B_Cnter = 0;
		B_CRun = 0;
		BPluseDecCount = 0;
	}
}
/************************************************/
/*
移动脉冲数
*/
/************************************************/
void MV_Set_Pulse_CPU(unsigned short axis, int wdata)
{
	axis = axis;
	wdata = wdata;
}
/************************************************/
/*
设置软件限位
*/
/************************************************/
void MV_Set_Command_SlMTP_CPU(unsigned short axis, int wdata)
{
	if (axis == A_AXIS)
	{
		ASoftLinitPos = wdata;
		A_SLMTPEN = 1;
	}
	else if (axis == B_AXIS)
	{
		BSoftLinitPos = wdata;
		B_SLMTPEN = 1;
	}
}
/************************************************/
/*
设置软件限位
*/
/************************************************/
void MV_Set_Command_SlMTN_CPU(unsigned short axis, int wdata)
{
	if (axis == A_AXIS)
	{
		ASoftLinitNeg = wdata;
		A_SLMTNEN = 1;
	}
	else if (axis == B_AXIS)
	{
		BSoftLinitNeg = wdata;
		B_SLMTNEN = 1;
	}
}
/************************************************/
/*
计算加减速
*/
/************************************************/
#if CURVEACC
void CalAccDecSpeed(unsigned long Pulsemov, RunSpeedStrct currunspeed, SpeedPusleStrct *curspeedpusel)
{
	unsigned long long Kacc; //加速度变化率
	unsigned long long Kdec; //减速度变化率
	unsigned long SAcc;		 //加速距离
	unsigned long SDec;		 //减速距离

	unsigned long caltmp;
	unsigned long accpulsetotal = 0, decpulsetotal = 0;
	unsigned short AccSecond, DecSecond;
	unsigned short i;
	unsigned long Acc_n_1, Acc_n;

	if (currunspeed.StartSpeed < 10)
	{
		currunspeed.StartSpeed = 10;
	}
	if (currunspeed.OverSpeed < 10)
	{
		currunspeed.OverSpeed = 10;
	}
	if (currunspeed.CurSpeed < 10)
	{
		currunspeed.CurSpeed = 10;
	}
	currunspeed.CurSpeed *= 10;
	currunspeed.StartSpeed *= 10;
	currunspeed.MaxSpeed *= 10;
	currunspeed.OverSpeed *= 10;
	//
	AccSecond = 1 + currunspeed.AccSpdTime / 1000;
	DecSecond = 1 + currunspeed.DecSpdTime / 1000;

	//S1为加速距离,S3为减速部分距离
	SAcc = (currunspeed.CurSpeed * currunspeed.AccSpdTime) / 200 + (currunspeed.StartSpeed * currunspeed.AccSpdTime) / 100;
	SDec = (currunspeed.CurSpeed * currunspeed.DecSpdTime) / 200 + (currunspeed.OverSpeed * currunspeed.DecSpdTime) / 100;

	while (SAcc + SDec > Pulsemov)
	{
		if (SAcc + SDec > Pulsemov)
		{
			currunspeed.CurSpeed = (currunspeed.CurSpeed * 9) / 10;
			if (currunspeed.CurSpeed < currunspeed.StartSpeed)
				break;
		}
		SAcc = (currunspeed.CurSpeed * currunspeed.AccSpdTime) / 200 + (currunspeed.StartSpeed * currunspeed.AccSpdTime) / 100;
		SDec = (currunspeed.CurSpeed * currunspeed.DecSpdTime) / 200 + (currunspeed.OverSpeed * currunspeed.DecSpdTime) / 100;
	}
	if (currunspeed.CurSpeed > currunspeed.StartSpeed) //计算出来的当前速度大于起始速度
	{
		Kacc = (4 * (currunspeed.CurSpeed - currunspeed.StartSpeed)) * 1000 / (currunspeed.AccSpdTime);
		Kacc = Kacc * 1000 / currunspeed.AccSpdTime;
		if (Kacc < 1000)
			Kacc = 1000;
		Kdec = (4 * (currunspeed.CurSpeed - currunspeed.OverSpeed)) * 1000 / (currunspeed.DecSpdTime);
		Kdec = Kdec * 1000 / currunspeed.DecSpdTime;
		if (Kdec < 1000)
			Kdec = 1000;
	}
	else
	{
		currunspeed.CurSpeed = currunspeed.StartSpeed;
		Kacc = 0;
		Kdec = 0;
	}
	curspeedpusel->ContSpeed = currunspeed.CurSpeed;
	//
	curspeedpusel->AccStepPulse[0] = currunspeed.StartSpeed; //加速脉冲起始值
	curspeedpusel->DecStepPulse[0] = currunspeed.CurSpeed;   //减速脉冲起始值

	curspeedpusel->AccStep = currunspeed.AccSpdTime / AccSecond;
	curspeedpusel->DecStep = currunspeed.DecSpdTime / DecSecond;
	curspeedpusel->AccStepTime = AccSecond;
	curspeedpusel->DecStepTime = DecSecond;

	Acc_n_1 = 0;
	//计算加速部分的速度表
	for (i = 1; i < 1000; i++)
	{
		if (i > curspeedpusel->AccStep)
		{
			curspeedpusel->AccStepPulse[i] = currunspeed.CurSpeed;
		}
		else
		{
			if (i <= curspeedpusel->AccStep / 2)
			{
				Acc_n = Acc_n_1 + (Kacc * AccSecond) / 1000;
			}
			else
			{
				Acc_n = Acc_n_1 - (Kacc * AccSecond) / 1000;
				if (i == curspeedpusel->AccStep)
				{
					Acc_n = 0;
				}
			}
			if (Acc_n != 0)
			{
				if ((i % 2) == 0)
				{
					curspeedpusel->AccStepPulse[i] = curspeedpusel->AccStepPulse[i - 1] + ((Acc_n + Acc_n_1) * AccSecond) / 2000;
				}
				else
				{
					curspeedpusel->AccStepPulse[i] = curspeedpusel->AccStepPulse[i - 1] + ((Acc_n + Acc_n_1) * AccSecond) / 2000 + 1;
				}
			}
			else
			{
				curspeedpusel->AccStepPulse[i] = curspeedpusel->AccStepPulse[i - 1] + ((Acc_n + Acc_n_1) * AccSecond) / 2000;
			}
			if (curspeedpusel->AccStepPulse[i] > currunspeed.CurSpeed)
			{
				curspeedpusel->AccStepPulse[i] = currunspeed.CurSpeed;
				curspeedpusel->AccStep = i;
			}
			accpulsetotal += curspeedpusel->AccStepPulse[i] * AccSecond;
			Acc_n_1 = Acc_n;
		}
	}
	//计算减速部分的速度表
	curspeedpusel->DecStepPulse[0] = currunspeed.CurSpeed;
	Acc_n_1 = 0;
	for (i = 1; i < 1000; i++)
	{
		if (i > curspeedpusel->DecStep)
		{
			curspeedpusel->DecStepPulse[i] = currunspeed.OverSpeed;
		}
		else
		{
			if (i <= curspeedpusel->DecStep / 2)
			{
				Acc_n = Acc_n_1 + (Kdec * AccSecond) / 1000;
			}
			else
			{
				Acc_n = Acc_n_1 - (Kdec * AccSecond) / 1000;
				if (i == curspeedpusel->DecStep)
				{
					Acc_n = 0;
				}
			}
			if (Acc_n != 0)
			{
				if ((i % 2) == 0)
				{
					curspeedpusel->DecStepPulse[i] = curspeedpusel->DecStepPulse[i - 1] - ((Acc_n + Acc_n_1) * AccSecond) / 2000;
				}
				else
				{
					curspeedpusel->DecStepPulse[i] = curspeedpusel->DecStepPulse[i - 1] - ((Acc_n + Acc_n_1) * AccSecond) / 2000 - 1;
				}
			}
			else
			{
				curspeedpusel->DecStepPulse[i] = curspeedpusel->DecStepPulse[i - 1] - ((Acc_n + Acc_n_1) * AccSecond) / 2000;
			}
			if (curspeedpusel->DecStepPulse[i] < currunspeed.OverSpeed)
			{
				curspeedpusel->DecStepPulse[i] = currunspeed.OverSpeed;
				curspeedpusel->DecStep = i;
			}
			decpulsetotal += (curspeedpusel->DecStepPulse[i] * DecSecond);
			Acc_n_1 = Acc_n;
		}
	}
	curspeedpusel->AccDotDistance = accpulsetotal / 100;
	decpulsetotal = decpulsetotal / 100;
	if (Pulsemov > decpulsetotal)
	{
		curspeedpusel->DecDotDistance = Pulsemov - decpulsetotal;
	}
	else
	{
		curspeedpusel->DecDotDistance = 0;
	}
	for (i = 0; i < 1000; i++)
	{
		caltmp = curspeedpusel->AccStepPulse[i];
		caltmp = PCLK_FREQUENCY / caltmp;
		caltmp /= 10;
		curspeedpusel->AccStepPulse[i] = caltmp;

		caltmp = curspeedpusel->DecStepPulse[i];
		caltmp = PCLK_FREQUENCY / caltmp;
		caltmp /= 10;
		curspeedpusel->DecStepPulse[i] = caltmp;
	}
	caltmp = curspeedpusel->ContSpeed;
	caltmp = PCLK_FREQUENCY / caltmp;
	caltmp /= 10;
	curspeedpusel->ContSpeed = caltmp;
}
#else
void CalAccDecSpeed(unsigned long Pulsemov, RunSpeedStrct currunspeed, SpeedPusleStrct *curspeedpusel)
{
	unsigned long caltmp;
	unsigned long accsteppulse, decsteppulse;
	unsigned long accpulsetotal, decpulsetotal;
	unsigned short AccSecond, DecSecond;
	unsigned short i;
	unsigned short flag2 = 0;
	unsigned long DecOverSpeed;
	memset(curspeedpusel, 0, sizeof(SpeedPusleStrct));
	if (currunspeed.StartSpeed < 10)
	{
		currunspeed.StartSpeed = 10;
	}
	if (currunspeed.MaxSpeed < 10)
	{
		currunspeed.MaxSpeed = 10;
	}
	if (currunspeed.CurSpeed < 10)
	{
		currunspeed.CurSpeed = 10;
	}
	currunspeed.CurSpeed *= RATIO / 10;
	currunspeed.StartSpeed *= RATIO / 10;
	currunspeed.MaxSpeed *= RATIO / 10;
	DecOverSpeed = 10 * RATIO / 10;

	AccSecond = 1 + currunspeed.AccSpdTime / 1000;
	DecSecond = 1 + currunspeed.DecSpdTime / 1000;
	if (currunspeed.CurSpeed < currunspeed.StartSpeed) //起始速度小于运行速度
	{
		currunspeed.CurSpeed = currunspeed.StartSpeed;
	}
	accsteppulse = (currunspeed.CurSpeed - currunspeed.StartSpeed);		//加速脉冲数
	accsteppulse = (accsteppulse / currunspeed.AccSpdTime) * AccSecond; //加速步进脉冲值
	if (accsteppulse == 0)
	{
		accsteppulse = 1;
	}
	decsteppulse = (currunspeed.CurSpeed - DecOverSpeed);				//减速脉冲数量
	decsteppulse = (decsteppulse / currunspeed.DecSpdTime) * DecSecond; //减速脉冲步进值
	if (decsteppulse == 0)
	{
		decsteppulse = 1;
	}

	curspeedpusel->AccStepPulse[0] = currunspeed.StartSpeed; //加速脉冲起始值
	curspeedpusel->DecStepPulse[0] = currunspeed.CurSpeed;   //减速脉冲起始值
	for (i = 1; i < ACCDEC_BUF_SIZE; i++)					 //加速赋值
	{
		curspeedpusel->AccStepPulse[i] = curspeedpusel->AccStepPulse[i - 1] + accsteppulse;
		curspeedpusel->DecStepPulse[i] = curspeedpusel->DecStepPulse[i - 1] - decsteppulse;
		if (curspeedpusel->AccStepPulse[i] >= currunspeed.CurSpeed)
		{
			curspeedpusel->AccStepPulse[i] = currunspeed.CurSpeed;
		}
		if (curspeedpusel->DecStepPulse[i] <= DecOverSpeed)
		{
			curspeedpusel->DecStepPulse[i] = DecOverSpeed;
		}
	}
	for (i = 0; i < ACCDEC_BUF_SIZE; i++) //加速赋值
	{
		if (curspeedpusel->AccStepPulse[i] >= currunspeed.CurSpeed)
		{
			curspeedpusel->AccStepPulse[i] = currunspeed.CurSpeed;
			curspeedpusel->AccStep = i;
			break;
		}
	}
	for (i = 0; i < ACCDEC_BUF_SIZE; i++) //加减速赋值
	{
		if (curspeedpusel->DecStepPulse[i] <= DecOverSpeed)
		{
			curspeedpusel->DecStepPulse[i] = DecOverSpeed;
			curspeedpusel->DecStep = i;
			break;
		}
	}
	decpulsetotal = 0; //(DecOverSpeed*DecSecond);
	accpulsetotal = 0; //(currunspeed.StartSpeed*AccSecond);
	curspeedpusel->ContSpeed = currunspeed.CurSpeed;
	for (i = 0; i < ACCDEC_BUF_SIZE; i++) //计算减速脉冲总数
	{
		if (i < curspeedpusel->AccStep)
			accpulsetotal += (curspeedpusel->AccStepPulse[i] * AccSecond);
		if (i <= curspeedpusel->DecStep)
			decpulsetotal += (curspeedpusel->DecStepPulse[i + 1] * DecSecond);
		if (2 * (accpulsetotal + decpulsetotal) >= Pulsemov * 100)
		{
			curspeedpusel->ContSpeed = curspeedpusel->AccStepPulse[i];
			curspeedpusel->AccStep = i;
			flag2 = 1;
			break;
		}
	}
	//accpulsetotal=(currunspeed.CurSpeed/2+currunspeed.StartSpeed/2)*currunspeed.AccSpdTime;
	//decpulsetotal=(currunspeed.CurSpeed/2+DecOverSpeed/2)*currunspeed.DecSpdTime;
	decpulsetotal /= 100;
	accpulsetotal /= 100;

	if (flag2) //防止三角形
	{
		curspeedpusel->AccStepPulse[0] = currunspeed.StartSpeed;
		curspeedpusel->DecStepPulse[0] = curspeedpusel->ContSpeed;
		for (i = 1; i < ACCDEC_BUF_SIZE; i++) //加速赋值
		{
			curspeedpusel->AccStepPulse[i] = curspeedpusel->AccStepPulse[i - 1] + accsteppulse;
			curspeedpusel->DecStepPulse[i] = curspeedpusel->DecStepPulse[i - 1] - decsteppulse;
			if (curspeedpusel->AccStepPulse[i] >= curspeedpusel->ContSpeed)
			{
				curspeedpusel->AccStepPulse[i] = curspeedpusel->ContSpeed;
			}
			if (curspeedpusel->DecStepPulse[i] <= DecOverSpeed)
			{
				curspeedpusel->DecStepPulse[i] = DecOverSpeed;
			}
		}
		for (i = 0; i < ACCDEC_BUF_SIZE; i++) //重新计算减速
		{
			if (curspeedpusel->DecStepPulse[i] <= DecOverSpeed)
			{
				curspeedpusel->DecStepPulse[i] = DecOverSpeed;
				curspeedpusel->DecStep = i;
				break;
			}
		}
		for (i = 0; i < ACCDEC_BUF_SIZE; i++) //加速赋值
		{
			if (curspeedpusel->AccStepPulse[i] >= curspeedpusel->ContSpeed)
			{
				curspeedpusel->AccStep = i;
				break;
			}
		}
		decpulsetotal = (DecOverSpeed * DecSecond);
		accpulsetotal = (currunspeed.StartSpeed * AccSecond);
		for (i = 1; i < ACCDEC_BUF_SIZE; i++) //计算减速脉冲总数
		{
			if (i < curspeedpusel->AccStep)
				accpulsetotal += (curspeedpusel->AccStepPulse[i] * AccSecond);
			if (i <= curspeedpusel->DecStep)
				decpulsetotal += (curspeedpusel->DecStepPulse[i] * DecSecond);
		}
		decpulsetotal /= 100;
		accpulsetotal /= 100;
	}
	for (i = 0; i < ACCDEC_BUF_SIZE; i++)
	{
		caltmp = curspeedpusel->AccStepPulse[i];
		caltmp = PCLK_FREQUENCY / caltmp;
		caltmp /= 10;
		curspeedpusel->AccStepPulse[i] = caltmp;

		caltmp = curspeedpusel->DecStepPulse[i];
		caltmp = PCLK_FREQUENCY / caltmp;
		caltmp /= 10;
		curspeedpusel->DecStepPulse[i] = caltmp;
	}
	caltmp = curspeedpusel->ContSpeed;
	caltmp = PCLK_FREQUENCY / caltmp;
	caltmp /= 10;
	curspeedpusel->ContSpeed = caltmp;
	curspeedpusel->AccDotDistance = accpulsetotal;
	if (Pulsemov > decpulsetotal)
	{
		curspeedpusel->DecDotDistance = Pulsemov - decpulsetotal;
	}
	else
	{
		curspeedpusel->DecDotDistance = 0;
	}
	curspeedpusel->AccStepTime = AccSecond;
	curspeedpusel->DecStepTime = DecSecond;
}
#endif
/************************************************/
/*
计算减速
*/
/************************************************/
#if CURVEACC
void CalDecStop(RunSpeedStrct currunspeed, SpeedPusleStrct *curspeedpusel)
{
	unsigned long long Kacc; //加速度变化率
	unsigned long long Kdec; //减速度变化率

	unsigned long caltmp;
	unsigned long decpulsetotal = 0;
	unsigned short AccSecond, DecSecond;
	unsigned short i;
	unsigned long Acc_n_1, Acc_n;
	if (currunspeed.StartSpeed < 10)
	{
		currunspeed.StartSpeed = 10;
	}
	if (currunspeed.OverSpeed < 10)
	{
		currunspeed.OverSpeed = 10;
	}

	if (currunspeed.CurSpeed < 10)
	{
		currunspeed.CurSpeed = 10;
	}
	currunspeed.CurSpeed *= 10;
	currunspeed.StartSpeed *= 10;
	currunspeed.MaxSpeed *= 10;
	currunspeed.OverSpeed *= 10;
	//
	AccSecond = 1 + currunspeed.AccSpdTime / 1000;
	DecSecond = 1 + currunspeed.DecSpdTime / 1000;

	if (currunspeed.CurSpeed > currunspeed.StartSpeed) //计算出来的当前速度大于起始速度
	{
		Kacc = (4 * (currunspeed.CurSpeed - currunspeed.StartSpeed)) * 1000 / (currunspeed.AccSpdTime);
		Kacc = Kacc * 1000 / currunspeed.AccSpdTime;
		if (Kacc < 1000)
			Kacc = 1000;
		Kdec = (4 * (currunspeed.CurSpeed - currunspeed.OverSpeed)) * 1000 / (currunspeed.DecSpdTime);
		Kdec = Kdec * 1000 / currunspeed.DecSpdTime;
		if (Kdec < 1000)
			Kdec = 1000;
	}
	else
	{
		currunspeed.CurSpeed = currunspeed.StartSpeed;
		Kacc = 0;
		Kdec = 0;
	}
	curspeedpusel->ContSpeed = currunspeed.CurSpeed;
	//
	curspeedpusel->AccStepPulse[0] = currunspeed.StartSpeed; //加速脉冲起始值
	curspeedpusel->DecStepPulse[0] = currunspeed.CurSpeed;   //减速脉冲起始值

	curspeedpusel->AccStep = currunspeed.AccSpdTime / AccSecond;
	curspeedpusel->DecStep = currunspeed.DecSpdTime / DecSecond;
	curspeedpusel->AccStepTime = AccSecond;
	curspeedpusel->DecStepTime = DecSecond;
	Acc_n_1 = 0;
	for (i = 1; i < 1000; i++)
	{
		if (i > curspeedpusel->DecStep)
		{
			curspeedpusel->DecStepPulse[i] = currunspeed.OverSpeed;
		}
		else
		{
			if (i <= curspeedpusel->DecStep / 2)
			{
				Acc_n = Acc_n_1 + (Kdec * AccSecond) / 1000;
			}
			else
			{
				Acc_n = Acc_n_1 - (Kdec * AccSecond) / 1000;
				if (i == curspeedpusel->DecStep)
				{
					Acc_n = 0;
				}
			}
			if (Acc_n != 0)
			{
				if ((i % 2) == 0)
				{

					curspeedpusel->DecStepPulse[i] = curspeedpusel->DecStepPulse[i - 1] - ((Acc_n + Acc_n_1) * AccSecond) / 2000;
				}
				else
				{

					curspeedpusel->DecStepPulse[i] = curspeedpusel->DecStepPulse[i - 1] - ((Acc_n + Acc_n_1) * AccSecond) / 2000 - 1;
				}
			}
			else
			{

				curspeedpusel->DecStepPulse[i] = curspeedpusel->DecStepPulse[i - 1] - ((Acc_n + Acc_n_1) * AccSecond) / 2000;
			}
			if (curspeedpusel->DecStepPulse[i] < currunspeed.OverSpeed)
			{
				curspeedpusel->DecStepPulse[i] = currunspeed.OverSpeed;
				curspeedpusel->DecStep = i;
			}
			decpulsetotal += (curspeedpusel->DecStepPulse[i] * DecSecond);
			Acc_n_1 = Acc_n;
		}
	}
	for (i = 0; i < 1000; i++)
	{
		caltmp = curspeedpusel->DecStepPulse[i];
		caltmp = PCLK_FREQUENCY / caltmp;
		caltmp /= 10;
		curspeedpusel->DecStepPulse[i] = caltmp;
	}
	caltmp = curspeedpusel->ContSpeed;
	caltmp = PCLK_FREQUENCY / caltmp;
	caltmp /= 10;
	curspeedpusel->ContSpeed = caltmp;
}
#else
void CalDecStop(RunSpeedStrct currunspeed, SpeedPusleStrct *curspeedpusel)
{
	unsigned long caltmp;
	unsigned long decsteppulse;
	unsigned long decpulsetotal;
	unsigned short DecSecond;
	unsigned short i;
	unsigned long DecOverSpeed;
	unsigned long DecSpdTimetmp = 0;
	if (currunspeed.StartSpeed < 10)
	{
		currunspeed.StartSpeed = 10;
	}
	if (currunspeed.MaxSpeed < 10)
	{
		currunspeed.MaxSpeed = 10;
	}
	if (currunspeed.CurSpeed < 10)
	{
		currunspeed.CurSpeed = 10;
	}
	currunspeed.CurSpeed *= RATIO / 10;
	currunspeed.StartSpeed *= RATIO / 10;
	currunspeed.MaxSpeed *= RATIO / 10;
	DecOverSpeed = 10 * RATIO / 10;
	DecSpdTimetmp = 200;

	DecSecond = 1 + DecSpdTimetmp / 1000;
	if (currunspeed.CurSpeed < currunspeed.StartSpeed) //起始速度小于运行速度
	{
		currunspeed.CurSpeed = currunspeed.StartSpeed;
	}
	decsteppulse = (currunspeed.CurSpeed - DecOverSpeed);	  //减速脉冲数量
	decsteppulse = (decsteppulse / DecSpdTimetmp) * DecSecond; //减速脉冲步进值
	if (decsteppulse == 0)
	{
		decsteppulse = 1;
	}
	curspeedpusel->DecStepPulse[0] = currunspeed.CurSpeed; //减速脉冲起始值
	for (i = 1; i < ACCDEC_BUF_SIZE; i++)				   //加减速赋值
	{
		curspeedpusel->DecStepPulse[i] = curspeedpusel->DecStepPulse[i - 1] - decsteppulse;
		if (curspeedpusel->DecStepPulse[i] <= DecOverSpeed)
		{
			curspeedpusel->DecStepPulse[i] = DecOverSpeed;
		}
	}
	decpulsetotal = 0;
	for (i = 1; i < ACCDEC_BUF_SIZE; i++) //计算减速脉冲总数
	{
		decpulsetotal += (curspeedpusel->DecStepPulse[i] * DecSecond);
		if (curspeedpusel->DecStepPulse[i] <= DecOverSpeed)
		{
			curspeedpusel->DecStep = i + 20;
			break;
		}
	}
	for (i = 0; i < ACCDEC_BUF_SIZE; i++)
	{
		caltmp = curspeedpusel->DecStepPulse[i];
		caltmp = PCLK_FREQUENCY / caltmp;
		caltmp /= 10;
		curspeedpusel->DecStepPulse[i] = caltmp;
	}
	curspeedpusel->ContSpeed = currunspeed.CurSpeed;
	caltmp = curspeedpusel->ContSpeed;
	caltmp = PCLK_FREQUENCY / caltmp;
	caltmp /= 10;
	curspeedpusel->ContSpeed = caltmp;
	curspeedpusel->DecStepTime = DecSecond;
	curspeedpusel->DecDotDistance = decpulsetotal;
}
#endif
/************************************************/
/*
计算加速
*/
/************************************************/
#if CURVEACC
void CalAccRun(RunSpeedStrct currunspeed, SpeedPusleStrct *curspeedpusel)
{
	unsigned long long Kacc; //加速度变化率
	unsigned long long Kdec; //减速度变化率

	unsigned long caltmp;
	unsigned long accpulsetotal = 0;
	unsigned short AccSecond, DecSecond;
	unsigned short i;
	unsigned long Acc_n_1, Acc_n;
	if (currunspeed.StartSpeed < 10)
	{
		currunspeed.StartSpeed = 10;
	}
	if (currunspeed.OverSpeed < 10)
	{
		currunspeed.OverSpeed = 10;
	}
	if (currunspeed.CurSpeed < 10)
	{
		currunspeed.CurSpeed = 10;
	}
	currunspeed.CurSpeed *= 10;
	currunspeed.StartSpeed *= 10;
	currunspeed.MaxSpeed *= 10;
	currunspeed.OverSpeed *= 10;
	//
	AccSecond = 1 + currunspeed.AccSpdTime / 1000;
	DecSecond = 1 + currunspeed.DecSpdTime / 1000;

	if (currunspeed.CurSpeed > currunspeed.StartSpeed) //计算出来的当前速度大于起始速度
	{
		Kacc = (4 * (currunspeed.CurSpeed - currunspeed.StartSpeed)) * 1000 / (currunspeed.AccSpdTime);
		Kacc = Kacc * 1000 / currunspeed.AccSpdTime;
		if (Kacc < 1000)
			Kacc = 1000;
		Kdec = (4 * (currunspeed.CurSpeed - currunspeed.OverSpeed)) * 1000 / (currunspeed.DecSpdTime);
		Kdec = Kdec * 1000 / currunspeed.DecSpdTime;
		if (Kdec < 1000)
			Kdec = 1000;
	}
	else
	{
		currunspeed.CurSpeed = currunspeed.StartSpeed;
		Kacc = 0;
		Kdec = 0;
	}
	curspeedpusel->ContSpeed = currunspeed.CurSpeed;
	//
	curspeedpusel->AccStepPulse[0] = currunspeed.StartSpeed; //加速脉冲起始值
	curspeedpusel->DecStepPulse[0] = currunspeed.CurSpeed;   //减速脉冲起始值

	curspeedpusel->AccStep = currunspeed.AccSpdTime / AccSecond;
	curspeedpusel->DecStep = currunspeed.DecSpdTime / DecSecond;
	curspeedpusel->AccStepTime = AccSecond;
	curspeedpusel->DecStepTime = DecSecond;
	Acc_n_1 = 0;
	//计算加速部分的速度表
	for (i = 1; i < 1000; i++)
	{
		if (i > curspeedpusel->AccStep)
		{
			curspeedpusel->AccStepPulse[i] = currunspeed.CurSpeed;
		}
		else
		{
			if (i <= curspeedpusel->AccStep / 2)
			{
				Acc_n = Acc_n_1 + (Kacc * AccSecond) / 1000;
			}
			else
			{
				Acc_n = Acc_n_1 - (Kacc * AccSecond) / 1000;
				if (i == curspeedpusel->AccStep)
				{
					Acc_n = 0;
				}
			}
			if (Acc_n != 0)
			{
				if ((i % 2) == 0)
				{
					curspeedpusel->AccStepPulse[i] = curspeedpusel->AccStepPulse[i - 1] + ((Acc_n + Acc_n_1) * AccSecond) / 2000;
				}
				else
				{
					curspeedpusel->AccStepPulse[i] = curspeedpusel->AccStepPulse[i - 1] + ((Acc_n + Acc_n_1) * AccSecond) / 2000 + 1;
				}
			}
			else
			{
				curspeedpusel->AccStepPulse[i] = curspeedpusel->AccStepPulse[i - 1] + ((Acc_n + Acc_n_1) * AccSecond) / 2000;
			}
			if (curspeedpusel->AccStepPulse[i] > currunspeed.CurSpeed)
			{
				curspeedpusel->AccStepPulse[i] = currunspeed.CurSpeed;
				curspeedpusel->AccStep = i;
			}
			accpulsetotal += curspeedpusel->AccStepPulse[i] * AccSecond;
			Acc_n_1 = Acc_n;
		}
	}
	for (i = 0; i < 1000; i++)
	{
		caltmp = curspeedpusel->AccStepPulse[i];
		caltmp = PCLK_FREQUENCY / caltmp;
		caltmp /= 10;
		curspeedpusel->AccStepPulse[i] = caltmp;
	}
	caltmp = curspeedpusel->ContSpeed;
	caltmp = PCLK_FREQUENCY / caltmp;
	caltmp /= 10;
	curspeedpusel->ContSpeed = caltmp;
}
#else
void CalAccRun(RunSpeedStrct currunspeed, SpeedPusleStrct *curspeedpusel)
{
	unsigned long caltmp;
	unsigned long accsteppulse;
	unsigned long accpulsetotal;
	unsigned short AccSecond;
	unsigned short i;
	if (currunspeed.StartSpeed < 10)
	{
		currunspeed.StartSpeed = 10;
	}
	if (currunspeed.MaxSpeed < 10)
	{
		currunspeed.MaxSpeed = 10;
	}
	if (currunspeed.CurSpeed < 10)
	{
		currunspeed.CurSpeed = 10;
	}
	currunspeed.CurSpeed *= RATIO / 10;
	currunspeed.StartSpeed *= RATIO / 10;
	currunspeed.MaxSpeed *= RATIO / 10;

	AccSecond = 1 + currunspeed.AccSpdTime / 1000;
	if (currunspeed.CurSpeed < currunspeed.StartSpeed) //起始速度小于运行速度
	{
		currunspeed.CurSpeed = currunspeed.StartSpeed;
	}
	accsteppulse = (currunspeed.CurSpeed - currunspeed.StartSpeed);		//加速脉冲数
	accsteppulse = (accsteppulse / currunspeed.AccSpdTime) * AccSecond; //加速步进脉冲值
	if (accsteppulse == 0)
	{
		accsteppulse = 1;
	}
	curspeedpusel->AccStepPulse[0] = currunspeed.StartSpeed; //加速脉冲起始值
	for (i = 1; i < ACCDEC_BUF_SIZE; i++)					 //加减速赋值
	{
		curspeedpusel->AccStepPulse[i] = curspeedpusel->AccStepPulse[i - 1] + accsteppulse;
		if (curspeedpusel->AccStepPulse[i] >= currunspeed.CurSpeed)
		{
			curspeedpusel->AccStepPulse[i] = currunspeed.CurSpeed;
		}
	}
	accpulsetotal = (currunspeed.StartSpeed * AccSecond) / 100;
	for (i = 1; i < ACCDEC_BUF_SIZE; i++) //计算加速脉冲总数
	{
		accpulsetotal += (curspeedpusel->AccStepPulse[i] * AccSecond) / 100;
		if (curspeedpusel->AccStepPulse[i] >= currunspeed.CurSpeed)
		{
			curspeedpusel->AccStep = i;
			curspeedpusel->ContSpeed = currunspeed.CurSpeed;
			break;
		}
	}
	for (i = 0; i < ACCDEC_BUF_SIZE; i++)
	{
		caltmp = curspeedpusel->AccStepPulse[i];
		caltmp = PCLK_FREQUENCY / caltmp;
		caltmp /= 10;
		curspeedpusel->AccStepPulse[i] = caltmp;
	}
	caltmp = curspeedpusel->ContSpeed;
	caltmp = PCLK_FREQUENCY / caltmp;
	caltmp /= 10;
	curspeedpusel->ContSpeed = caltmp;
	curspeedpusel->AccDotDistance = accpulsetotal;
	curspeedpusel->AccStepTime = AccSecond;
}
#endif
/************************************************/
/*
A轴脉冲启动
*/
/************************************************/
void A_Pulse_Start()
{
	APwmStepcounter = 0;
	A_Frecy = 0;
	A_DRV = 1;
	A_AccStep = 0;
	A_DecStep = 1;
	APluseMoveCount = 0;
	A_Pulse_StartFlg = 1;
}
/************************************************/
/*
B轴脉冲启动
*/
/************************************************/
void B_Pulse_Start()
{
	BPwmStepcounter = 0;
	B_Frecy = 0;
	B_DRV = 1;
	B_AccStep = 0;
	B_DecStep = 1;
	BPluseMoveCount = 0;
	B_Pulse_StartFlg = 1;
}
/************************************************/
/*
A轴脉冲停止
*/
/************************************************/
void A_Pulse_Stop()
{
	A_PStart = 0;
	APwmStepcounter = 0;
	APluseDecCount = 0;
	APluseMoveDist = 0;
	A_AccStep = 0;
	A_DecStep = 0;
	A_Frecy = 0;
	A_Cnter = 0;
	A_CRun = 0;
	A_DStop = 0;
	A_DRV = 0;
	A_LimitStop = 0;
	APluseMoveCount = 0;
	AAxis_TCNT = 0x0000;
	A_Pulse_StartFlg = 0;
	A_AXIS_PULSE_OFF();
}
/************************************************/
/*
B轴脉冲停止
*/
/************************************************/
void B_Pulse_Stop()
{
	B_PStart = 0;
	BPwmStepcounter = 0;
	BPluseDecCount = 0;
	BPluseMoveDist = 0;
	B_AccStep = 0;
	B_DecStep = 0;
	B_Frecy = 0;
	B_Cnter = 0;
	B_CRun = 0;
	B_DStop = 0;
	B_DRV = 0;
	B_LimitStop = 0;
	BPluseMoveCount = 0;
	BAxis_TCNT = 0x0000;
	B_Pulse_StartFlg = 0;
	B_AXIS_PULSE_OFF();
}
/************************************************/
/*
A轴软件限位
*/
/************************************************/
void ASoftLimitProc(void)
{
	//unsigned long pmov;
	if (A_SLMTNEN)
	{
		if (ASoftLinitNeg < A_DistanceCount)
		{
			A_SLMTN = 0;
		}
		if ((A_DistanceCount < ASoftLinitNeg) && A_Dir)
		{
			if (A_SLMTN == 0)
				MV_Limit_Stop(A_AXIS);
			A_SLMTN = 1;
		}
	}
	else if (!A_SLMTNEN)
	{
		A_SLMTN = 0;
	}
	if (A_SLMTPEN)
	{
		if (ASoftLinitPos > A_DistanceCount)
		{
			A_SLMTP = 0;
		}
		if ((A_DistanceCount > ASoftLinitPos) && !A_Dir)
		{
			if (A_SLMTP == 0)
				MV_Limit_Stop(A_AXIS);
			A_SLMTP = 1;
		}
	}
	else if (!A_SLMTPEN)
	{
		A_SLMTP = 0;
	}
}
/************************************************/
/*
B轴软件限位
*/
/************************************************/
void BSoftLimitProc(void)
{
	//unsigned long pmov;
	if (B_SLMTNEN)
	{
		if (BSoftLinitNeg < B_DistanceCount)
		{
			B_SLMTN = 0;
		}
		if ((B_DistanceCount < BSoftLinitNeg) && B_Dir)
		{
			if (B_SLMTN == 0)
				MV_Limit_Stop(B_AXIS);
			B_SLMTN = 1;
		}
	}
	else if (!B_SLMTNEN)
	{
		B_SLMTN = 0;
	}
	if (B_SLMTPEN)
	{
		if (BSoftLinitPos > B_DistanceCount)
		{
			B_SLMTP = 0;
		}
		if ((B_DistanceCount > BSoftLinitPos) && !B_Dir)
		{
			if (B_SLMTP == 0)
				MV_Limit_Stop(B_AXIS);
			B_SLMTP = 1;
		}
	}
	else if (!B_SLMTPEN)
	{
		B_SLMTP = 0;
	}
}
/************************************************/
/*
A轴脉冲程序
*/
/************************************************/
void APwmInterruptProgram(void)
{
	if (A_PStart)
	{
		if (A_Dir)
			A_DistanceCount--;
		else
			A_DistanceCount++;
//		ASoftLimitProc();
	}
	if (A_Cnter)
	{
		APluseMoveCount++;
		if (APluseMoveCount < ACurSpeedPusel.AccDotDistance) //加速区
		{
			A_ACC = 1;
			A_DeC = 0;
			if (A_Frecy)
			{
				A_TGRA = ACurSpeedPusel.AccStepPulse[A_AccStep];
				A_TGRB = ACurSpeedPusel.AccStepPulse[A_AccStep] / 2;
				A_AccStep++;
				A_Frecy = 0;
			}
		}
		else if ((APluseMoveCount >= ACurSpeedPusel.AccDotDistance) && (APluseMoveCount < ACurSpeedPusel.DecDotDistance)) //匀速区
		{
			A_ACC = 0;
			A_DeC = 0;
			if (A_Frecy)
			{
				A_TGRA = ACurSpeedPusel.ContSpeed;
				A_TGRB = ACurSpeedPusel.ContSpeed / 2;
				A_Frecy = 0;
			}
		}
		else if ((APluseMoveCount >= ACurSpeedPusel.DecDotDistance) //减速区
				 && (APluseMoveCount < APluseMoveDist))
		{
			A_ACC = 0;
			A_DeC = 1;
			if (A_Frecy)
			{
				A_TGRA = ACurSpeedPusel.DecStepPulse[A_DecStep];
				A_TGRB = ACurSpeedPusel.DecStepPulse[A_DecStep] / 2;
				A_Frecy = 0;
				A_DecStep++;
			}
		}
		if (APluseMoveCount >= APluseMoveDist) //停止去
		{
			A_ACC = 0;
			A_DeC = 0;
			A_Pulse_Stop();
		}
	}
	else
	{
		if (A_CRun) //运行
		{
			if (A_AccStep <= ACurSpeedPusel.AccStep) //加速去
			{
				A_ACC = 1;
				if (A_Frecy)
				{
					A_TGRA = ACurSpeedPusel.AccStepPulse[A_AccStep];
					A_TGRB = ACurSpeedPusel.AccStepPulse[A_AccStep] / 2;
					A_AccStep++;
					A_Frecy = 0;
				}
			}
			else //匀速区
			{
				A_TGRA = ACurSpeedPusel.ContSpeed;
				A_TGRB = ACurSpeedPusel.ContSpeed / 2;
			}
		}
		else if (A_DStop)
		{
			if (A_DecStep <= ACurSpeedPusel.DecStep)
			{
				if (A_Frecy) //减速区
				{
					A_TGRA = ACurSpeedPusel.DecStepPulse[A_DecStep];
					A_TGRB = ACurSpeedPusel.DecStepPulse[A_DecStep] / 2;
					A_DecStep++;
					A_Frecy = 0;
				}
			}
			else
			{
				A_Pulse_Stop();
			}
		}
		else if (A_LimitStop) //限位停止
		{
			if (A_DecStep <= ACurSpeedPusel.DecStep)
			{
				A_TGRA = ACurSpeedPusel.DecStepPulse[A_DecStep];
				A_TGRB = ACurSpeedPusel.DecStepPulse[A_DecStep] / 2;
			}
			else
			{
				A_Pulse_Stop();
			}
			if (ACurSpeedPusel.DecStep > 5)
			{
				A_DecStep += ACurSpeedPusel.DecStep / 5;
			}
			APluseDecCount++;
			if (APluseDecCount >= 5)
			{
				APluseDecCount = 0;
				A_Pulse_Stop();
			}
		}
	}
}
/************************************************/
/*
B轴脉冲程序
*/
/************************************************/
void BPwmInterruptProgram(void)
{
	if (B_PStart)
	{
		if (B_Dir)
			B_DistanceCount--;
		else
			B_DistanceCount++;
//		BSoftLimitProc();
	}
	if (B_Cnter)
	{
		BPluseMoveCount++;
		if (BPluseMoveCount < BCurSpeedPusel.AccDotDistance) //加速区
		{
			B_ACC = 1;
			B_DeC = 0;
			if (B_Frecy)
			{
				B_TGRA = BCurSpeedPusel.AccStepPulse[B_AccStep];
				B_TGRB = BCurSpeedPusel.AccStepPulse[B_AccStep] / 2;
				B_AccStep++;
				B_Frecy = 0;
			}
		}
		else if ((BPluseMoveCount >= BCurSpeedPusel.AccDotDistance) && (BPluseMoveCount < BCurSpeedPusel.DecDotDistance)) //匀速区
		{
			B_ACC = 0;
			B_DeC = 0;
			if (B_Frecy)
			{
				B_TGRA = BCurSpeedPusel.ContSpeed;
				B_TGRB = BCurSpeedPusel.ContSpeed / 2;
				B_Frecy = 0;
			}
		}
		else if ((BPluseMoveCount >= BCurSpeedPusel.DecDotDistance) //减速区
				 && (BPluseMoveCount < BPluseMoveDist))
		{
			B_ACC = 0;
			B_DeC = 1;
			if (B_Frecy)
			{
				B_TGRA = BCurSpeedPusel.DecStepPulse[B_DecStep];
				B_TGRB = BCurSpeedPusel.DecStepPulse[B_DecStep] / 2;
				B_Frecy = 0;
				B_DecStep++;
			}
		}
		if (BPluseMoveCount >= BPluseMoveDist) //停止去
		{
			B_ACC = 0;
			B_DeC = 0;
			B_Pulse_Stop();
		}
	}
	else
	{
		if (B_CRun) //运行
		{
			if (B_AccStep <= BCurSpeedPusel.AccStep) //加速去
			{
				B_ACC = 1;
				if (B_Frecy)
				{
					B_TGRA = BCurSpeedPusel.AccStepPulse[B_AccStep];
					B_TGRB = BCurSpeedPusel.AccStepPulse[B_AccStep] / 2;
					B_AccStep++;
					B_Frecy = 0;
				}
			}
			else //匀速区
			{
				B_TGRA = BCurSpeedPusel.ContSpeed;
				B_TGRB = BCurSpeedPusel.ContSpeed / 2;
			}
		}
		else if (B_DStop)
		{
			if (B_DecStep <= BCurSpeedPusel.DecStep)
			{
				if (B_Frecy) //减速区
				{
					B_TGRA = BCurSpeedPusel.DecStepPulse[B_DecStep];
					B_TGRB = BCurSpeedPusel.DecStepPulse[B_DecStep] / 2;
					B_DecStep++;
					B_Frecy = 0;
				}
			}
			else
			{
				B_Pulse_Stop();
			}
		}
		else if (B_LimitStop) //限位停止
		{
			if (B_DecStep <= BCurSpeedPusel.DecStep)
			{
				B_TGRA = BCurSpeedPusel.DecStepPulse[B_DecStep];
				B_TGRB = BCurSpeedPusel.DecStepPulse[B_DecStep] / 2;
			}
			else
			{
				B_Pulse_Stop();
			}
			if (BCurSpeedPusel.DecStep > 5)
			{
				B_DecStep += BCurSpeedPusel.DecStep / 5;
			}
			BPluseDecCount++;
			if (BPluseDecCount >= 5)
			{
				BPluseDecCount = 0;
				B_Pulse_Stop();
			}
		}
	}
}
/************************************************/
/*
A轴硬件限位
*/
/************************************************/
void AAxisHLimitProc(void)
{
	if (A_HLMTN || A_HLMTP)
	{
		if (A_LimitDec)
		{
			MV_Sudden_Stop(A_AXIS);
		}
		else
		{
			if (A_HLimitFlg == 0)
				MV_Limit_Stop(A_AXIS);
			A_HLimitFlg = 1;
		}
	}
	else
	{
		A_HLimitFlg = 0;
	}
}
/************************************************/
/*
B轴硬件限位
*/
/************************************************/
void BAxisHLimitProc(void)
{
	if (B_HLMTN || B_HLMTP)
	{
		if (B_LimitDec)
		{
			MV_Sudden_Stop(B_AXIS);
		}
		else
		{
			if (B_HLimitFlg == 0)
				MV_Limit_Stop(B_AXIS);
			B_HLimitFlg = 1;
		}
	}
	else
	{
		B_HLimitFlg = 0;
	}
}
/************************************************/
/*
A轴驱动器告警
*/
/************************************************/
void AAxisDrvArmProc(void)
{
	if (A_ALARMEN)
	{
		if (A_SAlarmIn)
		{
			A_ALARM = 1;
			MV_Sudden_Stop(A_AXIS);
		}
		else
		{
			A_ALARM = 0;
		}
	}
	else
	{
		A_ALARM = 0;
	}
}
/************************************************/
/*
B轴驱动器告警
*/
/************************************************/
void BAxisDrvArmProc(void)
{
	if (B_ALARMEN)
	{
		if (B_SAlarmIn)
		{
			B_ALARM = 1;
			MV_Sudden_Stop(B_AXIS);
		}
		else
		{
			B_ALARM = 0;
		}
	}
	else
	{
		B_ALARM = 0;
	}
}

/************************************************/
/*
驱动器保护
*/
/************************************************/
void ServoDrvProct(void)
{

	{
		AAxisDrvArmProc();
		AAxisHLimitProc();
		ASoftLimitProc();
	}

	{
		BAxisDrvArmProc();
		BAxisHLimitProc();
		BSoftLimitProc();
	}
}
/************************************************/
/*
定时启动发送脉冲
*/
/************************************************/
void TimerPulseStart(void)
{
	if (A_Pulse_StartFlg)
	{
		A_Pulse_StartFlg = 0;
		A_PStart = 1;
		A_AXIS_PULSE_ON();
	}
	if (B_Pulse_StartFlg)
	{
		B_Pulse_StartFlg = 0;
		B_PStart = 1;
		B_AXIS_PULSE_ON();
	}
	if (A_PStart)
	{
		if (A_CRun || A_ACC)
		{
			APwmStepcounter++;
			if (APwmStepcounter >= ACurSpeedPusel.AccStepTime)
			{
				A_Frecy = 1;
			}
		}
		if (A_DeC || A_DStop)
		{
			APwmStepcounter++;
			if (APwmStepcounter >= ACurSpeedPusel.DecStepTime)
			{
				A_Frecy = 1;
			}
		}
	}
	if (B_PStart)
	{
		if (B_CRun || B_ACC)
		{
			BPwmStepcounter++;
			if (BPwmStepcounter >= BCurSpeedPusel.AccStepTime)
			{
				B_Frecy = 1;
			}
		}
		if (B_DeC || B_DStop)
		{
			BPwmStepcounter++;
			if (BPwmStepcounter >= BCurSpeedPusel.DecStepTime)
			{
				B_Frecy = 1;
			}
		}
	}
}
