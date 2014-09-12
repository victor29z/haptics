#include "includes.h"
#include "can.h"
#include "lib.h"
#include "plc_type.h"
#include "plc_sys.h"
#include "plc.h"
#include <string.h>

//#define DPRINTF		printf
#define DPRINTF		printfNULL

extern const can_driver_t* can_driver[];
extern const int Can_Device_Num;

extern struct_CAN_Buffer CAN_Buffer[];

#if 0
/***********************************************************************************\
								Can中断处理函数
 	参数: 
 		vector，	中断向量
		ndev，		CAN设备号

	Tip: 设备驱动程序在Drivers.c中定义
\***********************************************************************************/
void irq_OSCAN(int vector , int ndev)
{
	int result;
	struct_CAN_Buffer *pcan_Buffer;

	DPRINTF("CAN interrupt: vector=%d, device=%d\n",vector, ndev);

#if OS_ARG_CHK_EN > 0
	if(ndev>=Can_Device_Num)
		return;
#endif

	pcan_Buffer=&CAN_Buffer[ndev];

	while((result=can_driver[ndev]->poll())!=0){
		if(result & CANPOLL_Rev){
			//Receive data

			if(can_driver[ndev]->read(&pcan_Buffer->buffer[pcan_Buffer->nCanRevpos])!=OK){
				DPRINTF("CAN interrupt: receive data error!\n",ndev);
				return;
			}

			DPRINTF("CAN interrupt: receive data, device=%d\n",ndev);
			NextCanRevDataPos(pcan_Buffer->nCanRevpos);
			OSSemPost(pcan_Buffer->Rev_mbox);
		}
	}
}
#endif

/***********************************************************************************\
	收到的一侦数据，得到接收缓冲区指针，
	由驱动程序向Can接收缓冲区写入

 	参数: 
		ndev，		CAN设备号
		data，		接收到的数据

	如果缓冲区没有溢出，则返回数据首地址指针，否则，返回NULL

	Tip: 此函数被Can接收中断处理程序调用
\***********************************************************************************/
PCanData CanPut_RevBuffer(int ndev)
{
	int n;
	struct_CAN_Buffer *pcan_Buffer;
#if OS_ARG_CHK_EN > 0
	if(ndev>=Can_Device_Num)
		return;
#endif

	DPRINTF("CAN receive: device=%d\n", ndev);
	pcan_Buffer=&CAN_Buffer[ndev];
	n=pcan_Buffer->nCanRevpos;

//	memcpy(&pcan_Buffer->Rev_buffer[pcan_Buffer->nCanRevpos], data, sizeof(CanData));
	RoundINC(pcan_Buffer->nCanRevpos, CAN_REVBUFFER_SIZE);

	if(pcan_Buffer->nCanRevpos == pcan_Buffer->nCanReadpos){	//receive buffer full
		printk("CAN%d revbuf full\n", ndev);
		pcan_Buffer->nCanRevpos=n;
		return NULL;
	}

	OSSemPost(pcan_Buffer->Rev_sem);

	return &pcan_Buffer->Rev_buffer[n];
}

/***********************************************************************************\
				从Can发送缓冲区获得一侦数据
 	参数: 
		ndev，		CAN设备号

	如果有数据，则返回数据首地址指针，否则，返回NULL

	Tip: 此函数被Can发送中断处理程序调用
\***********************************************************************************/
PCanData CanGet_SndBuffer(int ndev)
{
	PCanData pdata;
	struct_CAN_Buffer *pcan_Buffer;
#if OS_ARG_CHK_EN > 0
	if(ndev>=Can_Device_Num)
		return NULL;
#endif

	DPRINTF("CAN Send: device=%d\n", ndev);
	pcan_Buffer=&CAN_Buffer[ndev];

	if(pcan_Buffer->nCanSndpos == pcan_Buffer->nCanWritepos){//send buffer empty
		return NULL;
	}

	pdata = &pcan_Buffer->Snd_buffer[pcan_Buffer->nCanSndpos];

	RoundINC(pcan_Buffer->nCanSndpos, CAN_SNDBUFFER_SIZE);
	OSSemPost(pcan_Buffer->Snd_sem);

	return pdata;
}


/***********************************************************************************\
								初始化Can设备									
 	参数: 
		ndev，		CAN设备号
		bandrate，	CAN波特率

	Tip: 设备驱动程序在Drivers.c中定义
\***********************************************************************************/
void Init_Can(int ndev, CanBaudRate baudrate, PCanFilter pfilter)
{
#if OS_ARG_CHK_EN > 0
	if(ndev>=Can_Device_Num)
		return;
#endif

	if(can_driver[ndev]->init)
		can_driver[ndev]->init(baudrate, FALSE, pfilter);
//		can_driver[ndev]->init(baudrate, TRUE, pfilter);
}

/***********************************************************************************\
				打开Can设备(开启中断)
 	参数: 
		ndev，		CAN设备号
\***********************************************************************************/
void Open_Can(int ndev)
{
	INT8U err;
	struct_CAN_Buffer *pcan_Buffer;

#if OS_ARG_CHK_EN > 0
	if(ndev>=Can_Device_Num)
		return;
#endif

	pcan_Buffer=&CAN_Buffer[ndev];

	if(pcan_Buffer->Rev_sem)
		OSSemDel(pcan_Buffer->Rev_sem, OS_DEL_ALWAYS, &err);
	if(pcan_Buffer->Snd_sem)
		OSSemDel(pcan_Buffer->Snd_sem, OS_DEL_ALWAYS, &err);

	pcan_Buffer->Rev_sem=OSSemCreate(0);
	pcan_Buffer->Snd_sem=OSSemCreate(CAN_SNDBUFFER_SIZE-1);

	if(can_driver[ndev]->open)
		can_driver[ndev]->open();
}

/***********************************************************************************\
				关闭Can设备停止接收数据
 	参数: 
		ndev，		CAN设备号
\***********************************************************************************/
void Close_Can(int ndev)
{
	INT8U err;
	struct_CAN_Buffer *pcan_Buffer;
#if OS_ARG_CHK_EN > 0
	if(ndev>=Can_Device_Num)
		return;
#endif

	pcan_Buffer=&CAN_Buffer[ndev];

	if(pcan_Buffer->Rev_sem)
		OSSemDel(pcan_Buffer->Rev_sem, OS_DEL_ALWAYS, &err);
	if(pcan_Buffer->Snd_sem)
		OSSemDel(pcan_Buffer->Snd_sem, OS_DEL_ALWAYS, &err);

	if(can_driver[ndev]->close)
		can_driver[ndev]->close();
}

/***********************************************************************************\
								读取Can设备收到的数据
	参数: 
		ndev，		CAN设备号
		data，		数据结构体
		ntimeout，	超时时间，单位毫秒，如果为0，则永远等待
	返回值:
		成功返回TRUE，否则，返回FALSE
\***********************************************************************************/
BOOL CanRead(int ndev, PCanData data, int ntimeout)
{
	INT8U err;
	struct_CAN_Buffer *pcan_Buffer;
#if OS_ARG_CHK_EN > 0
	if(ndev>=Can_Device_Num)
		return FALSE;
#endif
	pcan_Buffer=&CAN_Buffer[ndev];

	OSSemPend(pcan_Buffer->Rev_sem, ntimeout, &err);
	if(err==OS_TIMEOUT)
		return FALSE;

	memcpy(data, &(pcan_Buffer->Rev_buffer[pcan_Buffer->nCanReadpos]), sizeof(CanData));
	RoundINC(pcan_Buffer->nCanReadpos, CAN_REVBUFFER_SIZE);

	return TRUE;
}

/***********************************************************************************\
								Can设备发送数据
	参数: 
		ndev，		CAN设备号
		data，		数据结构体
		ntimeout，	超时时间，单位毫秒，如果为0，则永远等待
	返回值:
		成功返回TRUE，否则，返回FALSE
\***********************************************************************************/
BOOL CanWrite(int ndev, PCanData pdata, int ntimeout)
{
	INT8U err;
	struct_CAN_Buffer *pcan_Buffer;
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
#endif

#if OS_ARG_CHK_EN > 0
	if(ndev>=Can_Device_Num)
		return FALSE;
	if(can_driver[ndev]->write==NULL)
		return FALSE;
#endif
	pcan_Buffer=&CAN_Buffer[ndev];

	OSSemPend(pcan_Buffer->Snd_sem, ntimeout, &err);
	if(err==OS_TIMEOUT)
		return FALSE;

	OS_ENTER_CRITICAL();
	memcpy(&(pcan_Buffer->Snd_buffer[pcan_Buffer->nCanWritepos]), pdata, sizeof(CanData));
	RoundINC(pcan_Buffer->nCanWritepos, CAN_SNDBUFFER_SIZE);
	can_driver[ndev]->write();
	OS_EXIT_CRITICAL();

	return TRUE;
}

#define CANTEST_TX	1
#define CANTEST_RX	1

static void CAN_Test(void)
{
	int i;
	static CanData revdata, senddata={0x1, {1,2,3,4,5,6,7,8},8, FALSE,FALSE};

	Init_Can(0,BaudRate_1Mbps,NULL);
	Init_Can(1,BaudRate_1Mbps,NULL);
	Open_Can(0);
	Open_Can(1);

#if 0
	for(;;){
		CanWrite(0,&senddata, 0);
	}
#endif

	for(i=0;;i++){
#if (CANTEST_TX == 1)
		OSTimeDly(HZ);
		CanWrite(0,&senddata, 0);
		senddata.id++;
#endif

#if (CANTEST_RX == 1)
		memset(&revdata,0,sizeof(revdata));
		CanRead(1, &revdata,0);
			printk("id=%x\n",revdata.id);
			printk("data=%x,%x,%x,%x,%x,%x,%x,%x\n",
				revdata.data[0],revdata.data[1],revdata.data[2],revdata.data[3],
				revdata.data[4],revdata.data[5],revdata.data[6],revdata.data[7]);
#endif

	}	
}

static int cantest_callback(int argc, char *argv[])
{
	CAN_Test();
	return 0;
}

void CanService_task(void *p_arg){
	CanFilter filter;
	CanData tx_candata,rx_candata;
	int16_t* D_Reg = plc_getMem(plc_D);
	bit_byte* M8_Reg = plc_getMem(plc_M8);
	uint8_t dlc;
	filter.Filter = 0;
	filter.Mask = 0;
	filter.IsExt = 0;
	Init_Can(0, BaudRate_250kbps , &filter);
	Open_Can(0);
	SFR_CAN_TXBUSY = 0;
	SFR_CAN_RXREADY = 0;
	while(1){
		if(SFR_CAN_TXREADY){
        	    SFR_CAN_TXBUSY=1;
                tx_candata.dlc = D_Reg[SFR_CAN_TXDLC_ADD];
				tx_candata.id = D_Reg[SFR_CAN_TXID_ADD];
				tx_candata.IsExt = FALSE;
				tx_candata.IsRTR = FALSE;
				for(U8 i=0;i<tx_candata.dlc;i++){
                	tx_candata.data[i]=D_Reg[SFR_CAN_TXBUF_ADD+i];           
                }
				CanWrite(0,&tx_candata,10);
			SFR_CAN_TXBUSY=0;
			SFR_CAN_TXREADY=0;
		}
		if(CanRead(0, &rx_candata, 100) == TRUE){
			D_Reg[SFR_CAN_RXDLC_ADD] = rx_candata.dlc;
			D_Reg[SFR_CAN_RXID_ADD] = rx_candata.id ;
			for(U8 i=0;i<rx_candata.dlc;i++){
               	D_Reg[SFR_CAN_RXBUF_ADD+i] = rx_candata.data[i];           
				DEBUG_HEX((uint8_t)D_Reg[SFR_CAN_RXBUF_ADD+i]);
				DEBUG(" ");
								
			}
			DEBUG_HEX((uint8_t)(D_Reg[SFR_CAN_RXID_ADD]>>8));
			DEBUG_HEX((uint8_t)(D_Reg[SFR_CAN_RXID_ADD]&0xff));
			DEBUG_LN(" ");
			
			SFR_CAN_RXREADY=1;
			//HexDumpStr(rx_candata.data,rx_candata.dlc);
			//HexDumpStr((uint8_t*)(&rx_candata.id),4);
		}

	}
		

}

//__commandlist(cantest_callback, "cantest", "Usage: cantest");

