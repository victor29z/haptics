#ifndef __UART_H__
#define __UART_H__

#include "marco.h"
#include "ucos_ii.h"

#define UART_REVBUFFER_SIZE		512
#define UART_SNDBUFFER_SIZE		512
typedef int (*uart_write_func_t)(int ndev, U8 data[], int num, int ntimeout);

typedef struct{
	U8 revBuffer[UART_REVBUFFER_SIZE];	//发送缓冲区
	U8 sndBuffer[UART_SNDBUFFER_SIZE];	//接收缓冲区
	OS_EVENT *Uart_Rev_sem;                   //Uart receive sem
	OS_EVENT *Uart_RevFrame_sem;              //Uart receive frame
	OS_EVENT *Uart_Snd_sem;                   //Uart send sem
	int nRev_Uart;	//设定的Uart接收字符数
	U16 nUart_rev;	//Uart 接收缓冲区位置
	U16 nUart_read;	//Uart 读缓冲区位置
	U16 nUart_send;	//Uart 发送缓冲区位置
	U16 nUart_write;	//Uart 写缓冲区位置
	U16 nTimeout;	//Uart 超时设置ms
	U32 nLastRecvTime;
	uart_write_func_t Uart_write;
}struct_Uart_Buffer;

void OSInitUart(int ndev,int baud, int timeout);
//void OSInitUart(void);
int OSReadUart(int ndev, U8 data[], int num, int ntimeout);
int OSWriteUart(int ndev, U8 data[], int num, int ntimeout);
int OpenUartRev(int ndev);
void Put_RevBuffer(int ndev, U8 data);
int Get_SndBuffer(int ndev);
void Uart_FlushRxBuffer(int whichUart);
void Uart_FlushTxBuffer(int whichUart);
void UartMonitor_task(void *p_arg);
int CloseUartRev(int ndev);



#endif //#ifndef __UART_H__

