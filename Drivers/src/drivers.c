/***************************************************************************\
	Copyright (c) 2004-2007 threewater@up-tech.com, All rights reserved.
	by threewter	2004.4.26
\***************************************************************************/
	

/***************************************************************************\
    #说明: 驱动程序定义
	----------------------------------  Bug  --------------------------------------

	----------------------------------  TODO list  --------------------------------------

	----------------------------------修正--------------------------------------
	2004-4-26	添加串口驱动程序lpc2xxx_serialx_driver

\***************************************************************************/

#include "marco.h"
#include "drivers.h"
//#include "sys/sysdrv.h"

/***************************************************************************\
	for serial drivers
\***************************************************************************/
extern const serial_driver_t STM32_serial0_driver, STM32_serial1_driver, 
		STM32_serial2_driver, STM32_serial3_driver, STM32_serial4_driver;
const serial_driver_t* serial_drv[]={&STM32_serial0_driver, 
		&STM32_serial1_driver, &STM32_serial2_driver, &STM32_serial3_driver, 
		&STM32_serial4_driver};
const int NumberOfUartDrv=NumberOfArray(serial_drv);
struct_Uart_Buffer Uart_Buffer[NumberOfArray(serial_drv)];


/***************************************************************************\
	for can drivers
\***************************************************************************/
/*
*/

//extern const can_driver_t STM32_can0_driver, STM32_can1_driver;
//const can_driver_t* can_driver[]={&STM32_can0_driver, &STM32_can1_driver};
//const int Can_Device_Num=NumberOfArray(can_driver);
//struct_CAN_Buffer CAN_Buffer[NumberOfArray(can_driver)];



