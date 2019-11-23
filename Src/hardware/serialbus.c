/*
 * @Author: clive
 * @Date: 2018-11-30 10:46:18
 * @Last Modified by: clive
 * @Last Modified time: 2018-12-04 14:52:47
 */
#include <stdio.h>
#include <stdarg.h>
#include "stm32f407xx.h"
#include "stm32f4xx_hal.h"

#include "inout.h"
#include "serialbus.h"

static int uart1RequireLength = 0;
static int uart2RequireLength = 0;
static int uart6RequireLength = 0;

static uint32 uart1DMARevLen = 0;
static uint32 uart2DMARevLen = 0;
static uint32 uart6DMARevLen = 0;

char serial1ReceiveOk = 0;

#define USE_UART1 (1)
#define USE_UART2 (0)
#define USE_UART3 (0)
#define USE_UART6 (1)

DMA_TypeDef *Uart1Tx_DMA;
DMA_Stream_TypeDef *Uart1Tx_Stream;
uint32 Uart1Tx_Channel;
DMA_TypeDef *Uart1Rx_DMA;
DMA_Stream_TypeDef *Uart1Rx_Stream;
uint32 Uart1Rx_Channel;

DMA_TypeDef *Uart2_DMA;
DMA_Stream_TypeDef *Uart2_DMA_Stream;
DMA_TypeDef *Uart3_DMA;
DMA_Stream_TypeDef *Uart3_DMA_Stream;
DMA_TypeDef *Uart4_DMA;
DMA_Stream_TypeDef *Uart4_DMA_Stream;

DMA_TypeDef *Uart6Tx_DMA;
DMA_Stream_TypeDef *Uart6Tx_Stream;
uint32 Uart6Tx_Channel;
DMA_TypeDef *Uart6Rx_DMA;
DMA_Stream_TypeDef *Uart6Rx_Stream;
uint32 Uart6Rx_Channel;
/**
 * @brief
 * @note
 * @param  apbtype: 0 apb1 clk   1,apb2 clk
 * @param  speed: 波特率
 * @retval
 */
static uint16 UartSpeed(uint16 apbtype, uint32 speed)
{
    double temp;
    uint16 mantissa;
    uint16 fraction;

    uint32 busfreq = (apbtype > 0) ? APB2_PER_CLK : APB1_PER_CLK;

    temp = (double)(busfreq) / (speed * 16);     //得到USARTDIV@OVER8=0
    mantissa = (uint16)temp;                     //得到整数部分
    fraction = (uint16)((temp - mantissa) * 16); //得到小数部分@OVER8=0
    mantissa <<= 4;
    mantissa += fraction;

    return mantissa;
}

/**
 *
 *
 * @author xuzhenglim (2017-11-15)
 *
 * @param speed
 */
void hw_serial1_init(int speed)
{
    uint16 mantissa = UartSpeed(1, speed);

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; //使能PORTA口时钟
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; //使能串口1时钟
    RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    GPIO_Set(GPIOA, PIN9 | PIN10, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //PA9,PA10,复用功能,上拉输出
    GPIO_Set(GPIOB, PIN12, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);

    GPIO_AF_Set(GPIOA, 9, PIN_AF7);  //PA9,AF8
    GPIO_AF_Set(GPIOA, 10, PIN_AF7); //PA10,AF8

    Uart1Tx_DMA = DMA2;
    Uart1Tx_Stream = DMA2_Stream7;
    Uart1Tx_Channel = DMA_CHANNEL_4;

    Uart1Rx_DMA = DMA2;
    Uart1Rx_Stream = DMA2_Stream5;
    Uart1Rx_Channel = DMA_CHANNEL_4;

    USART1->BRR = mantissa; //波特率设置

    USART1->CR1 = 0;
    USART1->CR2 = 0;
    USART1->CR3 = 0;

    USART1->CR1 &= ~(USART_CR1_OVER8);             //设置OVER8=0
    USART1->CR1 |= USART_CR1_TE;                   //串口发送使能
    USART1->CR1 |= USART_CR1_RE;                   //串口接收使能
    USART1->CR1 &= ~(USART_CR1_M | USART_CR1_PCE); //1位起始无校验
    USART1->CR2 &= ~USART_CR2_STOP;
    USART1->CR3 |= USART_CR3_DMAR; //1 << 6; DMA接收
    USART1->CR3 |= USART_CR3_DMAT; //1 << 7; DMA发送

    USART1->CR1 |= USART_CR1_UE; //串口使能
}
/**
 * @description: 串口2初始化DMA方式 
 * @param : undefined
 * @return: 
 */
void hw_serial2_init(int speed)
{
    uint16 mantissa = UartSpeed(0, speed);

    RCC->AHB1ENR |= 1 << 3;  //使能PORT口时钟
    RCC->APB1ENR |= 1 << 17; //使能串口2时钟

    GPIO_Set(GPIOD, PIN5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_50M, GPIO_PUPD_PU); //
    GPIO_Set(GPIOD, PIN6, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_50M, GPIO_PUPD_PU); //

    GPIO_AF_Set(GPIOD, 5, PIN_AF7); //
    GPIO_AF_Set(GPIOD, 6, PIN_AF7); //

    //波特率设置
    USART2->BRR = mantissa; //波特率设置

    USART2->CR1 = 0;
    USART2->CR2 = 0;

    USART2->CR1 &= ~USART_CR1_OVER8;
    USART2->CR1 |= (USART_CR1_TE);                 //接收发送使能
    USART2->CR1 |= (USART_CR1_RE);                 //接收发送使能                //接收发送使能
    USART2->CR1 &= ~(USART_CR1_M | USART_CR1_PCE); //1位起始无校验
                                                   //USART2->CR1 |= USART_CR1_TCIE;//1 << 6;                         //发送完中断
    USART2->CR2 &= ~USART_CR2_STOP;                //1位停止位

    //使能接收中断
    //USART2->CR1 |= USART_CR1_RXNEIE; //接收缓冲区非空中断使能
    //MY_NVIC_Init(3, 2, USART2_IRQn); //组2，最低优先级

    USART2->CR3 |= USART_CR3_DMAR; //1 << 6; DMA接收
    USART2->CR3 |= USART_CR3_DMAT; //1 << 7; DMA发送

    USART2->CR1 |= USART_CR1_UE; //串口使能

    //Serial2TransmitEnable();
}

/**
 * 
 * 
 * @author zhang (2019/9/2)
 * 
 * @param speed 波特率
 */
void hw_serial3_init(int speed)
{
    uint16 mantissa = UartSpeed(0, speed);

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;  //使能PORTG口时钟
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN; //使能串口3时钟
    RCC->APB1RSTR |= RCC_APB1RSTR_USART3RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_USART3RST;

    //GPIO_Set(GPIOB, PIN10|PIN11, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);

    GPIO_Set(GPIOB, PIN10 | PIN11, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //PA9,PA10,复用功能,上拉输出
    GPIO_AF_Set(GPIOB, 10, PIN_AF7);                                                           //PA9,AF8
    GPIO_AF_Set(GPIOB, 11, PIN_AF7);                                                           //PA10,AF8

    //波特率设置
    USART3->BRR = mantissa;    //波特率设置
                               //
    USART3->CR1 = 0;
    USART3->CR2 = 0;
    USART3->CR3 = 0;

    USART3->CR1 &= ~USART_CR1_OVER8;

    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;    //串口发送接收使能
    USART3->CR1 &= ~(USART_CR1_M | USART_CR1_PCE); // 8个数据位，不使用就校验
    USART3->CR2 &= ~USART_CR2_STOP;                // 一个停止位

    USART3->CR3 |= USART_CR3_DMAR; //1 << 6; DMA接收
    USART3->CR3 |= USART_CR3_DMAT; //1 << 7; DMA发送

    USART3->CR1 |= USART_CR1_UE; //串口使能
}

void hw_serial4_init(int speed)
{
    uint16 mantissa = UartSpeed(0, speed);

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  //使能PORTG口时钟
    RCC->APB1ENR |= RCC_APB1ENR_UART4EN; //使能串口3时钟
    RCC->APB1RSTR |= RCC_APB1RSTR_UART4RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_UART4RST;

    //GPIO_Set(GPIOB, PIN10|PIN11, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);

    GPIO_Set(GPIOA, PIN0 | PIN1, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //PA9,PA10,复用功能,上拉输出
    GPIO_AF_Set(GPIOA, 0, PIN_AF8);                                                           //PA9,AF8
    GPIO_AF_Set(GPIOA, 1, PIN_AF8);                                                           //PA10,AF8

    //波特率设置
    UART4->BRR = mantissa;    //波特率设置
                              //
    UART4->CR1 = 0;
    UART4->CR2 = 0;
    UART4->CR3 = 0;

    UART4->CR1 &= ~USART_CR1_OVER8;

    UART4->CR1 |= USART_CR1_TE | USART_CR1_RE;    //串口发送接收使能
    UART4->CR1 &= ~(USART_CR1_M | USART_CR1_PCE); // 8个数据位，不使用就校验
    UART4->CR2 &= ~USART_CR2_STOP;                // 一个停止位

    UART4->CR3 |= USART_CR3_DMAR; //1 << 6; DMA接收
    UART4->CR3 |= USART_CR3_DMAT; //1 << 7; DMA发送

    UART4->CR1 |= USART_CR1_UE; //串口使能
}

static void hw_serial6_init(int speed)
{
    uint16 mantissa = UartSpeed(1, speed);

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOGEN; //使能PORTA口时钟
    RCC->APB2ENR |= RCC_APB2ENR_USART6EN; //使能串口1时钟
    RCC->APB2RSTR |= RCC_APB2RSTR_USART6RST;
    RCC->APB2RSTR &= ~RCC_APB2RSTR_USART6RST;

    GPIO_Set(GPIOG, PIN13, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);
    GPIO_Set(GPIOG, PIN9 | PIN14, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //PA9,PA10,复用功能,上拉输出
    GPIO_AF_Set(GPIOG, 9, PIN_AF8);                                                            //PA9,AF8
    GPIO_AF_Set(GPIOG, 14, PIN_AF8);                                                           //PA10,AF8

    Uart6Tx_DMA = DMA2;
    Uart6Tx_Stream = DMA2_Stream6;
    Uart6Tx_Channel = DMA_CHANNEL_5;

    Uart6Rx_DMA = DMA2;
    Uart6Rx_Stream = DMA2_Stream1;
    Uart6Rx_Channel = DMA_CHANNEL_5;

    //波特率设置
    USART6->BRR = mantissa; //波特率设置

    USART6->CR1 = 0;
    USART6->CR2 = 0;
    USART6->CR3 = 0;

    USART6->CR1 &= ~USART_CR1_OVER8;

    USART6->CR1 |= USART_CR1_TE | USART_CR1_RE;    //串口发送接收使能
    USART6->CR1 &= ~(USART_CR1_M | USART_CR1_PCE); // 8个数据位，不使用就校验
    USART6->CR2 &= ~USART_CR2_STOP;                // 一个停止位

    USART6->CR3 |= USART_CR3_DMAR; //1 << 6; DMA接收
    USART6->CR3 |= USART_CR3_DMAT; //1 << 7; DMA发送

    USART6->CR1 |= USART_CR1_UE; //串口使能

    //Uart6RxEnable(); //开始接收
}

/**
 * 串口初始化
 * 
 * @author xt (2019-04-02)
 * 
 * @param void 
 */
void hw_serial_bus_init(void)
{
#if USE_UART1
    hw_serial1_init(115200);
#endif
#if USE_UART2
    hw_serial2_init(9600);
#endif
#if USE_UART3
    hw_serial3_init(9600);
#endif
#if USE_UART4
    hw_serial4_init(9600);
#endif
#if USE_UART6
    hw_serial6_init(9600);
#endif
    //hw_serial1_test();
}

/**
 * //清空之前该stream上的所有中断标志
 * 
 * @author xt (2019-04-02)
 * 
 * @param DMAx DMA1 DMA2
 * @param DMA_Streamx DMA_Stream0--DMA_Stream7
 */
void ClearDMAInterruputFlag(DMA_TypeDef *DMAx, DMA_Stream_TypeDef *DMA_Streamx)
{
    uint32 streamx;

    if ((uint32)DMAx == (uint32)DMA2)
    {                            //DMA2
        RCC->AHB1ENR |= 1 << 22; //DMA2时钟使能
    }
    else
    {                            //DMA1
        RCC->AHB1ENR |= 1 << 21; //DMA1时钟使能
    }

    //得到stream通道号
    streamx = (u32)DMA_Streamx - (u32)DMAx;
    streamx -= 0x10;
    streamx /= 0X18;

    if (streamx > 3) //>= 4)
    {
        if (streamx > 5) //>= 6)
        {
            streamx -= 6;
            streamx *= 6;
            streamx += 16;
        }
        else
        {
            streamx -= 4;
            streamx *= 6;
        }

        DMAx->HIFCR |= 0X3D << streamx;
    }
    else
    {
        if (streamx > 1) //>= 2)
        {
            streamx -= 2;
            streamx *= 6;
            streamx += 16;
        }
        else
        {
            streamx *= 6;
        }
        //清空之前该stream上的所有中断标志
        DMAx->LIFCR |= 0X3D << streamx;
    }
}
/**
 * 串口1 DMA 发送
 * 
 * @author zhang (2019/9/2)
 * 
 * @param src 
 * @param length 
 * 
 * @return int 
 */
int Uart1DMASend(void *src, int length)
{
    DMA_TypeDef *DMAx = Uart1Tx_DMA;
    DMA_Stream_TypeDef *DMA_Streamx = Uart1Tx_Stream;

    DMA_Streamx->CR = 0; //先全部复位CR寄存器值
    ClearDMAInterruputFlag(DMAx, DMA_Streamx);

    DMA_Streamx->PAR = (uint32)&USART1->DR; //DMA外设地址
    DMA_Streamx->M0AR = (uint32)src;        //DMA 存储器0地址
    DMA_Streamx->NDTR = length;             //DMA 存储器0地址

    DMA_Streamx->CR |= DMA_MEMORY_TO_PERIPH; //存储器到外设模式
    DMA_Streamx->CR |= DMA_NORMAL;           //非循环模式(即使用普通模式)
    DMA_Streamx->CR |= DMA_PINC_DISABLE;     //外设非增量模式
    DMA_Streamx->CR |= DMA_MINC_ENABLE;      //存储器增量模式
    DMA_Streamx->CR |= DMA_PDATAALIGN_BYTE;  //外设数据长度:8位
    DMA_Streamx->CR |= DMA_MDATAALIGN_BYTE;  //存储器数据长度:8位
    DMA_Streamx->CR |= DMA_PRIORITY_MEDIUM;  //中等优先级
    DMA_Streamx->CR |= DMA_PBURST_SINGLE;    //外设突发单次传输
    DMA_Streamx->CR |= DMA_MBURST_SINGLE;    //存储器突发单次传输

    DMA_Streamx->CR |= Uart1Tx_Channel;
    DMA_Streamx->CR |= DMA_SxCR_EN; // 1 << 0; //开启DMA传输

    return 0;
}

int Uart1DMAReceive(void *dst, int length)
{
    DMA_TypeDef *DMAx = Uart1Rx_DMA;
    DMA_Stream_TypeDef *DMA_Streamx = Uart1Rx_Stream;
    //清除IDLE;
    uint32 sr;

    DMA_Streamx->CR = 0; //先全部复位CR寄存器值
    ClearDMAInterruputFlag(DMAx, DMA_Streamx);
    sr = USART1->SR;
    sr = USART1->DR;
    (void)sr;

    DMA_Streamx->PAR = (uint32)&USART1->DR; //DMA外设地址
    DMA_Streamx->M0AR = (uint32)dst;        //DMA 存储器0地址
    DMA_Streamx->NDTR = length;             //DMA 存储器0地址

    DMA_Streamx->CR |= DMA_PERIPH_TO_MEMORY; //外设到存储器
    DMA_Streamx->CR |= DMA_NORMAL;           //非循环模式(即使用普通模式)
    DMA_Streamx->CR |= DMA_PINC_DISABLE;     //外设非增量模式
    DMA_Streamx->CR |= DMA_MINC_ENABLE;      //存储器增量模式
    DMA_Streamx->CR |= DMA_PDATAALIGN_BYTE;  //外设数据长度:8位
    DMA_Streamx->CR |= DMA_MDATAALIGN_BYTE;  //存储器数据长度:8位
    DMA_Streamx->CR |= DMA_PRIORITY_MEDIUM;  //中等优先级
    DMA_Streamx->CR |= DMA_PBURST_SINGLE;    //外设突发单次传输
    DMA_Streamx->CR |= DMA_MBURST_SINGLE;    //存储器突发单次传输

    DMA_Streamx->CR |= Uart1Rx_Channel;
    DMA_Streamx->CR |= DMA_SxCR_EN; // 1 << 0; //开启DMA传输

    uart1RequireLength = length;
    uart1DMARevLen = length;

    return 0;
}

int Uart1CheckSendEnd(void)
{
    if ((USART1->SR & UART_FLAG_TC)) //1 << 6))
    {
        return 1;
    }
    return 0;
}

int Uart1CheckRecvEnd(void)
{
    static uint32 delay = 0;
    uint32 res = Uart1Rx_Stream->NDTR;

    if (uart1RequireLength == 0)
    {
        return 0xffff;
    }
    if (uart1RequireLength == res)
    {
        return 0;
    }

    if (res == 0)
    {
        res = uart1RequireLength - res;
        uart1RequireLength = 0;
        return res;
    }
    else if (res != uart1DMARevLen)
    {
        uart1DMARevLen = res;
        delay = dwTickCount + 3;
    }
    else if (res == uart1DMARevLen)
    {
        if (delay < dwTickCount)
        {
            res = uart1RequireLength - res;
            uart1RequireLength = 0;
            Uart1Rx_Stream->CR &= ~DMA_SxCR_EN;
            Uart1Rx_Stream->NDTR = 0;
            return res;
        }
    }

    return 0;
}

/**
 * 串口2DMA发送
 * 使用DMA 1，数据流6 通道4
 * @author xt (2019-04-02)
 * 
 * @param src 
 * @param length 
 * 
 * @return int 
 */
int Uart2DMASend(void *src, int length)
{
    DMA_TypeDef *DMAx = DMA1;
    DMA_Stream_TypeDef *DMA_Streamx = DMA1_Stream6;

    DMA_Streamx->CR = 0; //先全部复位CR寄存器值
    ClearDMAInterruputFlag(DMAx, DMA_Streamx);

    DMA_Streamx->PAR = (uint32)&USART2->DR; //DMA外设地址
    DMA_Streamx->M0AR = (uint32)src;        //DMA 存储器0地址
    DMA_Streamx->NDTR = length;             //DMA 存储器0地址

    DMA_Streamx->CR |= DMA_MEMORY_TO_PERIPH; //存储器到外设模式
    DMA_Streamx->CR |= DMA_NORMAL;           //非循环模式(即使用普通模式)
    DMA_Streamx->CR |= DMA_PINC_DISABLE;     //外设非增量模式
    DMA_Streamx->CR |= DMA_MINC_ENABLE;      //存储器增量模式
    DMA_Streamx->CR |= DMA_PDATAALIGN_BYTE;  //外设数据长度:8位
    DMA_Streamx->CR |= DMA_MDATAALIGN_BYTE;  //存储器数据长度:8位
    DMA_Streamx->CR |= DMA_PRIORITY_MEDIUM;  //中等优先级
    DMA_Streamx->CR |= DMA_PBURST_SINGLE;    //外设突发单次传输
    DMA_Streamx->CR |= DMA_MBURST_SINGLE;    //存储器突发单次传输

    DMA_Streamx->CR |= DMA_CHANNEL_4;
    DMA_Streamx->CR |= DMA_SxCR_EN; // 1 << 0; //开启DMA传输

    return 0;
}

/**
 * 串口2 DMA接收
 * 使用DMA 1，数据流5 通道4
 * @author xt (2019-04-03)
 * 
 * @param dst 
 * @param length 
 * 
 * @return int 
 */
int Uart2DMAReceive(void *dst, int length)
{
    DMA_TypeDef *DMAx = DMA1;
    DMA_Stream_TypeDef *DMA_Streamx = DMA1_Stream5;
    //清除IDLE;
    u32 sr;

    DMA_Streamx->CR = 0; //先全部复位CR寄存器值
    ClearDMAInterruputFlag(DMAx, DMA_Streamx);
    sr = USART6->SR;
    sr = USART6->DR;
    (void)sr;
    DMA_Streamx->PAR = (u32)&USART6->DR; //DMA外设地址
    DMA_Streamx->M0AR = (u32)dst;        //DMA 存储器0地址
    DMA_Streamx->NDTR = length;          //DMA 存储器0地址

    DMA_Streamx->CR |= DMA_PERIPH_TO_MEMORY; //外设到存储器
    DMA_Streamx->CR |= DMA_NORMAL;           //非循环模式(即使用普通模式)
    DMA_Streamx->CR |= DMA_PINC_DISABLE;     //外设非增量模式
    DMA_Streamx->CR |= DMA_MINC_ENABLE;      //存储器增量模式
    DMA_Streamx->CR |= DMA_PDATAALIGN_BYTE;  //外设数据长度:8位
    DMA_Streamx->CR |= DMA_MDATAALIGN_BYTE;  //存储器数据长度:8位
    DMA_Streamx->CR |= DMA_PRIORITY_MEDIUM;  //中等优先级
    DMA_Streamx->CR |= DMA_PBURST_SINGLE;    //外设突发单次传输
    DMA_Streamx->CR |= DMA_MBURST_SINGLE;    //存储器突发单次传输

    DMA_Streamx->CR |= DMA_CHANNEL_4;
    DMA_Streamx->CR |= DMA_SxCR_EN; // 1 << 0; //开启DMA传输

    uart2RequireLength = length;
    uart2DMARevLen = length;

    return 0;
}

/**
 * 串口2 发送完成
 * 
 * @author xt (2019-04-03)
 * 
 * @param void 
 * 
 * @return int 
 */
int Uart2CheckSendEnd(void)
{
    if ((USART2->SR & USART_FLAG_TC)) //1 << 6))
    {
        return 1;
    }
    return 0;
}

/**
 * 串口2 接收完成
 * 
 * @author xt (2019-04-03)
 * 
 * @return int 
 */
int Uart2CheckRecvEnd(void)
{
    static uint32 delay = 0;
    uint32 res = DMA1_Stream5->NDTR;

    if (uart2RequireLength == 0)
    {
        return 0xffff;
    }
    if (uart2RequireLength == res)
    {
        return 0;
    }

    if (res == 0)
    {
        res = uart2RequireLength - res;
        uart2RequireLength = 0;
        return res;
    }
    else if (res != uart2DMARevLen)
    {
        uart2DMARevLen = res;
        delay = dwTickCount + 3;
    }
    else if (res == uart2DMARevLen)
    {
        if (delay < dwTickCount)
        {
            res = uart2RequireLength - res;
            uart2RequireLength = 0;
            DMA1_Stream5->CR &= ~DMA_SxCR_EN;
            DMA1_Stream5->NDTR = 0;
            return res;
        }
    }

    return 0;
}

/**
 * 串口6DMA发送
 * 使用DMA 2，数据流7 通道5
 * @author xt (2019-04-02)
 * 
 * @param src 
 * @param length 
 * 
 * @return int 
 */
int Uart6DMASend(void *src, int length)
{
    DMA_TypeDef *DMAx = Uart6Tx_DMA;
    DMA_Stream_TypeDef *DMA_Streamx = Uart6Tx_Stream;

    DMA_Streamx->CR = 0; //先全部复位CR寄存器值
    ClearDMAInterruputFlag(DMAx, DMA_Streamx);

    DMA_Streamx->PAR = (u32)&USART6->DR; //DMA外设地址
    DMA_Streamx->M0AR = (u32)src;        //DMA 存储器0地址
    DMA_Streamx->NDTR = length;          //DMA 存储器0地址

    DMA_Streamx->CR |= DMA_MEMORY_TO_PERIPH; //存储器到外设模式
    DMA_Streamx->CR |= DMA_NORMAL;           //非循环模式(即使用普通模式)
    DMA_Streamx->CR |= DMA_PINC_DISABLE;     //外设非增量模式
    DMA_Streamx->CR |= DMA_MINC_ENABLE;      //存储器增量模式
    DMA_Streamx->CR |= DMA_PDATAALIGN_BYTE;  //外设数据长度:8位
    DMA_Streamx->CR |= DMA_MDATAALIGN_BYTE;  //存储器数据长度:8位
    DMA_Streamx->CR |= DMA_PRIORITY_MEDIUM;  //中等优先级
    DMA_Streamx->CR |= DMA_PBURST_SINGLE;    //外设突发单次传输
    DMA_Streamx->CR |= DMA_MBURST_SINGLE;    //存储器突发单次传输

    DMA_Streamx->CR |= Uart6Tx_Channel;
    DMA_Streamx->CR |= DMA_SxCR_EN; // 1 << 0; //开启DMA传输

    return 0;
}

/**
 * 串口6 DMA接收
 * 使用DMA 2，数据流1 通道5
 * @author xt (2019-04-03)
 * 
 * @param dst 
 * @param length 
 * 
 * @return int 
 */
int Uart6DMAReceive(void *dst, int length)
{
    DMA_TypeDef *DMAx = Uart6Rx_DMA;
    DMA_Stream_TypeDef *DMA_Streamx = Uart6Rx_Stream;
    //清除IDLE;
    u32 sr;

    DMA_Streamx->CR = 0; //先全部复位CR寄存器值
    ClearDMAInterruputFlag(DMAx, DMA_Streamx);
    sr = USART6->SR;
    sr = USART6->DR;
    (void)sr;
    DMA_Streamx->PAR = (u32)&USART6->DR; //DMA外设地址
    DMA_Streamx->M0AR = (u32)dst;        //DMA 存储器0地址
    DMA_Streamx->NDTR = length;          //DMA 存储器0地址

    DMA_Streamx->CR |= DMA_PERIPH_TO_MEMORY; //外设到存储器
    DMA_Streamx->CR |= DMA_NORMAL;           //非循环模式(即使用普通模式)
    DMA_Streamx->CR |= DMA_PINC_DISABLE;     //外设非增量模式
    DMA_Streamx->CR |= DMA_MINC_ENABLE;      //存储器增量模式
    DMA_Streamx->CR |= DMA_PDATAALIGN_BYTE;  //外设数据长度:8位
    DMA_Streamx->CR |= DMA_MDATAALIGN_BYTE;  //存储器数据长度:8位
    DMA_Streamx->CR |= DMA_PRIORITY_MEDIUM;  //中等优先级
    DMA_Streamx->CR |= DMA_PBURST_SINGLE;    //外设突发单次传输
    DMA_Streamx->CR |= DMA_MBURST_SINGLE;    //存储器突发单次传输

    DMA_Streamx->CR |= Uart6Rx_Channel;
    DMA_Streamx->CR |= DMA_SxCR_EN; // 1 << 0; //开启DMA传输

    uart6RequireLength = length;
    uart6DMARevLen = length;

    return 0;
}

/**
 * 串口6 发送完成
 * 
 * @author xt (2019-04-03)
 * 
 * @param void 
 * 
 * @return int 
 */
int Uart6CheckSendEnd(void)
{
    if ((USART6->SR & USART_FLAG_TC)) //1 << 6))
    {
        return 1;
    }
    return 0;
}

/**
 * 串口6 接收完成
 * 
 * @author xt (2019-04-03)
 * 
 * @return int 
 */
int Uart6CheckRecvEnd(void)
{
    static uint32 delay = 0;
    uint32 res = Uart6Rx_Stream->NDTR;

    if (uart6RequireLength == 0)
    {
        return 0xffff;
    }
    if (uart6RequireLength == res)
    {
        return 0;
    }

    if (res == 0)
    {
        res = uart6RequireLength - res;
        uart6RequireLength = 0;
        return res;
    }
    else if (res != uart6DMARevLen)
    {
        uart6DMARevLen = res;
        delay = dwTickCount + 3;
    }
    else if (res == uart6DMARevLen)
    {
        if (delay < dwTickCount)
        {
            res = uart6RequireLength - res;
            uart6RequireLength = 0;
            Uart6Rx_Stream->CR &= ~DMA_SxCR_EN;
            Uart6Rx_Stream->NDTR = 0;
            return res;
        }
    }

    return 0;
}

#if 0
//#pragma import(__use_no_semihosting)
// 标准库需要支持的函数
// struct __FILE
// {
//     int handle;
//     /* Whatever you require here. If the only file you are using is */
//     /* standard output using printf() for debugging, no file handling */
//     /* is required. */
// };
// /* FILE is typedef?? d in stdio.h. */
// FILE __stdout;
// //定义_sys_exit 以避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
// 重定义fput
int fputc(int ch, FILE *f)
{
    while ((USART2->SR & 0X40) == 0)
    ; //????1?7?1?7?1?7?1?7???,??1?7?1?7???1?7?1?7?????1?7?1?7?1?7?1?7?
    USART2->DR = (u8)ch;
    return ch;
}
#else
static char printbuff[512];
int printf(const char *format, ...)
{
    int rv;
    va_list arg; 

    if (PRINTF_DEBUG > 0)
    {
        while (Uart1CheckSendEnd() == 0)
        ;
        va_start(arg, format);
        rv = vsprintf(printbuff, format, arg);
        va_end(arg);

        Uart1DMASend(printbuff, rv);
    }

    return rv;
}

#endif
//static uint8 uart2_data = 0;
/**
 * @description: 
 * @param {type} 
 * @return: 
 */
void USART2_IRQHandler(void)
{
    if (USART2->SR & USART_SR_RXNE) //(1<<5))//接收到数据
    {
        //uart2_data = USART2->DR;

        // switch (uart2_data)
        // {
        // case RX_STATE_END1:
        //     ScanCodeBuffer.RxState = RX_END1;
        //     ScanCodeBuffer.RxOk = 1;
        //     ScanCodeBuffer.RxState = 0;
        //     break;
        // case RX_STATE_END2:
        //     if (ScanCodeBuffer.RxState == RX_END1)
        //     {
        //         ScanCodeBuffer.RxOk = 1;
        //         ScanCodeBuffer.RxState = 0;
        //     }
        //     break;
        // default:
        //     if (ScanCodeBuffer.RxState == 0)
        //     {
        //         ScanCodeBuffer.RxBuf[ScanCodeBuffer.RxPos & 0x3f] = uart2_data;
        //         ScanCodeBuffer.RxPos++;
        //     }

        //     break;
        // }
    }
    // if (USART2->SR & USART_SR_TC)
    // {
    //     USART2->SR &= ~USART_SR_TC;
    // }
}
