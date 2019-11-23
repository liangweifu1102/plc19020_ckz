#ifndef __SPI_BUS_H__
#define __SPI_BUS_H__
#include "../mylib/sys.h"

#define SpiDevie1Take()    PDout(2) = 0
#define SPiDevie1UnTake()  PDout(2) = 1

 void hw_spi_bus_init(void);
u8 SPI_ReadWriteByte(u8 TxData);
void SPI_SetSpeed(u8 SpeedSet);
#endif
