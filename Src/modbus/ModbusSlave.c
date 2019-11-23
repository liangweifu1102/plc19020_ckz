/*
 * @Author: ccc 
 * @Date: 2019-07-25 00:31:53 
 * @Last Modified by:   ccc 
 * @Last Modified time: 2019-07-25 00:31:53 
 */

#include "ModbusSlave.h"
#include "../app/usermemory.h"
#include "../hardware/serialbus.h"

#include <string.h>
#include <stdio.h>

static const unsigned char BinToAscTab[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                            'A', 'B', 'C', 'D', 'E', 'F'};

static unsigned char CRCTblHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};

static unsigned char CRCTblLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40

};
static uint8 RtuFlag = 0;
/**
 * 计算累加和校验
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param dat 
 * @param length 
 * 
 * @return unsigned char 
 */
static unsigned char CalcLrc(unsigned char *dat, int length)
{
    unsigned char lrc = 0;

    while (length > 0)
    {
        length--;
        lrc += *dat;
        dat++;
    }
    return (~lrc) + 1;
}

/**
 * 计算累加和校验
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param dat 
 * @param length 
 * 
 * @return unsigned char 
 */
static unsigned short CalcCrc(unsigned char *dat, int length)
{
    unsigned char lo = 0xff, hi = 0xff;
    unsigned char index = 0;

    while (length > 0)
    {
        length--;
        index = hi ^ (*dat);
        hi = lo ^ CRCTblHi[index];
        lo = CRCTblLo[index];
        dat++;
    }
    return 0x0100 * lo + hi;
}

/**
 * Ascii 编码
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param dst 编码后数据指针
 * @param src 源数据指针
 * @param length 源数据指针
 */
static int ModbusEncodeAscii(unsigned char *dst, unsigned char *src, int length)
{
    int index = length * 2 + 1;

    //计算累加和
    unsigned char lrc = CalcLrc(src, length);

    //从后面编码可以保证前面不会被覆盖
    while (length > 0)
    {
        length--;
        dst[(length << 1) + 1] = BinToAscTab[(src[length] >> 4) & 0x0f];
        dst[(length << 1) + 2] = BinToAscTab[src[length] & 0x0f];
    }

    //首字节肯定是':'
    dst[0] = ':';

    /**
     * 加上结尾
     */
    dst[index++] = BinToAscTab[(lrc >> 4) & 0x0f];
    dst[index++] = BinToAscTab[lrc & 0x0f];
    dst[index++] = 0x0D;
    dst[index++] = 0x0A;

    return index;
}

/**
 * Ascii 解码
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param dst 解码后数据指针
 * @param src 源数据指针
 * @param length 源数据长度
 */
static int ModbusDecodeAscii(unsigned char *dst, unsigned char *src, int length)
{
    int index = 0;
    int res = 0;
    unsigned char dat = 0;

    //不符合长度与格式要求不能进行解码
    if (length < 17)
    {
        return -1;
    }
    if (*src != ':' || src[length - 1] != 0x0A || src[length - 2] != 0x0D)
    {
        return -2;
    }

    //跳掉 ':'
    src++;

    //Ascii 解码成hex
    while (length > 1)
    {
        dat = 0;
        if (*src >= 'A' && *src <= 'F')
        {
            dat = *src - 'A' + 10;
        }
        else if (*src >= '0' && *src <= '9')
        {
            dat = *src - '0';
        }

        src++;
        dat <<= 4;

        if (*src >= 'A' && *src <= 'F')
        {
            dat |= *src - 'A' + 10;
        }
        else if (*src >= '0' && *src <= '9')
        {
            dat |= *src - '0';
        }

        src++;
        length -= 2;

        dst[index] = dat;
        index++;
    }

    //校验通过
    if (CalcCrc(dst, index) != 0)
    {
        res = index;
    }

    return res;
}

/**
 * RTU 编码
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param dst 
 * @param src 
 * @param length 
 */
static int ModbusEncodeRTU(unsigned char *dst, unsigned char *src, int length)
{
    unsigned short crc = CalcCrc(src, length);

    //需要时就复制源数据
    if (dst != src)
    {
        int index = 0;
        while (index < length)
        {
            dst[index] = src[index];
            index++;
        }
    }

    dst[length++] = crc;
    dst[length++] = crc >> 8;
    return length;
}

/**
 * RTU 解码
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param dst 
 * @param src 
 * @param length 
 *               
 */
static int ModbusDecodeRTU(unsigned char *dst, unsigned char *src, int length)
{

    if (length < 5)
    {
        return -1;
    }

    if (CalcCrc(src, length) == 0)
    {
        //需要时就复制源数据
        if (dst != src)
        {
            int index = 0;
            while (index < length)
            {
                dst[index] = src[index];
                index++;
            }
        }
        return length - 2;
    }

    return 0;
}

/**
 * 处理MiddleCoil通讯
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param ack 
 * @param req 
 * 
 * @return int 
 *         返回长度，当length为负时，说明有错误
 */
static int ModbusReadCoil(unsigned char *ack, unsigned char *req)
{
    unsigned short address;
    unsigned short number;
    char shift = 0;
    int len = 0;

    address = (req[2] << 8) + req[3];

    if (RtuFlag)
    {
        address += 1;
    }

    number = (req[4] << 8) + req[5];

    //限制地址
    address &= 0x03FF;
    //
    //    //限制地址
    //    if (address + number >= 400)
    //    {
    //        return -2;
    //    }
    //    //限制单交最多的长度
    //    else if (number > 0x07D0 || number == 0)
    //    {
    //        return -3;
    //    }

    ack[2] = (number + 7) >> 3;
    shift = address & 0x07;
    address >>= 3;

    len = 3;

    if (shift == 0)
    {
        while ((len - 3) < ack[2])
        {
            ack[len++] = cMidleCoil[address].Val;
            address++;
            ;
        }
    }
    else
    {
        unsigned char dat = 0;
        while ((len - 3) < ack[2])
        {
            dat = cMidleCoil[address].Val >> shift;
            address++;
            dat |= cMidleCoil[address].Val << (8 - shift);
            ack[len++] = dat;
        }
    }

    return len;
}

/**
 *  处理多个寄存器
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param ack 
 * @param req 
 * 
 * @return int 
 *         返回有效命令的长度，如果是负值，说明有错误
 */
static int ModbusReadRegister(unsigned char *ack, unsigned char *req)
{
    unsigned short address;
    unsigned short number;
    int len = 0;

    address = ((req[2]) << 8) + req[3];
    if (RtuFlag)
    {
        address += 1;
    }
    number = (req[4] << 8) + req[5];

    //限制地址
    address &= 0x0FFF;
    //    //限制地址
    //    if (address + number >= MEMSIZE)
    //    {
    //        return -2;
    //    }
    //    //限制单交最多的长度
    //    else if (number > 0x07D || number == 0)
    //    {
    //        return -3;
    //    }

    //拷贝缓冲数据
    ack[2] = number * 2;
    len = 3;
    while (number > 0)
    {
        ack[len++] = cDataArea[address] >> 8;
        ack[len++] = cDataArea[address];
        address++;
        number--;
    }

    return len;
}

/**
 * 写多个寄存器的值
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param ack 
 * @param req 
 * 
 * @return int 
 *         返回有效命令的长度，如果是负值，说明有错误
 */
static int ModbusWriteCoilSingle(unsigned char *ack, unsigned char *req)
{
    unsigned short address;
    unsigned short value;
    int len = 0;

    address = (req[2] << 8) + req[3];
    if (RtuFlag)
    {
        address += 1;
    }
    value = (req[4] << 8) + req[5];
    //
    //    //限制地址
    address &= 0x03FF;
    //
    //    //限制地址
    //    if (address  >= 400)
    //    {
    //        return -2;
    //    }
    //    //限制单交最多的长度

    if (value != 0x0000)
    {
        cMidleCoil[address >> 3].Val |= 1 << (address & 0x07);
    }
    else
    {
        cMidleCoil[address >> 3].Val &= ~(1 << (address & 0x07));
    }

    ack[2] = req[2];
    ack[3] = req[3];
    ack[4] = req[4];
    ack[5] = req[5];
    len = 6;

    return len;
}

/**
 * 写单个寄存器的值
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param ack 
 * @param req 
 * 
 * @return int 
 *         返回有效命令的长度，如果是负值，说明有错误
 */
static int ModbusWriteRegisterSingle(unsigned char *ack, unsigned char *req)
{
    unsigned short address;
    unsigned short value;
    int len = 0;

    address = (req[2] << 8) + req[3];
    if (RtuFlag)
    {
        address += 1;
    }
    value = (req[4] << 8) + req[5];

    //    //限制地址
    address &= 0x0FFF;

    cDataArea[address] = value;

    Save16BitDate(value, address); //
    //Write24C04_16bit(address, value);

    ack[2] = req[2];
    ack[3] = req[3];
    ack[4] = req[4];
    ack[5] = req[5];
    len = 6;

    return len;
}

/**
 * 写多个线圈值
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param ack 
 * @param req 
 * 
 * @return int 
 *         返回有效命令的长度，如果是负值，说明有错误
 */
static int ModbusWriteCoilMultiple(unsigned char *ack, unsigned char *req)
{
    unsigned short address;
    unsigned short number;
    int index = 7;
    char shift = 0;
    int i = 0;
    int len = 0;

    address = (req[2] << 8) + req[3];
    if (RtuFlag)
    {
        address += 1;
    }
    number = (req[4] << 8) + req[5];

    //    //限制地址
    address &= 0x03FF;

    shift = address & 0x07;
    address >>= 3;

    if (shift == 0)
    {
        //满足字节对齐的部分优先处理
        while (number > 7)
        {
            cMidleCoil[address].Val = req[index];
            index++;
            address++;
            number -= 8;
        }

        //最后不足整个字节的部分需要特殊处理
        if (number != 0)
        {
            cMidleCoil[address].Val &= 0xFF << (number);
            req[index] &= 0xff >> (8 - number);

            cMidleCoil[address].Val |= req[index];
        }
    }
    else
    {
        //整个字节的独立处理
        while (number > 7)
        {
            cMidleCoil[address].Val &= 0xff >> (8 - shift);
            cMidleCoil[address].Val |= req[index] << shift;
            address++;
            cMidleCoil[address].Val &= 0xff << shift;
            cMidleCoil[address].Val |= req[index] >> (8 - shift);
            index++;
            number -= 8;
        }

        //最后一个字节不满字节，按位处理
        while (i < number)
        {
            if (req[index] & (1 << i))
            {
                cMidleCoil[address].Val |= 1 << shift;
            }
            else
            {
                cMidleCoil[address].Val &= ~(1 << shift);
            }
            i++;
            shift++;
            if (shift > 8)
            {
                shift = 0;
                address++;
            }
        }
    }
    ack[2] = req[2];
    ack[3] = req[3];
    ack[4] = req[4];
    ack[5] = req[5];
    len = 6;

    return len;
}

/**
 * 写多个寄存器
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param ack 
 * @param req 
 * 
 * @return int 
 *         返回有效命令的长度，如果是负值，说明有错误
 */
static int ModbusWriteRegisterMultiple(unsigned char *ack, unsigned char *req)
{
    unsigned short address;
    unsigned short number;

    int index = 7;
    int len = 0;

    address = (req[2] << 8) + req[3];
    if (RtuFlag)
    {
        address += 1;
    }
    number = (req[4] << 8) + req[5];

    address &= 0x0FFF; //限制地址

    while (number > 0)
    {
        cDataArea[address] = (req[index++] << 8);
        cDataArea[address] += req[index++];
        Save16BitDate(cDataArea[address], address);
        //index += 2;
        address++;
        number--;
    }
    //Write24C04_n16bit(&cDataArea[j], j, i);

    ack[2] = req[2];
    ack[3] = req[3];
    ack[4] = req[4];
    ack[5] = req[5];
    len = 6;

    return len;
}
/**
  * @brief  测试IAP串口通信和
  * @note   
  * @param  *ack: 
  * @param  *req: 
  * @retval 
  */
static int modbus_slave_test_com(unsigned char *ack, unsigned char *req)
{
    ack[2] = req[2];
    ack[3] = req[3];
    ack[4] = 'X';
    ack[5] = 'T';
    ack[6] = 'A';
    ack[7] = 'P';
    ack[8] = 'P';

    return 9;
}

static int modbus_slave_exit_app(unsigned char *ack, unsigned char *req)
{
    *((uint32_t *)0x2000000C) = 0xA5A5A5A5;

    //Sys_Soft_Reset();
    NVIC_SystemReset();
    ack[2] = req[2];
    ack[3] = req[3];
    //ack[4] = req[4];
    //ack[5] = req[5];
    return 4;
}

/**
 * Modbus协议 
 * 属于自己定制的 
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param dst 
 * @param src 
 * 
 * @return int 
 */
static int ModbusStandard(unsigned char *ack, unsigned char *req)
{
    int len = 0;

    switch (req[1])
    {
    case 0x01:
        len = ModbusReadCoil(ack, req);
        break;
    case 0x04:
    case 0x03:
        len = ModbusReadRegister(ack, req);
        break;
    case 0x05:
        len = ModbusWriteCoilSingle(ack, req);
        break;
    case 0x06:
        len = ModbusWriteRegisterSingle(ack, req);
        break;
    case 0x0F:
        len = ModbusWriteCoilMultiple(ack, req);
        break;
    case 0x10:
        len = ModbusWriteRegisterMultiple(ack, req);
        break;
    case 0x23:
        len = modbus_slave_exit_app(ack, req);
        break;
    case 0x25:
        len = modbus_slave_test_com(ack, req);
        break;
    default:
        len = -1;
        break;
    }

    //拷贝ID
    ack[0] = req[0];
    ack[1] = req[1];

    //错误时返回错误帧
    if (len < 0)
    {
        ack[1] |= 0x80;
        ack[2] = -len;
        len = 3;
    }
    else
    {
        ack[1] = req[1];
    }

    return len;
}

/**
 * Delta协议，盗版，只识别有限的几个地址域
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param ack 
 * @param req 
 * 
 * @return int 
 */
int ModbusDelta(unsigned char *ack, unsigned char *req)
{

    int len = 0;

    switch (req[1])
    {
    case 0x01:
    case 0x02:
        len = ModbusReadCoil(ack, req);
        break;
    case 0x03:
        len = ModbusReadRegister(ack, req);
        break;
    case 0x05:
        len = ModbusWriteCoilSingle(ack, req);
        break;
    case 0x06:
        len = ModbusWriteRegisterSingle(ack, req);
        break;
    case 0x0F:
        len = ModbusWriteCoilMultiple(ack, req);
        break;
    case 0x10:
        len = ModbusWriteRegisterMultiple(ack, req);
        break;
    default:
        len = -1;
        break;
    }

    //拷贝ID
    ack[0] = req[0];
    ack[1] = req[1];

    //错误时返回错误帧
    if (len < 0)
    {
        ack[1] |= 0x80;
        ack[2] = -len;
        len = 3;
    }
    else
    {
        ack[1] = req[1];
    }

    return len;
}

static int step6 = 1;
static unsigned char recvBuf6[1024];
static unsigned char sendBuf6[1024];

void Uart6_Modbus_CheckData(void)
{
    int len = 0;

    switch (step6)
    {
    case 0:
        step6 = 1;
        break;
    case 1:
        //采用DMA接收包
        Uart6RxEnable();
        Uart6DMAReceive(recvBuf6, sizeof(recvBuf6));
        step6++;
        break;
    case 2:
        //判断是否有包输入
        len = Uart6CheckRecvEnd();
        if (len > 0)
        {
            if (recvBuf6[0] == ':' && recvBuf6[1] == '0' &&
                recvBuf6[len - 2] == 0x0D &&
                recvBuf6[len - 1] == 0x0A)
            { // DVP_ASCII
                if (ModbusDecodeAscii(recvBuf6, recvBuf6, len) > 0)
                {
                    RtuFlag = 0;
                    len = ModbusDelta(sendBuf6, recvBuf6);
                    if (len > 0)
                    {
                        len = ModbusEncodeAscii(sendBuf6, sendBuf6, len);
                    }
                }
            }
            else
            { //RTU
                if (ModbusDecodeRTU(recvBuf6, recvBuf6, len) > 0)
                {
                    RtuFlag = 1;
                    len = ModbusStandard(sendBuf6, recvBuf6);
                    if (len > 0)
                    {
                        len = ModbusEncodeRTU(sendBuf6, sendBuf6, len);
                    }
                }
            }

            if (len > 0)
            {
                //发送数据
                memset(recvBuf6, 0, sizeof(recvBuf6));
                Uart6TxEnable();
                Uart6DMASend(sendBuf6, len);
                step6++;
            }
            else
            {
                step6 = 1;
            }
        }
        break;
    case 3:
        //发送完毕，回去继续接收
        if (Uart6CheckSendEnd())
        {
            memset(sendBuf6, 0, sizeof(sendBuf6));
            step6 = 1;
        }
        break;
    default:
        step6 = 1;
    }
}

static int step1 = 1;
static unsigned char recvBuf1[1024];
static unsigned char sendBuf1[1024];
/**
 * 通讯主程序，外部需要进行调用
 * 
 * @author LXZ (2018-12-03)
 * 
 * @param void 
 */
void Uart1_Modbus_CheckData(void)
{
    int len = 0;

    switch (step1)
    {
    case 0:
        step1 = 1;
        break;
    case 1:
        //采用DMA接收包
        Uart1RxEnable();
        Uart1DMAReceive(recvBuf1, sizeof(recvBuf1));
        step1++;
        break;
    case 2:
        //判断是否有包输入
        len = Uart1CheckRecvEnd();
        if (len > 0)
        {
            if (recvBuf1[0] == ':' && recvBuf1[1] == '0' &&
                recvBuf1[len - 2] == 0x0D &&
                recvBuf1[len - 1] == 0x0A)
            { // DVP_ASCII
                if (ModbusDecodeAscii(recvBuf1, recvBuf1, len) > 0)
                {
                    len = ModbusDelta(sendBuf1, recvBuf1);
                    if (len > 0)
                    {
                        len = ModbusEncodeAscii(sendBuf1, sendBuf1, len);
                    }
                }
            }
            else
            { //RTU
                if (ModbusDecodeRTU(recvBuf1, recvBuf1, len) > 0)
                {
                    len = ModbusStandard(sendBuf1, recvBuf1);
                    if (len > 0)
                    {
                        len = ModbusEncodeRTU(sendBuf1, sendBuf1, len);
                    }
                }
            }

            if (len > 0)
            {
                //发送数据
                memset(recvBuf1, 0, sizeof(recvBuf1));
                Uart1TxEnable();
                Uart1DMASend(sendBuf1, len);
                step1++;
            }
            else
            {
                step1 = 1;
            }
        }
        break;
    case 3:
        //发送完毕，回去继续接收
        if (Uart1CheckSendEnd())
        {
            memset(sendBuf1, 0, sizeof(sendBuf1));
            step1 = 1;
        }
        break;
    default:
        step1 = 1;
    }
}
