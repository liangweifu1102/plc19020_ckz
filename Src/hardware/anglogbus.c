#include "anglogbus.h"
#include "board.h"
#include "../mylib/sys.h"

/**
 * 模拟硬件初始化
 *
 * @author xuzhenglim (2017-11-15)
 *
 * @param void
 */
void hw_anglog_bus_init(void)
{
    RCC->AHB1ENR |= 1 << 0;  //GPIOA RCC 使能
    RCC->APB1ENR |= 1 << 29; //使能DAC时钟
    RCC->APB1RSTR |= 1 << 29;
    RCC->APB1RSTR &= ~(1 << 29);

    GPIO_Set(GPIOA, PIN4 | PIN5, GPIO_MODE_AIN, 0, 0, GPIO_PUPD_PU); //PA4,模拟输入,下拉

    DAC->CR |= 1 << 1 | 1 << 17; //DAC1输出缓存不使能 BOFF1=1
    DAC->CR |= 1 << 0 | 1 << 16; //使能DAC1
    DAC->CR |= 0 << 2;           //不使用触发功能 TEN1=0
    DAC->CR |= 0 << 3;           //DAC TIM6 TRGO,不过要TEN1=1才行
    DAC->CR |= 0 << 6;           //不使用波形发生
    DAC->CR |= 0 << 8;           //屏蔽、幅值设置
    DAC->CR |= 0 << 12;          //DAC1 DMA不使能

    DAC->DHR12R1 = 0;
    DAC->DHR12R2 = 0;
}

/**
 * 12位的ADC输出
 *
 * @author xuzhenglim (2017-11-15)
 *
 * @param ch
 * @param value
 */
void DAC_Conver(int ch, int value)
{
    if (value > 0xfff)
        value = 0xfff;
    if (ch == 0)
    {
        DAC->DHR12R1 = value;
        //DAC->SWTRIGR |= 1 << 0;
    }
    else if (ch == 1)
    {
        DAC->DHR12R2 = value; //value & 0x0fffUL;
        //DAC->SWTRIGR |= 1 << 1;
    }
}

/**
 * 测试DAC
 *
 * @author xuzhenglim (2017-11-16)
 */
//static void hw_dac_test()
//{
//   delay_ms(1000);
//   {
//      int curValue1 = 0;
//      int curValue2 = 0;
//      int dir1 = 0;
//      int dir2 = 0;
//      int temptick = dwTickCount;
//      do
//      {
//
//         if (temptick < dwTickCount)
//         {
//            temptick = dwTickCount + 2;
//
//            if (dir1 == 0)
//            {
//               if (curValue1 <= 8)
//               {
//                  curValue1 = 0;
//                  dir1 = 1;
//               }
//               else
//               {
//                  curValue1 -= 8;
//               }
//            }
//            else
//            {
//               if (curValue1 + 8 >= 0xfff)
//               {
//                  curValue1 = 0xff0;
//                  dir1 = 0;
//               }
//               else
//               {
//                  curValue1 += 8;
//               }
//
//            }
//
//            if (dir2 == 0)
//            {
//               if (curValue2 <= 32)
//               {
//                  curValue2 = 0;
//                  dir2 = 1;
//               }
//               else
//               {
//                  curValue2 -= 32;
//               }
//            }
//            else
//            {
//               if (curValue2 + 32 >= 0xff0)
//               {
//                  curValue2 = 0xff0;
//                  dir2 = 0;
//               }
//               else
//               {
//                  curValue2 += 32;
//               }
//
//            }
//            //curValue1 += 8;
//            //curValue2 += 32;
//
//            DAC_Conver(1, curValue1);
//            DAC_Conver(2, curValue2);
//            //DAC->DHR12RD = curValue1 | curValue2 << 16;
//            //DAC->DHR12R1 = curValue1 & 0xfff;
//            //DAC->DHR12R2 = curValue2 & 0xfff;
//
//         }
//      }
//      while (1);
//   }
//
//}
