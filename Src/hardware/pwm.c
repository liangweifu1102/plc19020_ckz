#include "servostep.h"
#include "../app/usermemory.h"
//#include "servodrv.h"

void TIM13_PWM_Init(void)
{
    RCC_AHB1_GPIO_EN(GPIO_F); //PF
    RCC_AHB1_GPIO_EN(GPIO_B); //ʹ��PORTFʱ��
    RCC_AHB1_GPIO_EN(GPIO_A); //ʹ��PORTFʱ��

    GPIO_Set(GPIOF, PIN9, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOF, PIN10, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOA, PIN2, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOB, PIN0, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);

    GPIO_Set(GPIOF, PIN8, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //���ù���,�������
    GPIO_AF_Set(GPIOF, 8, PIN_AF9);

    RCC_APB1_TIM_EN(APB1_TIM13);

    TIM13->CR1 = 0;
    TIM13->CR1 &= TIM_CR1_CKD; // 1分频
    TIM13->CR1 &= ~TIM_CR1_ARPE; // 不缓冲
    TIM13->CR1 |= TIM_CR1_URS; // 只有计数上溢
    TIM13->CR1 &= ~TIM_CR1_UDIS; // 使能更新中断生成

    TIM13->PSC = 0;
    TIM13->CCER = 0;
    TIM13->EGR = 0;

    TIM13->DIER = 0;
    TIM13->DIER |= 1; // ���������ж�

    TIM13->CCMR1 = 0;
    TIM13->CCMR1 |= (6 << 4); // PWM1 模式
    TIM13->CCMR1 |= (0 << 3); // 禁止预装载
    TIM13->CCMR1 |= (1 << 2); // 快速比较输出使能
    TIM13->CCMR1 |= (0 << 0); // 输出模式

    TIM13->CCER = 0;
    TIM13->CCER |= 1 << 0; //OC1 输出使能
    TIM13->CCER |= 0 << 1; //OC1 高电平有效

    MY_NVIC_Init(1, 2, TIM8_UP_TIM13_IRQn); //��2��������ȼ�
}
//TIM8 PWM���ֳ�ʼ��
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
// APB2 timer clock 120M, ��������60M
void TIM11_PWM_Init(void)
{
    RCC_AHB1_GPIO_EN(GPIO_F); //ʹ��PORTFʱ��
    RCC_AHB1_GPIO_EN(GPIO_C); //ʹ��PORTFʱ��

    GPIO_Set(GPIOF, PIN5, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOC, PIN15, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOC, PIN3, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);

    GPIO_Set(GPIOF, PIN7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //���ù���,�������
    GPIO_AF_Set(GPIOF, 7, PIN_AF3);

    RCC_APB2_TIM_EN(APB2_TIM11);

    TIM11->CR1 = 0;
    TIM11->CR1 |= (0 << 8) | (0 << 7) | (1 << 2) | (0 << 1); // ��ֹ�Զ�����

    TIM11->PSC = 0;
    TIM11->CCER = 0;
    TIM11->EGR = 0;

    TIM11->DIER = 0;
    TIM11->DIER |= 1; // ���������ж�

    TIM11->CCMR1 = 0;
    TIM11->CCMR1 |= (6 << 4) | (0 << 3) | (1 << 2) | (0 << 0); //CH1 PWM1ģʽ

    TIM11->CCER = 0;
    TIM11->CCER |= 1 << 0; //OC1 ���ʹ��
    TIM11->CCER |= 0 << 1; //OC1 �͵�ƽ��Ч

    MY_NVIC_Init(1, 2, TIM1_TRG_COM_TIM11_IRQn); //��2��������ȼ�
}

void TIM10_PWM_Init(void)
{
    RCC_AHB1_GPIO_EN(GPIO_F); //ʹ��PORTFʱ��
    RCC_AHB1_GPIO_EN(GPIO_C); //ʹ��PORTFʱ��
    RCC_AHB1_GPIO_EN(GPIO_A);

    GPIO_Set(GPIOC, PIN13, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOC, PIN2, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOA, PIN6, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOF, PIN0, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);

    GPIO_Set(GPIOF, PIN6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //���ù���,�������
    GPIO_AF_Set(GPIOF, 6, PIN_AF3);

    RCC_APB2_TIM_EN(APB2_TIM10);

    TIM10->CR1 = 0;
    TIM10->CR1 |= (0 << 1); // 使能事件生成
    TIM10->CR1 |= (1 << 2); // 只有计数上溢产生中断
    TIM10->CR1 |= (0 << 7); // 自动重载不缓冲
    TIM10->CR1 |= (0 << 8); // 一分频

    TIM10->PSC = 0;
    TIM10->CCER = 0;
    TIM10->EGR = 0;

    TIM10->DIER = 0;
    TIM10->DIER |= 1; // 允许更新中断

    TIM10->CCMR1 = 0;
    TIM10->CCMR1 |= (0 << 0); // CH1 设置为输出
    TIM10->CCMR1 |= (1 << 2); // CH1 快速比较输出使能
    TIM10->CCMR1 |= (0 << 3); // CH1 禁止预装载
    TIM10->CCMR1 |= (6 << 4); //CH1 PWM1模式

    TIM10->CCER = 0;
    TIM10->CCER |= 1 << 0; //OC1 输出使能
    TIM10->CCER |= 0 << 1; //OC1 高电平有效

    MY_NVIC_Init(1, 2, TIM1_UP_TIM10_IRQn); //��2��������ȼ�
}

void TIM9_PWM_Init(void)
{
    RCC_AHB1_GPIO_EN(GPIO_E); //ʹ��PORTFʱ��
    RCC_AHB1_GPIO_EN(GPIO_A); //ʹ��PORTFʱ��

    GPIO_Set(GPIOE, PIN5, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOA, PIN3, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOA, PIN7, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);

    GPIO_Set(GPIOE, PIN6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //���ù���,�������
    GPIO_AF_Set(GPIOE, 6, PIN_AF3);

    RCC_APB2_TIM_EN(APB2_TIM9);

    TIM9->CR1 = 0;
    TIM9->CR1 |= (0 << 8); // 一分频
    TIM9->CR1 |= (0 << 7); // 自动重载不缓冲
    TIM9->CR1 |= (0 << 3); // 发生更新时计数不停止
    TIM9->CR1 |= (1 << 2); // 只有计数上溢产生中断
    TIM9->CR1 |= (0 << 1); // 使能事件生成

    TIM9->PSC = 0;
    TIM9->CCER = 0;
    TIM9->RCR = 0;

    TIM9->DIER = 0;
    TIM9->DIER |= 1; // ���������ж�

    TIM9->CCMR1 = 0;
    TIM9->CCMR1 |= (0 << 8); // CH2 设置为输出
    TIM9->CCMR1 |= (1 << 10); // 快速比较输出使能
    TIM9->CCMR1 |= (0 << 11); // 禁止预装载
    TIM9->CCMR1 |= (6 << 12); // CH2 PWM1 模式

    TIM9->CCER |= 1 << 4; //OC2 输出使能
    TIM9->CCER |= 0 << 5; //OC2 高电平有效

    MY_NVIC_Init(1, 3, TIM1_BRK_TIM9_IRQn); //��2��������ȼ�
}

//TIM8 PWM���ֳ�ʼ��
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
// APB2 timer clock 120M, ��������60M
void TIM8_PWM_Init(void)
{
    RCC_AHB1_GPIO_EN(GPIO_C); //ʹ��PORTFʱ��

    GPIO_Set(GPIOC, PIN6, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOC, PIN7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //���ù���,�������
    GPIO_AF_Set(GPIOC, 7, PIN_AF3);

    RCC_APB2_TIM_EN(APB2_TIM8);

    TIM8->CR1 = 0;
    // TIM8->CR1 &= ~TIM_CR1_CKD;// 1��Ƶ
    // TIM8->CR1 &= ~TIM_CR1_ARPE;// ��ֹ�Զ�����
    TIM8->CR1 |= (0 << 8) | (0 << 7) | (0 << 5) | (0 << 4) | (0 << 3) | (1 << 2) | (0 << 1); // ��ֹ�Զ�����
    TIM8->PSC = 0;
    TIM8->CCER = 0;
    TIM8->RCR = 0;

    TIM8->DIER = 0;
    TIM8->DIER |= 1; // ���������ж�

    TIM8->CCMR1 = 0;
    TIM8->CCMR1 |= (6 << 12) | (1 << 11) | (1 << 10) | (0 << 8); //CH1 PWM1ģʽ

    TIM8->CCER = 0;
    TIM8->CCER |= 1 << 4; //OC1 ���ʹ��
    TIM8->CCER |= 0 << 5; //OC1 �͵�ƽ��Ч

    TIM8->BDTR = 0;
    TIM8->BDTR |= TIM_BDTR_MOE;
    MY_NVIC_Init(1, 2, TIM8_UP_TIM13_IRQn); //��2��������ȼ�
}

void DR_CW_X(void)
{
    if (SystemParam->XDir > 0)
    {
        SET_DR_CW_X();
    }
    else
    {
        SET_DR_CCW_X();
    }
}

void DR_CCW_X(void)
{
    if (SystemParam->XDir > 0)
    {
        SET_DR_CCW_X();
    }
    else
    {
        SET_DR_CW_X();
    }
}

void DR_CW_Y(void)
{
    if (SystemParam->YDir > 0)
    {
        SET_DR_CW_Y();
    }
    else
    {
        SET_DR_CCW_Y();
    }
}

void DR_CCW_Y(void)
{
    if (SystemParam->YDir > 0)
    {
        SET_DR_CCW_Y();
    }
    else
    {
        SET_DR_CW_Y();
    }
}

void DR_CW_Z(void)
{
//    if (SystemParam->ZDir > 0)
//    {
//        SET_DR_CW_Z();
//    }
//    else
//    {
//        SET_DR_CCW_Z();
//    }
}

void DR_CCW_Z(void)
{
//    if (SystemParam->ZDir > 0)
//    {
//        SET_DR_CCW_Z();
//    }
//    else
//    {
//        SET_DR_CW_Z();
//    }
}

/********************************************

MTU8-PWM????????ж????

*********************************************/
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
    if (TIM11->SR & TIM_SR_UIF)
    {
        TIM11_PWMProc();
        TIM11->SR &= ~TIM_SR_UIF;
    }
}

void TIM1_UP_TIM10_IRQHandler(void)
{
    if (TIM10->SR & TIM_SR_UIF)
    {
        TIM10_PWMProc();
        TIM10->SR &= ~TIM_SR_UIF;
    }
}

void TIM8_UP_TIM13_IRQHandler(void)
{
    if (TIM13->SR & TIM_SR_UIF)
    {
        TIM13_PWMProc();
        TIM13->SR &= ~TIM_SR_UIF;
    }
//  else if (TIM8->SR & TIM_SR_UIF)
//  {
//      TIM8_PWMProc();
//      TIM8->SR &= ~TIM_SR_UIF;
//  }
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
    if (TIM9->SR & TIM_SR_UIF)
    {
        TIM9_PWMProc();
        TIM9->SR &= ~TIM_SR_UIF;
    }
}


void ServoMotorEnable(void)
{
    if (AXIS_NUM > 0)
    {
        X_SON = 1;
    }
    if (AXIS_NUM > 1)
    {
        Y_SON = 1;
    }
    if (AXIS_NUM > 2)
    {
        Z_SON = 1;
    }
    if (AXIS_NUM > 3)
    {
        U_SON = 1;
    }
}
