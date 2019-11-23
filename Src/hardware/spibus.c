#include "spibus.h"
#include "inout.h"
/**
 * 初始化SPI硬件
 *
 * @author xuzhenglim (2017-11-14)
 *
 * @param void
 */
void hw_spi_bus_init(void)
{
    u16 tempreg = 0;

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; //使能SPI引脚时钟
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //使能复位引脚时钟

    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN; //SPI时钟使能

    RCC->APB1RSTR |= RCC_APB1RSTR_SPI3RST;  //复位SPI
    RCC->APB1RSTR &= ~RCC_APB1RSTR_SPI3RST; //停止复位SPI

    GPIO_Set(GPIOC, PIN10 | PIN11 | PIN12,
             GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //SCK MISO MOSI引脚复用

    GPIO_Set(GPIOD, PIN2,
             GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //复位引脚设置为输出方式

    GPIO_AF_Set(GPIOC, 10, PIN_AF6); // 设置引脚复用为SPI脚
    GPIO_AF_Set(GPIOC, 11, PIN_AF6); //
    GPIO_AF_Set(GPIOC, 12, PIN_AF6); //

    // SPI3->CR1 = 0;
    // SPI3->CR1 = SPI3->CR1 & (~SPI_CR1_LSBFIRST) & (~SPI_CR1_RXONLY) & (~SPI_CR1_DFF);
    // SPI3->CR1 = SPI_CR1_SSM | SPI_CR1_SPE | SPI_CR1_BR | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA;

    tempreg |= 0 << 10; //全双工模式
    tempreg |= 1 << 9;  //软件nss管理
    tempreg |= 1 << 8;
    tempreg |= 1 << 2;  //SPI主机
    tempreg |= 0 << 11; //8位数据格式
    tempreg |= 1 << 1;  //空闲模式下SCK为1 CPOL=1
    tempreg |= 1 << 0;  //数据采样从第2个时间边沿开始,CPHA=1

    //对SPI属于APB2的外设.时钟频率最大为96Mhz频率.
    tempreg |= 7 << 3;           //Fsck=Fpclk1/256
    tempreg |= 0 << 7;           //MSB First
    tempreg |= 1 << 6;           //SPI启动
    SPI3->CR1 = tempreg;         //设置CR1
    SPI3->I2SCFGR &= ~(1 << 11); //选择SPI模式
    SPI_ReadWriteByte(0xff);     //启动传输
}

//SPI速度设置函数
//SpeedSet:0~7
//SPI速度=fAPB2/2^(SpeedSet+1)
//fAPB2时钟一般为90Mhz
void SPI_SetSpeed(u8 SpeedSet)
{
    SpeedSet &= 0X07; //限制范围
    SPI3->CR1 &= 0XFFC7;
    SPI3->CR1 |= SpeedSet << 3; //设置SPI速度
    SPI3->CR1 |= 1 << 6;        //SPI设备使能
}

//SPI 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI_ReadWriteByte(u8 TxData)
{
    while ((SPI3->SR & 1 << 1) == 0)
        ;              //等待发送区空
    SPI3->DR = TxData; //发送一个byte
    while ((SPI3->SR & 1 << 0) == 0)
        ;            //等待接收完一个byte
    return SPI3->DR; //返回收到的数据
}
