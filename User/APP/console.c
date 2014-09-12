#include "includes.h"
#include "console.h"

extern struct_Uart_Buffer Uart_Buffer[];

void ConsoleService_task(void *p_arg){
	uint16_t nrx,ntx;
	unsigned char rx_buf[UART_REVBUFFER_SIZE], tx_buf[UART_SNDBUFFER_SIZE];
	uint8_t err;
	
	OSInitUart(__bsp_uart_console,115200,10);	
	OpenUartRev(__bsp_uart_console);
	OSWriteUart(	__bsp_uart_console,
					"hello!",
					7,
					10);
	while(1){
		OSSemPend(Uart_Buffer[__bsp_uart_console].Uart_RevFrame_sem,0,&err);
		
		nrx = RoundCount(Uart_Buffer[__bsp_uart_console].nUart_rev, Uart_Buffer[__bsp_uart_console].nUart_read, UART_REVBUFFER_SIZE);
		OSReadUart(	__bsp_uart_console,
					rx_buf,
					nrx,
					10);
		
		OSWriteUart(	__bsp_uart_console,
					rx_buf,
					nrx,
					10);
		
		
	}

}


