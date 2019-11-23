
#include "../stm32f4lib/stm32f4xx.h"
#include "delay.h"

/*
*********************************************************************************************************
*                                             寄存器
*********************************************************************************************************
*/
#define DWT_CYCCNT *(volatile unsigned int *)0xE0001004
#define DWT_CR *(volatile unsigned int *)0xE0001000
#define DEM_CR *(volatile unsigned int *)0xE000EDFC
#define DBGMCU_CR *(volatile unsigned int *)0xE0042004

#define DEM_CR_TRCENA (1 << 24)
#define DWT_CR_CYCCNTENA (1 << 0)

static volatile uint32 systemCoreClock = 120000000; //系统时钟

void Delay_Configuration(uint32 clk)
{
    systemCoreClock = clk * 1000000;
    DEM_CR |= (unsigned int)DEM_CR_TRCENA; /* Enable Cortex-M4's DWT CYCCNT reg.  */
    DWT_CYCCNT = (unsigned int)0u;
    DWT_CR |= (unsigned int)DWT_CR_CYCCNTENA;
}

void delay_us(int usec)
{
    uint32_t tEnd, tDelayCnt;
    uint32_t tStart;

    tStart = DWT_CYCCNT; /* 刚进入时的计数器值 */

    tDelayCnt = usec * (systemCoreClock / 1000000); /* 需要的节拍数 */

    tEnd = tStart + tDelayCnt;

    // while ((DWT_CYCCNT - tStart) < tDelayCnt)
    // {
    //     ;
    // }
    if (tEnd > tStart)
    {
        while (DWT_CYCCNT < tEnd)
            ;
    }
    else
    {
        while (DWT_CYCCNT >= tEnd)
            ; // 翻转后的值不会比ticks_end小
        while (DWT_CYCCNT < tEnd)
            ;
    }
}

void HAL_Delay(__IO uint32_t Delay)
{
    delay_ms(Delay);
}

void delay_ms(int msec)
{
    delay_us(msec * 1000);
}
