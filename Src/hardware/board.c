#include <stdio.h>

#include "../mylib/sys.h"
#include "../mylib/delay.h"
#include "board.h"

//#include "iicbus.h"
#include "spibus.h"
#include "serialbus.h"
#include "anglogbus.h"

#include "../modbus/ModbusSlave.h"


u16 cpld[16];
u32 tick = 0;
u32 shift = 0;

//void TestCpLd()
//{
//   vu16 *addr;
//   addr = (vu16 *)CPLD_ADDR;
//   do
//   {
//      cpld[0] = addr[0];
//      cpld[1] = addr[2];
//      cpld[2] = addr[4];
//      cpld[3] = addr[6];
//      cpld[4] = addr[8];
//      cpld[5] = addr[10];
//      cpld[6] = addr[12];
//      cpld[7] = addr[14];
//      cpld[8] = addr[16];
//      if (tick < dwTickCount)
//      {
//         tick = dwTickCount + 40;
//         if (shift == 0)
//         {
//            shift = 1;
//         }
//         else
//         {
//            shift <<= 1;
//         }
//         addr[0] = ~shift;
//         addr[2] = (~shift) >> 16;
//      }
//   } while (1);
//}

void stm32_hw_cpld_init()
{
    GPIO_Set(GPIOD, PIN3 | PIN7 | PIN4 | PIN5, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //PD3 FSMC_CLK
    GPIO_Set(GPIOG, PIN12 | PIN10, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU);             //PG12 FSMC_NE4

    GPIO_AF_Set(GPIOD, 3, 12);  //PD3,AF12
    GPIO_AF_Set(GPIOD, 7, 12);  //PD7,AF12
    GPIO_AF_Set(GPIOD, 4, 12);  //PD7,AF12
    GPIO_AF_Set(GPIOD, 5, 12);  //PD7,AF12
    GPIO_AF_Set(GPIOG, 10, 10); //PG10,AF12
    GPIO_AF_Set(GPIOG, 12, 12); //PG12,AF12

    //����CPLD�ǽ���FSMC_NE4�ϵģ���������Bank1-4,��Ӧ��BTCR��6��7
    //   FMC_Bank1->BTCR[6] = 0X00000000;
    //   FMC_Bank1->BTCR[7] = 0X00000000;
    //   FMC_Bank1E->BWTR[6] = 0x00000000;
    /*
   //����BCR�Ĵ���	ʹ���첽ģʽ,ģʽA(��д����һ��ʱ��Ĵ���)
   //BTCR[ż��]:BCR�Ĵ���;BTCR[����]:BTR�Ĵ���
   FMC_Bank1->BTCR[6] |= 1 << 12; //�洢��дʹ��
   FMC_Bank1->BTCR[6] |= 1 << 4; //�洢�����ݿ��Ϊ16bit
                                 //����BTR�Ĵ���			��HCLK=168M, 1��HCLK=6ns
   FMC_Bank1->BTCR[7] |= 8 << 8; //���ݱ���ʱ�䣨DATAST��Ϊ9��HCLK 6*9=54ns
   FMC_Bank1->BTCR[7] |= 0 << 4; //��ַ����ʱ�䣨ADDHLD��δ�õ�
   FMC_Bank1->BTCR[7] |= 8 << 0; //��ַ����ʱ�䣨ADDSET��Ϊ0��HCLK 0ns

   FMC_Bank1E->BWTR[6] = 0x0fffffff;
   FMC_Bank1->BTCR[6] = 1 << 0;
//         */
    ////   FMC_Bank1->BTCR[6] = 0x00005910;
    //   FMC_Bank1->BTCR[7] = 0x0FFF30F1;
    ////   FMC_Bank1E->BWTR[6] = 0x0FF303F1;
    //   FMC_Bank1->BTCR[6] |= 1 << 0;

    //   //����CPLD�ǽ���FSMC_NE4�ϵģ���������Bank1-4,��Ӧ��BTCR��6��7
    //   FMC_Bank1->BTCR[0] = 0X00000000;
    //   FMC_Bank1->BTCR[1] = 0X00000000;
    //   FMC_Bank1E->BWTR[0] = 0x00000000;

    //   //����BCR�Ĵ���	ʹ���첽ģʽ,ģʽA(��д����һ��ʱ��Ĵ���)
    //   //BTCR[ż��]:BCR�Ĵ���;BTCR[����]:BTR�Ĵ���
    //   FMC_Bank1->BTCR[6] |= 1 << 12; //�洢��дʹ��
    //   FMC_Bank1->BTCR[6] |= 1 << 4; //�洢�����ݿ��Ϊ16bit
    //                                 //����BTR�Ĵ���			��HCLK=168M, 1��HCLK=6ns
    //   FMC_Bank1->BTCR[7] |= 8 << 8; //���ݱ���ʱ�䣨DATAST��Ϊ9��HCLK 6*9=54ns
    //   FMC_Bank1->BTCR[7] |= 0 << 4; //��ַ����ʱ�䣨ADDHLD��δ�õ�
    //   FMC_Bank1->BTCR[7] |= 8 << 0; //��ַ����ʱ�䣨ADDSET��Ϊ0��HCLK 0ns

    //   FMC_Bank1E->BWTR[6] = 0x0fffffff;
    //   FMC_Bank1->BTCR[6] = 1 << 0;

    //   FMC_Bank1->BTCR[0] = 0x00005910;
    //   FMC_Bank1->BTCR[1] = 0x0FF30F1;
    //   FMC_Bank1E->BWTR[0] = 0x0FF303F1;
    //   FMC_Bank1->BTCR[0] |= 1 << 0;
}

