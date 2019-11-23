

#include "inout.h"
#include "../app/usermemory.h"
//#include "..\hardware\Fm24c04.h"
#include "../hardware/iicbus.h"
#include "../hardware/w25qxx.h"
#include <string.h>

#define IOGetBit(data, bit) ((data & (1 << bit)) ? 1 : 0)
#define IOSetBit(data, bit) (data = data | (1 << bit))
#define IOClrBit(data, bit) (data = data & (~(1 << bit)))

WORD_BITS cInputVal[4];
WORD_BITS cOldInputVal[4];

WORD_BITS cRealInputVal[4];

WORD_BITS cOutputVal[4];
WORD_BITS cRealOutputVal[4];

unsigned char InputFiltering[100]; //�����˲��Ĵ���
unsigned short RunTestTime;
//uint16_t inputstatus[16];

#define SECTORSIZE 4096

void SetRealOut(void);

void GPIO_init(void)
{
    uint16 pinnumber = 0;
    uint16 i = 0;

    for (i = 0; i < 4; i++)
    {
        cInputVal[i].Val = 0;
        cOldInputVal[i].Val = 0;
        cOutputVal[i].Val = 0;
        cRealInputVal[i].Val = 0;
        cRealOutputVal[i].Val = 0;
    }

    RCC_AHB1_GPIO_EN(GPIO_A);
    RCC_AHB1_GPIO_EN(GPIO_B);
    RCC_AHB1_GPIO_EN(GPIO_C);
    RCC_AHB1_GPIO_EN(GPIO_D);
    RCC_AHB1_GPIO_EN(GPIO_E);
    RCC_AHB1_GPIO_EN(GPIO_F);
    RCC_AHB1_GPIO_EN(GPIO_G);
    RCC_AHB1_GPIO_EN(GPIO_H);
    RCC_AHB1_GPIO_EN(GPIO_I);

    // INPUT
    pinnumber = PIN15;
    GPIO_Set(GPIOA, pinnumber, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PU);

    pinnumber = PIN3 | PIN4 | PIN5 | PIN8 | PIN9;
    GPIO_Set(GPIOB, pinnumber, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PU);

    pinnumber = PIN0 | PIN1 | PIN14;
    GPIO_Set(GPIOC, pinnumber, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PU);

    pinnumber = PIN3 | PIN4 | PIN5 | PIN7 | PIN8 | PIN9 | PIN10;
    GPIO_Set(GPIOD, pinnumber, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PU);

    pinnumber = PIN0 | PIN1 | PIN2 | PIN3 | PIN15;
    GPIO_Set(GPIOE, pinnumber, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PU);

    pinnumber = PIN2 | PIN1 | PIN3;
    GPIO_Set(GPIOF, pinnumber, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PU);

    pinnumber = PIN10 | PIN12 | PIN15;
    GPIO_Set(GPIOG, pinnumber, GPIO_MODE_IN, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PU);

    //  OUTPUT
    // 初始化输出使能管脚
    GPIO_Set(GPIOF, PIN4, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PD);
    OutputDisable();

    pinnumber = PIN1 | PIN13; // | PIN15;
    GPIO_Set(GPIOB, pinnumber, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PD);

//    pinnumber = PIN0; // | PIN9;
//    GPIO_Set(GPIOC, pinnumber, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PU);

    pinnumber = PIN0 | PIN1 | PIN14 | PIN15;
    GPIO_Set(GPIOD, pinnumber, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PD);

    pinnumber = PIN7 | PIN8 | PIN9 | PIN10 | PIN11 | PIN12 | PIN13 | PIN14;
    GPIO_Set(GPIOE, pinnumber, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PD);

    pinnumber = PIN11 | PIN12 | PIN13 | PIN14 | PIN15;
    GPIO_Set(GPIOF, pinnumber, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PD);

    pinnumber = PIN0 | PIN1 | PIN8;
    GPIO_Set(GPIOG, pinnumber, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_2M, GPIO_PUPD_PD);

    //SetRealOut();
}

void GetRealInput(void)
{
    XREAL00 = I000;
    XREAL01 = I001;
    XREAL02 = I002;
    XREAL03 = I003;
    XREAL04 = I004;
    XREAL05 = I005;
    XREAL06 = I006;
    XREAL07 = I007;

    XREAL10 = I010;
    XREAL11 = I011;
    XREAL12 = I012;
    XREAL13 = I013;
    XREAL14 = I014;
    XREAL15 = I015;
    XREAL16 = I016;
    XREAL17 = I017;

    XREAL20 = I020;
    XREAL21 = I021;
    XREAL22 = I022;
    XREAL23 = I023;
    XREAL24 = I024;
    XREAL25 = I025;
    XREAL26 = I026;
    XREAL27 = I027;

    XREAL30 = I030;
    XREAL31 = I031;
    XREAL32 = I032;
    XREAL33 = I033;
    XREAL34 = I034;
    XREAL35 = I035;
    XREAL36 = I036;
    XREAL37 = I037;

    XREAL40 = I040;
    XREAL41 = I041;
    XREAL42 = I042;
    XREAL43 = I043;
    XREAL44 = I044;
    XREAL45 = I045;
    XREAL46 = I046;
    XREAL47 = I047;

    XREAL50 = I050;
    XREAL51 = I051;
    XREAL52 = I052;
    XREAL53 = I053;
    XREAL54 = I054;
    XREAL55 = I055;
    XREAL56 = I056;
    XREAL57 = I057;
}

void ReadInout_Convert(void)
{
    unsigned short index = 0;
    unsigned short bit = 0;
    unsigned short j = 0;
    unsigned short i = 0;
    unsigned short k = 0;

    for (i = 0; i < 50; i++)
    {
        index = IOConfigParam->XPin[i] / 16;
        bit = IOConfigParam->XPin[i] % 16;
        j = i % 16;
        k = i / 16;

        if (IOGetBit(cRealInputVal[index].Val, bit) == 1) //I000)
        {
            InputFiltering[i]++;
            if (InputFiltering[i] >= FILTERTIME_P)
            {
                InputFiltering[i] = FILTERTIME_P;

                if (IOGetBit(cInputVal[k].Val, j))
                {
                    IOSetBit(cOldInputVal[k].Val, j);
                }
                else
                {
                    IOClrBit(cOldInputVal[k].Val, j);
                }
                IOSetBit(cInputVal[k].Val, j);
            }
        }
        else
        {
            if (InputFiltering[i])
            {
                InputFiltering[i]--;
            }
            if (InputFiltering[i] <= FILTERTIME_N)
            {
                InputFiltering[i] = 0;
                if (IOGetBit(cInputVal[k].Val, j))
                {
                    IOSetBit(cOldInputVal[k].Val, j);
                }
                else
                {
                    IOClrBit(cOldInputVal[k].Val, j);
                }
                IOClrBit(cInputVal[k].Val, j);
            }
        }
    }
}

void SetInputDisplay(void)
{
    cMidleCoil[20].Val = cInputVal[0].byte.LB;
    cMidleCoil[21].Val = cInputVal[0].byte.HB;
    cMidleCoil[22].Val = cInputVal[1].byte.LB;
    cMidleCoil[23].Val = cInputVal[1].byte.HB;

    cMidleCoil[24].Val = cInputVal[2].byte.LB;
    cMidleCoil[25].Val = cInputVal[2].byte.HB;
    cMidleCoil[26].Val = cInputVal[3].byte.LB;
    cMidleCoil[27].Val = cInputVal[3].byte.HB;

   // cMidleCoil[40].Val = cRealInputVal[0].byte.LB;
   // cMidleCoil[41].Val = cRealInputVal[0].byte.HB;
   // cMidleCoil[42].Val = cRealInputVal[1].byte.LB;
   // cMidleCoil[43].Val = cRealInputVal[1].byte.HB;

    cMidleCoil[44].Val = cRealInputVal[2].byte.LB;
    cMidleCoil[45].Val = cRealInputVal[2].byte.HB;
    cMidleCoil[46].Val = cRealInputVal[3].byte.LB;
    cMidleCoil[47].Val = cRealInputVal[3].byte.HB;
}

void ReadInput(void)
{
    GetRealInput();
    ReadInout_Convert();
    SetInputDisplay();
}
void TestOutput(void)
{
    static unsigned long dwOutTime = 0;
    static unsigned short temp = 0;

    if (dwTickCount > dwOutTime)
    {
        dwOutTime = dwTickCount + 500;
        if (temp++ > 45) temp = 0;
    }
    if ((cInputX00_X17 == 0) && (cInputX20_X37 == 0) && (cInputX40_X57 == 0))
    {
        switch (temp)
        {
        case 0:
            cOutputValY00_Y17 = 0;
            cOutputValY20_Y37 = 0;
            cOutputValY40_Y57 = 0;
            break;
        case 1:
            Y00 = 1;
            break;
        case 2:
            Y01 = 1;
            break;
        case 3:
            Y02 = 1;
            break;
        case 4:
            Y03 = 1;
            break;
        case 5:
            Y04 = 1;
            break;
        case 6:
            Y05 = 1;
            break;
        case 7:
            Y06 = 1;
            break;
        case 8:
            Y07 = 1;
            break;
        case 9:
            Y10 = 1;
            break;
        case 10:
            Y11 = 1;
            break;
        case 11:
            Y12 = 1;
            break;
        case 12:
            Y13 = 1;
            break;
        case 13:
            Y14 = 1;
            break;
        case 14:
            Y15 = 1;
            break;
        case 15:
            Y16 = 1;
            break;
        case 16:
            Y17 = 1;
            break;
        case 17:
            Y20 = 1;
            break;
        case 18:
            Y21 = 1;
            break;
        case 19:
            Y22 = 1;
            break;
        case 20:
            Y23 = 1;
            break;
        case 21:
            Y00 = 1;
            break;
        case 22:
            Y01 = 1;
            break;
        case 23:
            Y02 = 1;
            break;
        case 24:
            Y03 = 1;
            break;
        case 25:
            Y04 = 1;
            break;
        case 26:
            Y05 = 1;
            break;
//      case 27:
//          Y32 = 1;
//          break;
//      case 28:
//          Y33 = 1;
//          break;
//      case 29:
//          Y34 = 1;
//          break;
//      case 30:
//          Y35 = 1;
//          break;
//      case 31:
//          Y36 = 1;
//          break;
//      case 32:
//          Y37 = 1;
            break;
        default:
            break;
        }
    }
    else
    {
        cOutputValY00_Y17 = cInputX00_X17;
        cOutputValY20_Y37 = cInputX20_X37;
        cOutputValY40_Y57 = cInputX40_X57;
        dwOutTime = dwTickCount + 500;
        temp = 0;
    }
}

void WriteOutput_Convert(void)
{
    unsigned short index = 0;
    unsigned short bit = 0;
    unsigned short j = 0;
    unsigned short i = 0;
    unsigned short k = 0;

    for (i = 0; i < 50; i++)
    {
        index = IOConfigParam->YPin[i] / 16;
        bit = IOConfigParam->YPin[i] % 16;
        j = i % 16;
        k = i / 16;

        if (IOGetBit(cOutputVal[k].Val, j) == 1) //I000)
        {
            IOSetBit(cRealOutputVal[index].Val, bit);
        }
        else
        {
            IOClrBit(cRealOutputVal[index].Val, bit);
        }
    }
}

void SetOutputDisplay(void)
{
    cMidleCoil[30].Val = cOutputVal[0].byte.LB;
    cMidleCoil[31].Val = cOutputVal[0].byte.HB;

    cMidleCoil[32].Val = cOutputVal[1].byte.LB;
    cMidleCoil[33].Val = cOutputVal[1].byte.HB;

    cMidleCoil[34].Val = cOutputVal[2].byte.LB;
    cMidleCoil[35].Val = cOutputVal[2].byte.HB;

    cMidleCoil[36].Val = cOutputVal[3].byte.LB;
    cMidleCoil[37].Val = cOutputVal[3].byte.HB;

    cMidleCoil[50].Val = cRealOutputVal[0].byte.LB;
    cMidleCoil[51].Val = cRealOutputVal[0].byte.HB;

    cMidleCoil[52].Val = cRealOutputVal[1].byte.LB;
    cMidleCoil[53].Val = cRealOutputVal[1].byte.HB;

    cMidleCoil[54].Val = cRealOutputVal[2].byte.LB;
    cMidleCoil[55].Val = cRealOutputVal[2].byte.HB;

    cMidleCoil[56].Val = cRealOutputVal[3].byte.LB;
    cMidleCoil[57].Val = cRealOutputVal[3].byte.HB;
}

void SetRealOut(void)
{
    PEout(10) = ~YREAL00;
    PEout(9) = ~YREAL01;
    PEout(8) = ~YREAL02;
    PEout(7) = ~YREAL03;
    PDout(1) = ~YREAL04;
    PDout(0) = ~YREAL05;
    PDout(15) = ~YREAL06;
    PDout(14) = ~YREAL07;

    PBout(13) = ~YREAL10;
    PGout(8) = ~YREAL11;
    PGout(1) = ~YREAL12;
    PGout(0) = ~YREAL13;
    PFout(15) = ~YREAL14;
    PFout(14) = ~YREAL15;
    PFout(13) = ~YREAL16;
    PFout(12) = ~YREAL17;

    PFout(11) = ~YREAL20;
    PBout(1) = ~YREAL21;
    PEout(14) = ~YREAL22;
    PEout(13) = ~YREAL23;
    PEout(12) = ~YREAL24;
    PEout(11) = ~YREAL25;

    PCout(3) = ~Z_SON;
    PAout(6) = ~U_SON;
    PBout(0) = ~X_SON;
    PAout(7) = ~Y_SON;
}

void WriteOutput(void)
{
    WriteOutput_Convert();
    SetRealOut();
    SetOutputDisplay();
}

unsigned char Test24c04(void)
{
    unsigned short i, Flag = 1;
    for (i = 0; i < 0x200; i++)
    {
        //Write24C04(i, 0x55);
        Write24C04_Byte(i, 0x55);
    }
    for (i = 0; i < 5000; i++)
    ;
    for (i = 0; i < 0x200; i++)
    {
        // if (Read24C04(i) != 0x55)
        // {
        //     Flag = 0;
        // }
        if (Read24C04_Byte(i) != 0x55)
        {
            Flag = 0;
        }
    }
    for (i = 0; i < 5000; i++)
    ;
    for (i = 0; i < 0x200; i++)
    {
        //Write24C04(i, 0xaa);
        Write24C04_Byte(i, 0xaa);
    }
    for (i = 0; i < 5000; i++)
    ;
    for (i = 0; i < 0x200; i++)
    {
        // if (Read24C04(i) != 0xaa)
        // {
        //     Flag = 0;
        // }
        if (Read24C04_Byte(i) != 0xaa)
        {
            Flag = 0;
        }
    }
    if (Flag)
    {
        for (i = 0; i < 0x200; i++)
        {
            //Write24C04(i, 0);
            Write24C04_Byte(i, 0);
        }
    }
    return (Flag);
}

unsigned char TestMCX314(void)
{
    unsigned short Flag = 1;

    return (Flag);
}

unsigned char TestFlash(void)
{
    unsigned short i, j, Flag = 1;
    memset(buffertmp, 0x55, SECTORSIZE / 2);

    W25QXX_Erase_Sector(0);

    W25QXX_Write(buffertmp, 0, 2048);

    for (i = 0; i < 5000; i++)
    ;

    W25QXX_Read(buffertmp, 0, 2048);

    for (j = 0; j < SECTORSIZE / 2; j++)
    {
        if (buffertmp[j] != 0x55)
        {
            Flag = 0;
            break;
        }
    }
    memset(buffertmp, 0xaa, SECTORSIZE / 2);

    W25QXX_Erase_Sector(0);

    //	printf("write data0:%d\n",buffertmp[0]);
    W25QXX_Write(buffertmp, 0, 2048);

    for (i = 0; i < 5000; i++)
    ;

    W25QXX_Read(buffertmp, 0, 2048);

    for (j = 0; j < SECTORSIZE / 2; j++)
    {
        if (buffertmp[j] != 0xaa)
        {
            Flag = 0;
            //			  printf("error j:%d\n",j);
            //			  printf("error data:%d\n",buffertmp[j]);
            break;
        }
    }
    return (Flag);
}

void TestHandWheel(void)
{
    //ReadSeroDrvState();
    //HandWheelProc(10, 1);
    /*SystemParam->XDistance=MV_Get_Command_Pos(X_AXIS);
    SystemParam->YDistance=MV_Get_Command_Pos(Y_AXIS);
    SystemParam->ZDistance=MV_Get_Command_Pos(Z_AXIS);
    SystemParam->UDistance=MV_Get_Command_Pos(U_AXIS);*/
}
