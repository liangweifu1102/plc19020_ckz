#ifndef __SERIAL_BUS_H__
#define __SERIAL_BUS_H__
#include "../mylib/sys.h"

#define PRINTF_DEBUG 0 // 1 打开printf  0 关闭printf

//串口总线

#define Uart1TxEnable() PBout(12) = 1 //使能发送
#define Uart1RxEnable() PBout(12) = 0  //使能接收

#define Serial2TransmitEnable() PDout(4) = 1 //使能发送
#define Serial2ReceiveEnable() PDout(4) = 0  //使能接收

#define Uart6TxEnable() PGout(13) = 1 //使能发送
#define Uart6RxEnable() PGout(13) = 0  //使能接收

#define Serial3TransmitEnable() PDout(10) = 1 //使能发送
#define Serial3ReceiveEnable() PDout(10) = 0  //使能接收

void hw_serial_bus_init(void);

int Uart6DMAReceive(void *dst, int length);
int Uart6DMASend(void *src, int length);
int Uart6CheckSendEnd(void);
int Uart6CheckRecvEnd(void);

int Uart1DMASend(void *src, int length);
int Uart1DMAReceive(void *dst, int length);
int Uart1CheckSendEnd(void);
int Uart1CheckRecvEnd(void);

#endif
