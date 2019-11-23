#include "../mylib/sys.h"

#define __RUNLED_CLK_ENABLE() __HAL_RCC_GPIOG_CLK_ENABLE()
#define RUNLED_GPIO_PORT GPIOG
#define RUNLED_PIN PIN11

#define RunLed1 PGout(11)
//#define RunLed2 PGout(5)

void RunLedInit(void)
{
    GPIO_Set(RUNLED_GPIO_PORT, RUNLED_PIN, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PU); 
    //GPIO_Set(GPIOG, PIN5, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PU); 
}

/************************************************/
/*
运行灯
*/
/************************************************/
#define BlinkTime 100

void BlinkRunLED(void)
{
    static u32 flashTick = 0;
    if (flashTick < dwTickCount)
    {
        RunLed1 = !RunLed1;
        //RunLed2 = !RunLed2;
        flashTick = dwTickCount + BlinkTime; 
    }
}

//void RunLedProc(void)
//{
//    static uint32 ledtick = 0;
//    static uint8 ledflag = 0;
//    uint32 dwtick = HAL_GetTick();
//
//    if (dwtick > ledtick)
//    {
//        ledtick = dwtick + 10;
//        ledflag = ~ledflag;
//        if (ledflag > 0)
//        {
//            HAL_GPIO_WritePin(RUNLED_GPIO_PORT, RUNLED_PIN, GPIO_PIN_SET);
//        }
//        else
//        {
//            HAL_GPIO_WritePin(RUNLED_GPIO_PORT, RUNLED_PIN, GPIO_PIN_RESET);
//        }
//    }
//    return;
//
////    if (dwtick > (ledtick + 20))
////    {
////        ledtick = dwtick;
////    }
////    else if (dwtick > (ledtick + 10))
////    {
////        HAL_GPIO_WritePin(RUNLED_GPIO_PORT, RUNLED_PIN, GPIO_PIN_SET);
////    }
////    else if (dwtick > ledtick)
////    {
////        HAL_GPIO_WritePin(RUNLED_GPIO_PORT, RUNLED_PIN, GPIO_PIN_RESET);
////    }
//}