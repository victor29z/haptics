/***************************************************************************\
	Copyright (c) 2004-2007 threewater@up-tech.com, All rights reserved.
	by threewter	2004.4.26
\***************************************************************************/
	

/***************************************************************************\
    #说明: 串口抽象层函数
	----------------------------------  Bug  --------------------------------------

	----------------------------------  TODO list  --------------------------------------

	----------------------------------修正--------------------------------------
	2004-5-26	创建

\***************************************************************************/

#include "marco.h"
#include "includes.h"

#include "uart.h"
#include "serial.h"
//#include "lib.h"

extern const serial_driver_t* serial_drv[];
extern struct_Uart_Buffer Uart_Buffer[];
extern const int NumberOfUartDrv;

void UartMonitor_task(void *p_arg) {
	uint8_t i;
	while(1){
		if(Uart_Buffer[i].Uart_RevFrame_sem != NULL){
			if((Uart_Buffer[i].nLastRecvTime != 0) && 
				(OSTime - Uart_Buffer[i].nLastRecvTime >= Uart_Buffer[i].nTimeout)){
				OSSemPost(Uart_Buffer[i].Uart_RevFrame_sem);
				Uart_Buffer[i].nLastRecvTime = 0;
			}
		}
		i = (i + 1) % 5;
		OSTimeDly(1);
	}
}


/*********************************\
	有数据收到，
	填充接收缓冲区数据
\**********************************/
void Put_RevBuffer(int ndev, U8 data)
{
	int nrev;
	struct_Uart_Buffer *pUart_Buffer;

#if OS_ARG_CHK_EN > 0
	if(ndev>=NumberOfUartDrv)
		return;
#endif
	pUart_Buffer=&Uart_Buffer[ndev];

	//Receive data
	pUart_Buffer->revBuffer[pUart_Buffer->nUart_rev]=data;
	RoundINC(pUart_Buffer->nUart_rev, UART_REVBUFFER_SIZE);

	nrev=RoundCount(pUart_Buffer->nUart_rev, pUart_Buffer->nUart_read, UART_REVBUFFER_SIZE);
	Uart_Buffer[ndev].nLastRecvTime = OSTime;
	if(nrev>=pUart_Buffer->nRev_Uart)
		OSSemPost(pUart_Buffer->Uart_Rev_sem);
}

/*********************************\
	获得发送缓冲区的数据
	成功返回数据
	没有数据，则返回-1
\**********************************/
int Get_SndBuffer(int ndev)
{
	int data;
	int n;
	struct_Uart_Buffer *pUart_Buffer;

#if OS_ARG_CHK_EN > 0
	if(ndev>=NumberOfUartDrv)
		return;
#endif
	pUart_Buffer=&Uart_Buffer[ndev];

	//get data
	n=RoundCount(pUart_Buffer->nUart_write, pUart_Buffer->nUart_send, UART_SNDBUFFER_SIZE);
	if(n==0)
		return -1;

	data=pUart_Buffer->sndBuffer[pUart_Buffer->nUart_send];
	RoundINC(pUart_Buffer->nUart_send, UART_SNDBUFFER_SIZE);

	n=UART_SNDBUFFER_SIZE-n;

	if(n>=UART_SNDBUFFER_SIZE/2)
		OSSemPost(pUart_Buffer->Uart_Snd_sem);

	return data;
}

/*************************
	读串口n的数据
	返回值,实际读取的数据数目
****************************/
int OSReadUart(int ndev, U8 data[], int num, int ntimeout)
{
	INT8U err;
	int nrev, n, snum;
	struct_Uart_Buffer *pUart_Buffer;
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif

#if OS_ARG_CHK_EN > 0
	if(ndev>=NumberOfUartDrv)
		return 0;
#endif
	pUart_Buffer=&Uart_Buffer[ndev];

	snum=num;
	for(; num>0;num-=n){
		OS_ENTER_CRITICAL();
		//设置Uart接收数据数
		n=min(num, UART_REVBUFFER_SIZE);
		pUart_Buffer->nRev_Uart=n;
		nrev=RoundCount(pUart_Buffer->nUart_rev, pUart_Buffer->nUart_read, UART_REVBUFFER_SIZE);
		OS_EXIT_CRITICAL();

		while(nrev<n){
			OSSemPend(pUart_Buffer->Uart_Rev_sem, ntimeout,&err);
			if(err==OS_ERR_TIMEOUT){
				goto end;
			}
			nrev=RoundCount(pUart_Buffer->nUart_rev, pUart_Buffer->nUart_read, UART_REVBUFFER_SIZE);
		}

		OS_ENTER_CRITICAL();
		RoundmemCpyFrom(data, pUart_Buffer->revBuffer, 
			pUart_Buffer->nUart_read, n, UART_REVBUFFER_SIZE);
		OS_EXIT_CRITICAL();
		data+=n;
	}
//	for(i=0;i<num;i++){
//		data[i]=*(pUart_Buffer->revBuffer[pUart_Buffer->nUart_read]);
//		RoundINC(pUart_Buffer->nUart_read, UART_REVBUFFER_SIZE);
//	}
end:
	return snum-num;
}

/*************************
	写串口n的数据
	返回值,实际写入的数据数目
****************************/
int OSWriteUart(int ndev, U8 data[], int num, int ntimeout)
{
	INT8U err;
	int n;
	struct_Uart_Buffer *pUart_Buffer;
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif

#if OS_ARG_CHK_EN > 0
	if(ndev>=NumberOfUartDrv)
		return 0;
#endif
	pUart_Buffer=&Uart_Buffer[ndev];

	OS_ENTER_CRITICAL();
	n=RoundFreeCount(pUart_Buffer->nUart_write, pUart_Buffer->nUart_send, UART_SNDBUFFER_SIZE);
	OS_EXIT_CRITICAL();

	do{
		if(n<num){
			if(n>0){
				OS_ENTER_CRITICAL();
				RoundmemCpyTo(pUart_Buffer->sndBuffer, data, pUart_Buffer->nUart_write, 
					n, UART_SNDBUFFER_SIZE);
				serial_drv[ndev]->write();
				OS_EXIT_CRITICAL();
				num-=n;
			}

			do{
				OSSemPend(pUart_Buffer->Uart_Snd_sem, ntimeout,&err);
				if(err==OS_ERR_TIMEOUT){
					return num;
				}
				n=RoundFreeCount(pUart_Buffer->nUart_write, 
					pUart_Buffer->nUart_send, UART_SNDBUFFER_SIZE);
			}while(n<=UART_SNDBUFFER_SIZE/2 && n<num);
			
		}else{
			OS_ENTER_CRITICAL();
			RoundmemCpyTo(pUart_Buffer->sndBuffer, data, pUart_Buffer->nUart_write, 
				num, UART_SNDBUFFER_SIZE);
			serial_drv[ndev]->write();
			OS_EXIT_CRITICAL();
			return 0;
		}		
	}while(num);

	return num;
}

/*************************
	写串口n的数据
	返回值,实际写入的数据数目
****************************/
int OSWritepollUart(int ndev, U8 data[], int num, int ntimeout)
{
	for(; num>0; num--){
		serial_drv[ndev]->writepoll(*data);
		data++;
	}
	
	return 0;
}


//发送缓冲区清空
void Uart_FlushTxBuffer(int ndev)
{
	struct_Uart_Buffer *pUart_Buffer;
#if OS_ARG_CHK_EN > 0
	if(ndev>=NumberOfUartDrv)
		return;
#endif
	pUart_Buffer=&Uart_Buffer[ndev];

	pUart_Buffer->nUart_send=pUart_Buffer->nUart_write=0;
	serial_drv[ndev]->flush_output();
}

//接收缓冲区清空
void Uart_FlushRxBuffer(int ndev)
{
	struct_Uart_Buffer *pUart_Buffer;
#if OS_ARG_CHK_EN > 0
	if(ndev>=NumberOfUartDrv)
		return;
#endif
	pUart_Buffer=&Uart_Buffer[ndev];

	pUart_Buffer->nUart_rev=pUart_Buffer->nUart_read=0;
	serial_drv[ndev]->flush_input();
}

/***********************************
      初始化串口
***********************************/
void OSInitUart(int ndev,int baud, int timeout)
{
#if OS_ARG_CHK_EN > 0
	if(ndev>=NumberOfUartDrv)
		return;
#endif

	Uart_Buffer[ndev].nRev_Uart=1;		//设定的Uart接收字符数
	Uart_Buffer[ndev].nUart_rev=Uart_Buffer[ndev].nUart_read=0;
	Uart_Buffer[ndev].nUart_send=Uart_Buffer[ndev].nUart_write=0;
	Uart_Buffer[ndev].Uart_Rev_sem=OSSemCreate(0);
	Uart_Buffer[ndev].Uart_RevFrame_sem=OSSemCreate(0);
	Uart_Buffer[ndev].Uart_Snd_sem=OSSemCreate(0);
	Uart_Buffer[ndev].Uart_write = OSWritepollUart;
	Uart_Buffer[ndev].nTimeout = timeout;
	Uart_Buffer[ndev].nLastRecvTime = 0;
	if(serial_drv[ndev]->init)
		serial_drv[ndev]->init(baud);
}

/*************************
	开启串口n中断和缓冲模式
****************************/
int OpenUartRev(int ndev)
{
//	struct_Uart_Buffer *pUart_Buffer;
#if OS_ARG_CHK_EN > 0
	if(ndev>=NumberOfUartDrv)
		return FAIL;
#endif

	Uart_Buffer[ndev].Uart_write = OSWriteUart;
	serial_drv[ndev]->open();

	return OK;
}

int CloseUartRev(int ndev)
{
//	struct_Uart_Buffer *pUart_Buffer;
#if OS_ARG_CHK_EN > 0
	if(ndev>=NumberOfUartDrv)
		return FAIL;
#endif

	//Uart_Buffer[ndev].Uart_write = OSWriteUart;
	serial_drv[ndev]->close();

	return OK;
}


