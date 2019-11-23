/*
 * modusASC.h
 *
 *  Created on: 2011-10-14
 *      Author: lenovo
 */

#ifndef MODUSASC_H_
#define MODUSASC_H_

#define MB_NO_ERR 0x00
#define ILLEGAL_FUNCTION 0x01
#define ILLEGAL_DATA_ADDR 0x02
#define ILLEGAL_DATA_VALUE 0x03
#define SLAVE_DEVICE_FAILURE 0x04
#define ACKNOWLEDGE 0x05
#define SLAVE_DEVICE_BUSY 0x06
#define MEMORY_PARITY_ERR 0x08
#define GATEWAY_PATH_UNABLE 0x0A
#define GATEWAY_TARGET_DEV_FAIL 0x0B

#define MB_FUN_NOT_DEF 0x80
#define MB_MASTER_BUSY 0x81
#define MB_MASTER_IDLE 0x82
#define MB_RESPONSE_TIME_OUT 0x82
#define MB_PARAMETER_ERR 0x83

extern void Uart6_Modbus_CheckData(void);
extern void Uart1_Modbus_CheckData(void);
#endif /* MODUSASC_H_ */
