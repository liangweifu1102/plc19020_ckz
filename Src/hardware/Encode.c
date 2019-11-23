/*
 * @Author: mikey.zhaopeng 
 * @Date: 2018-12-20 19:51:02 
 * @Last Modified by:   mikey.zhaopeng 
 * @Last Modified time: 2018-12-20 19:51:02 
 */
#include "Encode.h"
#include "../app/usermemory.h"
#include "../app/AutoRun.h"
#include "../hardware/pwm.h"

static volatile long ulEncoderCount @".ccram";
volatile long CheckSgnFilter = 0;
volatile long CheckSgnFilterOff = 0;

//volatile 
unsigned char flgSgn = 0;
//volatile 
unsigned char flgSgnOld = 0;

long getEncoderCount(void)
{
    return ulEncoderCount;
}

void setEncoderCount(long val)
{
    ulEncoderCount = val;
}

void EncoderInit(void)
{
    RCC_AHB1_GPIO_EN(GPIO_C); //使能PORTF时钟
    RCC_APB1_TIM_EN(APB1_TIM3);

    GPIO_Set(GPIOC, PIN6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_NONE);
    GPIO_Set(GPIOC, PIN7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_NONE); //
    GPIO_AF_Set(GPIOC, 6, PIN_AF2);
    GPIO_AF_Set(GPIOC, 7, PIN_AF2);

    MY_NVIC_Init(0, 2, TIM3_IRQn); //组2，最低优先级

    TIM3->CR1 = 0;
    TIM3->CR2 = 0;
    TIM3->CNT = 0;
    TIM3->CCER = 0;
    TIM3->CCMR1 = 0;
    TIM3->SMCR = 0;
    TIM3->DIER = 0;
    TIM3->PSC = 0; //59; // APB1 60M  1us 一次

    TIM3->CR1 |= (0 << 1); // 使能更新事件
    TIM3->CR1 |= (0 << 8);// 一分频
    TIM3->CR1 |= (0 << 7);// 不进行缓冲
    TIM3->CR1 |= (0 << 5);// 边沿对齐
    TIM3->CR1 |= (0 << 4);// 递增计数
    TIM3->CR1 |= (0 << 3);// 发生更新时不停止计数
    TIM3->CR1 |= (1 << 2);// 只有计数溢出产生中断
    
    TIM3->ARR = 1;

    TIM3->DIER |= 1; // 允许更新中断

    TIM3->SMCR |= (3 << 0); // 编码器模式1
    TIM3->CCMR1 |= (1 << 8);// CH2配置为输入TI2
    TIM3->CCMR1 |= (0 << 10);// 无分频
    TIM3->CCMR1 |= (8 << 12);// 8次滤波设置
    TIM3->CCMR1 |= (1 << 0);// CH1配置为输入TI1
    TIM3->CCMR1 |= (0 << 2);// 无分频
    TIM3->CCMR1 |= (8 << 4);// 8次滤波设置
    TIM3->CCER |= (1 << 5) | (1 << 1);             //CH1 CH2 输入捕获  上升沿有效

    TIM3->CCER |= (1 << 0);//CH1 开启输入捕获
    TIM3->CCER |= (1 << 4);//CH2 开启输入捕获
    TIM3->CR1 |= 1 << 0;
}
static int8 EncodePer = 2;
void EncoderDirSet(void)
{
    if (SystemParam->EncodeDirection)
    {
        EncodePer *= 1;
    }
    else
    {
        EncodePer *= -1;
    }
}

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & TIM_SR_UIF)
    {
        if (TIM3->CR1 & TIM_CR1_DIR)
        {
            ulEncoderCount -= EncodePer;
        }
        else
        {
            ulEncoderCount += EncodePer;
        }

        TIM3->SR &= ~TIM_SR_UIF;
    }
}

void TIM7_Timer_Init(void)
{
    RCC_APB1_TIM_EN(APB1_TIM7);

    TIM7->DIER = 0;
    TIM7->CR1 = 0;
    TIM7->CR1 |= (1 << 7) | (0 << 3) | (1 << 2) | (0 << 1); // 自动重载

    TIM7->PSC = 60 - 1; // 对应1M
    TIM7->ARR = 10;
    // 10us
    TIM7->DIER |= 1; // 允许更新中断

    MY_NVIC_Init(0, 1, TIM7_IRQn); //组2，最低优先级

    TIM7->CR1 |= TIM_CR1_CEN;
}

/************************************************/
/*
定时器中断，
*/
/************************************************/
char tim7_temp @"ccram" = 0;
void TIM7_IRQHandler(void)
{ // 10us一次
    if (StartFlag > 0)
    {
        if (GPIOD->IDR & PIN4)
        {
            CheckSgnFilterOff++;
            if (CheckSgnFilterOff > 1)
            {
                flgSgn = 0;
                CheckSgnFilter = 0;
            }
        }
        else
        {
            CheckSgnFilter++;
            if (CheckSgnFilter > 1)
            {
                flgSgn = 1;
                CheckSgnFilterOff = 0;
            }
        }

        if (flgSgn != flgSgnOld)
        {
            flgSgnOld = flgSgn;
            if (flgSgn > 0)
            { // 上升沿
                tim7_temp = CheckHoleNum[0];
                HoleinfoB[tim7_temp] = ulEncoderCount;
                CheckHoleNum[0]++;
            }
            else
            { // 下降沿
                tim7_temp = CheckHoleNum[1];
                HoleinfoF[tim7_temp] = ulEncoderCount;
                CheckHoleNum[1]++;
            }
            if (CheckHoleNum[0] > 99) CheckHoleNum[0] = 99;
            if (CheckHoleNum[1] > 99) CheckHoleNum[1] = 99;
        }
    }

    TIM7->SR = 0;
}
