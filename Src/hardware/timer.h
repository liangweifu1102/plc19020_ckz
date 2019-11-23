#ifndef USER_TIMER_H
#define USER_TIMER_H

#include "../mylib/typedefine.h"
#include "../modbus/ModbusSlave.h"

extern uint32 dwTickCount;
extern void SysTick_Configuration(void);
extern void TIM7_Timer_Init(void);

#endif
