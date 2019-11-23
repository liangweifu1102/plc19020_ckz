/**
 * 
 */
#include "../modbus/ModbusSlave.h"
#include "../hardware/inout.h"
#include "../hardware/pwm.h"
#include "../hardware/servostep.h"
#include "../hardware/timer.h"
#include "../hardware/encode.h"
#include "../hardware/serialbus.h"
#include "../hardware/spibus.h"
#include "../hardware/w25qxx.h"
//#include "../hardware/anglogbus.h"
#include "../hardware/iicbus.h"
#include "../usbapp/usb_host.h"
#include "action.h"
#include "usermemory.h"
#include "../mylib/delay.h"
#include "../usbapp/fatfs.h"
//#include "../jlink_rtt/SEGGER_RTT.h"

extern void BlinkRunLED(void);

int main(void)
{
    INTX_ENABLE();

    Stm32_Clock_Init(240, 8, 2, 5);
    Delay_Configuration(120);
    SysTick_Configuration(); //���ʱ��
    hw_serial_bus_init();

    GPIO_init();
    
    MX_FATFS_Init();
    MX_USB_HOST_Init();

    RunLedInit();

    ServoInit();
    hw_spi_bus_init();

    hw_iic_bus_init();
    W25QXX_Init();
    InitVariable();
    EncoderDirSet();
    EncoderInit();
    TIM7_Timer_Init();
    ServoMotorEnable();
    CalGearRatio();
    OutputEnable();
    printf("Init Ok!\r\n");

    while (1)
    {
        ReadInput();
        Action();
        //TestOutput();

        WriteOutput();
        BlinkRunLED();

        MX_USB_HOST_Process(); 
        Uart6_Modbus_CheckData();
        //Uart1_Modbus_CheckData();
    }
}

void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,*/
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    /* USER CODE END 6 */
}

void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    while (1)
    {}
    /* USER CODE END Error_Handler */
}
