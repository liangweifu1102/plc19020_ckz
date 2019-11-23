#ifndef __DELAY_H__
#define __DELAY_H__
#include "sys.h"
void Delay_Configuration(u32 clk);
void delay_us(int usec);
void delay_ms(int msec) ;
void counter_start(int n);
void counter_stop(int n);
u32 counter_read(int n);

#endif
