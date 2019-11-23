#ifndef PWM_H_
#define PWM_H_

#include "../mylib/typedefine.h"
#include "../hardware/inout.h"
#include "../app/Action.h"
#include "../mylib/sys.h"

#define ENCODETIME 1

#define APB1_TIM2 (1 << 0)
#define APB1_TIM3 (1 << 1)
#define APB1_TIM4 (1 << 2)
#define APB1_TIM5 (1 << 3)
#define APB1_TIM6 (1 << 4)
#define APB1_TIM7 (1 << 5)
#define APB1_TIM12 (1 << 6)
#define APB1_TIM13 (1 << 7)
#define APB1_TIM14 (1 << 8)

#define RCC_APB1_TIM_EN(X) RCC->APB1ENR |= X;

#define APB2_TIM1 (1 << 0)
#define APB2_TIM8 (1 << 1)
#define APB2_TIM9 (1 << 16)
#define APB2_TIM10 (1 << 17)
#define APB2_TIM11 (1 << 18)

#define RCC_APB2_TIM_EN(X) RCC->APB2ENR |= X;

//#define AxisNum 3
#define AXIS_NUM 4

#define Z_AXIS 0x2
#define U_AXIS 0x3
#define X_AXIS 0x0
#define Y_AXIS 0x1

#define XCW 0  // Զ��ԭ��
#define XCCW 1 // ����ԭ��
#define YCW 0  // Զ��ԭ��
#define YCCW 1 // ����ԭ��
#define ZCW 0  // Զ��ԭ��
#define ZCCW 1 // ����ԭ��
#define UCW 0  // Զ��ԭ��
#define UCCW 1 // ����ԭ��

#define Z_ARR TIM11->ARR
#define Z_CCR TIM11->CCR2
#define Z_CNT TIM10->CNT

#define U_ARR TIM10->ARR
#define U_CCR TIM10->CCR2
#define U_CNT TIM10->CNT

#define X_ARR TIM13->ARR
#define X_CCR TIM13->CCR1
#define X_CNT TIM13->CNT

#define Y_ARR TIM9->ARR
#define Y_CCR TIM9->CCR2
#define Y_CNT TIM9->CNT

#define Z_AXIS_PULSE_ON() TIM11->CR1 |= 0x01UL
#define Z_AXIS_PULSE_OFF() TIM11->CR1 &= ~0x01UL

#define U_AXIS_PULSE_ON() TIM10->CR1 |= 0x01UL
#define U_AXIS_PULSE_OFF() TIM10->CR1 &= ~0x01UL

#define X_AXIS_PULSE_ON() TIM13->CR1 |= 0x01UL
#define X_AXIS_PULSE_OFF() TIM13->CR1 &= ~0x01UL

#define Y_AXIS_PULSE_ON() TIM9->CR1 |= 0x01UL
#define Y_AXIS_PULSE_OFF() TIM9->CR1 &= ~0x01UL

#define SET_DR_CW_Z() GPIOF->ODR &= (uint16)(~PIN5)
#define SET_DR_CCW_Z() GPIOF->ODR |= (uint16)(PIN5)

#define SET_DR_CW_U() GPIOC->ODR &= (uint16)(~PIN13)
#define SET_DR_CCW_U() GPIOC->ODR |= (uint16)(PIN13)

#define SET_DR_CW_X() GPIOF->ODR &= (uint16)(~PIN9)
#define SET_DR_CCW_X() GPIOF->ODR |= (uint16)(PIN9)

#define SET_DR_CW_Y() GPIOE->ODR &= (uint16)(~PIN5)
#define SET_DR_CCW_Y() GPIOE->ODR |= (uint16)(PIN5)

#define Z_PWM_STATE (TIM11->CR1 & 0x01UL)
#define U_PWM_STATE (TIM10->CR1 & 0x01UL)
#define X_PWM_STATE (TIM13->CR1 & 0x01UL)
#define Y_PWM_STATE (TIM9->CR1 & 0x01UL)

#define bZDirction ((GPIOF->ODR & (uint32)(PIN5)))
#define bUDirction ((GPIOC->ODR & (uint32)(PIN13)))
#define bXDirction ((GPIOF->ODR & (uint32)(PIN9)))
#define bYDirction ((GPIOE->ODR & (uint32)(PIN5)))

#define TIM13_PWMProc()  XAxis_PWMProc() 
#define TIM11_PWMProc()  XAxis_PWMProc() 
#define TIM10_PWMProc()  YAxis_PWMProc() 
#define TIM9_PWMProc()  YAxis_PWMProc() 
#define TIM8_PWMProc()  XAxis_PWMProc()

#define ZAxis_PWM_Init()  TIM11_PWM_Init()  
#define UAxis_PWM_Init()  TIM10_PWM_Init()  
#define XAxis_PWM_Init() TIM13_PWM_Init()   
#define YAxis_PWM_Init() TIM9_PWM_Init()   
//#define TIM8_PWM_Init()  XAxis_PWM_Init()

extern void DR_CW_X(void);
extern void DR_CCW_X(void);
extern void DR_CW_Y(void);
extern void DR_CCW_Y(void);
extern void DR_CW_Z(void);
extern void DR_CCW_Z(void);
extern void DR_CW_U(void);
extern void DR_CCW_U(void);

extern void TIM13_PWM_Init(void);
extern void TIM11_PWM_Init(void);
extern void TIM10_PWM_Init(void);
extern void TIM9_PWM_Init(void);

#endif
