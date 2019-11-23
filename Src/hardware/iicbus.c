/*
 * @Description: 
 * @Author: huacong
 * @LastEditors: huacong
 * @Date: 2019-04-17 14:32:41
 * @LastEditTime: 2019-04-18 20:17:59
 */
#include "iicbus.h"
#include "inout.h"

/* --EV5 */
#define I2C_BUSY ((uint32_t)0x00020000)
/* --EV5 */
#define I2C_MASTER_MODE_SELECT ((uint32_t)0x00030001)
/* --EV6 */
#define I2C_MASTER_TRANSMITTER_MODE_SELECTED ((uint32_t)0x00070082) /* BUSY, MSL, ADDR, TXE and TRA flags */
#define I2C_MASTER_RECEIVER_MODE_SELECTED ((uint32_t)0x00030002)    /* BUSY, MSL and ADDR flags */
/* --EV9 */
#define I2C_MASTER_MODE_ADDRESS10 ((uint32_t)0x00030008) /* BUSY, MSL and ADD10 flags */
/* Master RECEIVER mode -----------------------------*/
/* --EV7 */
#define I2C_MASTER_BYTE_RECEIVED ((uint32_t)0x00030040) /* BUSY, MSL and RXNE flags */

/* Master TRANSMITTER mode --------------------------*/
/* --EV8 */
#define I2C_MASTER_BYTE_TRANSMITTING ((uint32_t)0x00070080) /* TRA, BUSY, MSL, TXE flags */
/* --EV8_2 */
#define I2C_MASTER_BYTE_TRANSMITTED ((uint32_t)0x00070084) /* TRA, BUSY, MSL, TXE and BTF flags */

#define Addr24c04 0xa0
/**
 * @description: I2C初始化，快速模式，400KHZ  10M时钟
 * @param : undefined
 * @return: 
 */
void hw_iic_bus_init(void)
{
    RCC->AHB1ENR |= 1 << 1;                                                                   //ʹ��PORTBʱ��
    GPIO_Set(GPIOB, PIN6 | PIN7, GPIO_MODE_AF, GPIO_OTYPE_PP, GPIO_SPEED_100M, GPIO_PUPD_PU); //PF9,PF10����
    GPIO_AF_Set(GPIOB, 6, 4);                                                                 //PG10,AF12
    GPIO_AF_Set(GPIOB, 7, 4);                                                                 //PG10,AF12

    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; //1 << 21;

    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;  //1 << 21;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST; //(1 << 21);

    //����ʱ��
    I2C1->CR2 |= 10;         //8; //10M
    I2C1->CCR |= I2C_CCR_FS; // 快速模式
    //I2C1->CCR &= ~I2C_CCR_FS;  // 标准模式
    I2C1->CCR |= I2C_CCR_DUTY; // 16：9
    I2C1->CCR |= 1;            //0x28;       //
    I2C1->TRISE = 4;           //9;         //300ns上升 100ns的时钟周期
    I2C1->CR1 |= I2C_CR1_PE;   //��ֹ����
}

uint8 I2C1_CheckEvent(uint32 event)
{
    uint32 flag = 0;

    flag = I2C1->SR1;
    flag += ((I2C1->SR2) << 16);
    flag &= 0xffffffff;

    if ((flag & event) == event)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void I2C1_Stop(void)
{
    I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C1_Start(void)
{
    I2C1->CR1 |= I2C_CR1_START;

    while (!I2C1_CheckEvent(I2C_MASTER_MODE_SELECT))
        ;
}

void I2C1_NoAck(void)
{
    I2C1->CR1 &= (uint16)~I2C_CR1_ACK;
}

void I2C1_Ack(void)
{
    I2C1->CR1 |= I2C_CR1_ACK;
}

void I2C_CheckBusy(void)
{
    while (I2C1_CheckEvent(I2C_BUSY))
    {
        I2C1_Stop();
    }
}
// 写
void I2C1_Write_DeviceType(unsigned short addr)
{
    I2C1->DR = addr;

    while (!I2C1_CheckEvent(I2C_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
}
// 读指定
void I2C1_Read_DeviceType(unsigned short addr)
{
    I2C1->DR = addr;

    while (!I2C1_CheckEvent(I2C_MASTER_RECEIVER_MODE_SELECTED))
        ;
}

void I2C1_MasterSendData(unsigned char value)
{
    I2C1->DR = value;
    while (!I2C1_CheckEvent(I2C_MASTER_BYTE_TRANSMITTED))
        ;
}

uint8 I2C1_MasterReadData(void)
{
    while (!I2C1_CheckEvent(I2C_MASTER_BYTE_RECEIVED))
        ;
    return (I2C1->DR);
}

uint8 getDeviceType(unsigned short addr)
{
    uint8 devicetype = Addr24c04;

    uint8 page = 0;
    page = addr >> 8;
    page = page << 1;

    devicetype += page;
    return devicetype;
    /*
    if (addr <= 0x07ff)
    {
        if (addr > 0x06ff)
        {
            devicetype += 14;
        }
        else if (addr > 0x05ff)
        {
            devicetype += 12;
        }
        else if (addr > 0x04ff)
        {
            devicetype += 10;
        }
        else if (addr > 0x03ff)
        {
            devicetype += 8;
        }
        else if (addr > 0x02ff)
        {
            devicetype += 6;
        }
        else if (addr > 0x01ff)
        {
            devicetype += 4;
        }
        else if (addr > 0x00ff)
        {
            devicetype += 2;
        }
        else
        {
            devicetype += 0;
        }
    }

    return devicetype;
	*/
}

uint16 get_I2C_RW_Addr(unsigned short addr)
{
    uint8 rw_addr = addr;

    if (addr <= 0x07ff)
    {
        if (addr > 0x06ff)
        {
            rw_addr -= 0x700;
        }
        else if (addr > 0x05ff)
        {
            rw_addr -= 0x600;
        }
        else if (addr > 0x04ff)
        {
            rw_addr -= 0x500;
        }
        else if (addr > 0x03ff)
        {
            rw_addr -= 0x400;
        }
        else if (addr > 0x02ff)
        {
            rw_addr -= 0x300;
        }
        else if (addr > 0x01ff)
        {
            rw_addr -= 0x200;
        }
        else if (addr > 0x00ff)
        {
            rw_addr -= 0x100;
        }
        else
        {
            rw_addr -= 0;
        }
    }

    return rw_addr;
}
/**
 * @description: I2C写字节，
 * @param : addr 地址， dat 数据
 * @return: 
 */
void Write24C04_Byte(unsigned short addr, unsigned char dat)
{
    uint8 devicetype = 0;

    if (addr < _24C04_BYTE_NUMBER)
    {
        devicetype = getDeviceType(addr);
        addr = get_I2C_RW_Addr(addr);

        I2C1_Start();
        I2C1_Write_DeviceType(devicetype);
        I2C1_MasterSendData(addr);
        I2C1_MasterSendData(dat);
        I2C1_Stop();
    }
}

void Write24C04_nByte(uint8 *src, unsigned short addr, uint16 size)
{
    uint8 devicetype = 0;

    if (addr < _24C04_BYTE_NUMBER)
    {
        devicetype = getDeviceType(addr);
        addr = get_I2C_RW_Addr(addr);

        I2C_CheckBusy();

        I2C1_Start();
        I2C1_Write_DeviceType(devicetype);
        I2C1_MasterSendData(addr);

        while (size)
        {
            I2C1_MasterSendData(*src);
            src++;
            size--;
        }

        I2C1_Stop();
        I2C_CheckBusy();
    }
}

void Write24C04_n16bit(uint16 *src, unsigned short addr, uint16 size)
{
    uint8 devicetype = 0;

    addr = addr * 2;
    size = size * 2;

    if (addr < _24C04_BYTE_NUMBER)
    {
        devicetype = getDeviceType(addr);
        addr = get_I2C_RW_Addr(addr);

        I2C_CheckBusy();

        I2C1_Start();
        I2C1_Write_DeviceType(devicetype);
        I2C1_MasterSendData(addr);

        while (size)
        {
            I2C1_MasterSendData((*src));
            I2C1_MasterSendData((*src) >> 8);
            src++;
            size--;
        }

        I2C1_Stop();
    }
}

/**
 * @description: I2C写，
 * @param : addr 地址， dat 数据
 * @return: 
 */
void Write24C04_16bit(unsigned short addr, unsigned short dat)
{
    uint8 devicetype = 0;
    //uint8 tmp;

    addr = addr * 2;
    if (addr < _24C04_BYTE_NUMBER)
    {
        devicetype = getDeviceType(addr);
        addr = get_I2C_RW_Addr(addr);

        I2C_CheckBusy();

        I2C1_Start();
        I2C1_Write_DeviceType(devicetype);
        I2C1_MasterSendData(addr);
        I2C1_MasterSendData(dat);
        I2C1_MasterSendData(dat >> 8);
        I2C1_Stop();
    }
}
/**
 * @description: I2C写，
 * @param : addr 地址， dat 数据
 * @return: 
 */
void Write24C04_32bit(unsigned short addr, unsigned short dat)
{
    uint8 devicetype = 0;

    addr = addr * 2;
    if (addr < _24C04_BYTE_NUMBER)
    {
        devicetype = getDeviceType(addr);
        addr = get_I2C_RW_Addr(addr);

        I2C1_Start();
        I2C1_Write_DeviceType(devicetype);
        I2C1_MasterSendData(addr);
        I2C1_MasterSendData(dat);
        I2C1_MasterSendData(dat >> 8);
        I2C1_MasterSendData(dat >> 16);
        I2C1_MasterSendData(dat >> 24);

        I2C1_Stop();
    }
}
/**
 * @description: 读取单字节 
 * @param : addr 地址
 * @return: 返回数据
 */
unsigned char Read24C04_Byte(unsigned short addr)
{
    unsigned char dat = 0;
    uint8 devicetype = 0;

    if (addr < _24C04_BYTE_NUMBER)
    {
        devicetype = getDeviceType(addr);
        addr = get_I2C_RW_Addr(addr);

        I2C_CheckBusy();
        I2C1_Start();
        I2C1_Write_DeviceType(devicetype + 0);
        I2C1_MasterSendData(addr);

        I2C1_Start();
        I2C1_Read_DeviceType(devicetype + 1);
        I2C1_NoAck();
        I2C1_Stop();
        dat = I2C1_MasterReadData();
    }

    return dat;
}
/**
 * @description: 从指定地址开始连续读取n字节长度大于1 
 * @param : 
 * @return: 无返回 
 */
void Read24C04_nByte(uint8 *dst, uint16 size, uint16 addr)
{
    uint8 devicetype;

    if (addr < _24C04_BYTE_NUMBER)
    {
        devicetype = getDeviceType(addr);
        addr = get_I2C_RW_Addr(addr);

        I2C_CheckBusy();
        I2C1_Ack();
        I2C1_Start();
        I2C1_Write_DeviceType(devicetype + 0);
        I2C1_MasterSendData(addr);

        I2C1_Start();
        I2C1_Read_DeviceType(devicetype + 1);

        while (size > 0)
        {
            if (size == 1)
            {
                I2C1_NoAck();
                I2C1_Stop();
            }

            *dst = I2C1_MasterReadData();
            dst++;
            size--;
        }
    }
}