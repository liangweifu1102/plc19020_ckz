#include "timer.h"
#include "../app/Action.h"

#include "pwm.h"
#include "../mylib/sys.h"

uint32 dwTickCount = 0;

/*******************************************************************************
 * Function Name  : SysTick_Configuration
 * Description    : Configures the SysTick for OS tick.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void SysTick_Configuration(void)
{
    uint32_t cnts;

    dwTickCount = 0;
    cnts = 120000000;
    cnts /= 1000;             //每秒钟的计数次数 单位为M
    SysTick->LOAD = cnts - 1; //每1/delay_ostickspersec秒中断一次
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
    //SysTick_Config(cnts);
    //SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    SysTick->VAL = 1;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk |
                    SysTick_CTRL_ENABLE_Msk;
    
    dwTickCount = 0;
}
/**
 * 系统计时时钟
 * 
 * @author xt (2019/7/3)
 * 
 * @param void 
 */
void SysTick_Handler(void)
{
    dwTickCount++;

    if (ServoDelay)
    {
        ServoDelay--;
    }
    if (RstPphlDlay)
    {
        RstPphlDlay--;
    }
    if(bpq_alarm_time)bpq_alarm_time--;
}
