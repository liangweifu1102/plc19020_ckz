#ifndef __SYS_H
#define __SYS_H
#include "../stm32f4lib/stm32f407xx.h"
#include "typedefine.h"
//0,不支持OS
//1,支持OS
#define SYSTEM_SUPPORT_OS 0 //定义系统文件夹是否支持OS
///////////////////////////////////////////////////////////////////////////////////
//定义一些常用的数据类型短关键字

extern uint32 dwTickCount;
// 系统频率120M时的频率
#define AHB_CLK 120000000UL
#define SYS_TIM_CLK 120000000UL
#define APB1_PER_CLK 30000000UL
#define APB1_TIM_CLK 60000000UL
#define APB2_PER_CLK 60000000UL
#define APB2_TIM_CLK 120000000UL

#define SYS_NVIC_Group 2

#define NVIC_VectTab_FLASH_USER           ((uint32_t)0x08000000)

#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
//IO?????
#define GPIOA_ODR_Addr (GPIOA_BASE + 20) //0x40020014
#define GPIOB_ODR_Addr (GPIOB_BASE + 20) //0x40020414
#define GPIOC_ODR_Addr (GPIOC_BASE + 20) //0x40020814
#define GPIOD_ODR_Addr (GPIOD_BASE + 20) //0x40020C14
#define GPIOE_ODR_Addr (GPIOE_BASE + 20) //0x40021014
#define GPIOF_ODR_Addr (GPIOF_BASE + 20) //0x40021414
#define GPIOG_ODR_Addr (GPIOG_BASE + 20) //0x40021814
#define GPIOH_ODR_Addr (GPIOH_BASE + 20) //0x40021C14
#define GPIOI_ODR_Addr (GPIOI_BASE + 20) //0x40022014

#define GPIOA_IDR_Addr (GPIOA_BASE + 16) //0x40020010
#define GPIOB_IDR_Addr (GPIOB_BASE + 16) //0x40020410
#define GPIOC_IDR_Addr (GPIOC_BASE + 16) //0x40020810
#define GPIOD_IDR_Addr (GPIOD_BASE + 16) //0x40020C10
#define GPIOE_IDR_Addr (GPIOE_BASE + 16) //0x40021010
#define GPIOF_IDR_Addr (GPIOF_BASE + 16) //0x40021410
#define GPIOG_IDR_Addr (GPIOG_BASE + 16) //0x40021810
#define GPIOH_IDR_Addr (GPIOH_BASE + 16) //0x40021C10
#define GPIOI_IDR_Addr (GPIOI_BASE + 16) //0x40022010

//IO???,?????IO?!
//??n????16!
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n) //??
#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr, n)  //??

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n) //??
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr, n)  //??

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n) //??
#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr, n)  //??

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n) //??
#define PDin(n) BIT_ADDR(GPIOD_IDR_Addr, n)  //??

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n) //??
#define PEin(n) BIT_ADDR(GPIOE_IDR_Addr, n)  //??

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n) //??
#define PFin(n) BIT_ADDR(GPIOF_IDR_Addr, n)  //??

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n) //??
#define PGin(n) BIT_ADDR(GPIOG_IDR_Addr, n)  //??

#define PHout(n) BIT_ADDR(GPIOH_ODR_Addr, n) //??
#define PHin(n) BIT_ADDR(GPIOH_IDR_Addr, n)  //??

#define PIout(n) BIT_ADDR(GPIOI_ODR_Addr, n) //??
#define PIin(n) BIT_ADDR(GPIOI_IDR_Addr, n)  //??
//////////////////////////////////////////////////////////////////////////////////

//Ex_NVIC_Config????
#define GPIO_A 0
#define GPIO_B 1
#define GPIO_C 2
#define GPIO_D 3
#define GPIO_E 4
#define GPIO_F 5
#define GPIO_G 6
#define GPIO_H 7
#define GPIO_I 8
//x范围为GPIO_A  -- GPIO_I
#define RCC_AHB1_GPIO_EN(X) RCC->AHB1ENR |= (1 << X);

#define FTIR 1 //下降沿触发
#define RTIR 2 //上升沿触发

//GPIO设置专用宏定义
#define GPIO_MODE_IN 0  //普通输入模式
#define GPIO_MODE_OUT 1 //普通输出模式
#define GPIO_MODE_AF 2  //AF功能模式
#define GPIO_MODE_AIN 3 //模拟输入模式

#define GPIO_SPEED_2M 0   //GPIO速度2Mhz
#define GPIO_SPEED_25M 1  //GPIO速度25Mhz
#define GPIO_SPEED_50M 2  //GPIO速度50Mhz
#define GPIO_SPEED_100M 3 //GPIO速度100Mhz

#define GPIO_PUPD_NONE 0 //不带上下拉
#define GPIO_PUPD_PU 1   //上拉
#define GPIO_PUPD_PD 2   //下拉
#define GPIO_PUPD_RES 3  //保留

#define GPIO_OTYPE_PP 0 //推挽输出
#define GPIO_OTYPE_OD 1 //开漏输出

#define PIN_AF0 0x0
#define PIN_AF1 0x1
#define PIN_AF2 0x2
#define PIN_AF3 0x3
#define PIN_AF4 0x4
#define PIN_AF5 0x5
#define PIN_AF6 0x6
#define PIN_AF7 0x7
#define PIN_AF8 0x8
#define PIN_AF9 0x9
#define PIN_AF10 0xa
#define PIN_AF11 0xb
#define PIN_AF12 0xc
#define PIN_AF13 0xd
#define PIN_AF14 0xe
#define PIN_AF15 0xf

//GPIO引脚编号定义
#define PIN0 (1 << 0)
#define PIN1 (1 << 1)
#define PIN2 (1 << 2)
#define PIN3 (1 << 3)
#define PIN4 (1 << 4)
#define PIN5 (1 << 5)
#define PIN6 (1 << 6)
#define PIN7 (1 << 7)
#define PIN8 (1 << 8)
#define PIN9 (1 << 9)
#define PIN10 (1 << 10)
#define PIN11 (1 << 11)
#define PIN12 (1 << 12)
#define PIN13 (1 << 13)
#define PIN14 (1 << 14)
#define PIN15 (1 << 15)

//////////////////////////////////////////////////////////////////////////////////
extern uint8 Sys_Clock_Set(u32 plln, u32 pllm, u32 pllp, u32 pllq);                         //系统时钟设置
extern void Stm32_Clock_Init(u32 plln, u32 pllm, u32 pllp, u32 pllq);                       //时钟初始化
extern void Sys_Soft_Reset(void);                                                           //系统软复位
extern void Sys_Standby(void);                                                              //待机模式
extern void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset);                           //设置偏移地址
extern void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);                                     //设置NVIC分组
extern void MY_NVIC_Init(u8 NVIC_PreemptionPriority, u8 NVIC_SubPriority, u8 NVIC_Channel); //, u8 NVIC_Group); //设置中断

extern void Ex_NVIC_Config(u8 GPIOx, u8 BITx, u8 TRIM); //外部中断配置函数(只对GPIOA~I)

extern void GPIO_AF_Set(GPIO_TypeDef *GPIOx, u8 BITx, u8 AFx);
extern void GPIO_Set(GPIO_TypeDef *GPIOx, u32 BITx, u32 MODE, u32 OTYPE, u32 OSPEED, u32 PUPD);

extern void Stm32_Clock_DeInit(void);

//以下为汇编函数
extern void WFI_SET(void);      //执行WFI指令
extern void INTX_DISABLE(void); //关闭所有中断
extern void INTX_ENABLE(void);  //开启所有中断
extern void MSR_MSP(u32 addr);  //设置堆栈地址
#endif
