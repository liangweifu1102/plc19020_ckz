/*
 * @Description: 
 * @Author: huacong
 * @LastEditors: huacong
 * @Date: 2019-04-17 14:32:41
 * @LastEditTime: 2019-04-18 17:51:28
 */
#ifndef __IIC_BUS_H__
#define __IIC_BUS_H__

#include "../mylib/sys.h"
#include "../mylib/typedefine.h"
//IO��������

#define I2CSIZE 512
#define _24C04_BYTE_NUMBER (I2CSIZE)

extern void hw_iic_bus_init(void);

extern void Write24C04_Byte(unsigned short addr, unsigned char dat);
extern void Write24C04_nByte(uint8 *src, unsigned short addr, uint16 size);
extern void Write24C04_16bit(unsigned short addr, unsigned short dat);
extern void Write24C04_n16bit(uint16 *src, unsigned short addr, uint16 size);
extern void Write24C04_32bit(unsigned short addr, unsigned short dat);

extern unsigned char Read24C04_Byte(unsigned short addr);
extern void Read24C04_nByte(uint8 *dst, uint16 size, uint16 addr);

#endif
