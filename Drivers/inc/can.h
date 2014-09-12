#ifndef __CAN_H__
#define __CAN_H__

#include "includes.h"
#include "marco.h"

#define CANTEST		0

//CAN poll 函数的返回值
#define CANPOLL_Rev			1	//接收到数据
#define CANPOLL_SendEnd	2	//发送完毕

typedef enum{
	BaudRate_125kbps=125000,
	BaudRate_250kbps=250000,
	BaudRate_500kbps=500000,
	BaudRate_1Mbps=1000000,
}CanBaudRate;

typedef struct {
	unsigned int id;	//CAN总线ID
	unsigned char data[8];		//CAN总线数据
	unsigned char dlc;		//数据长度
	BOOL IsExt; 	//是否是扩展总线
	BOOL IsRTR;	//是否是远程帧
}CanData, *PCanData;

/*********************************************************************\
	CAN设备设置接收过滤器结构体
	参数:	IdMask，Mask
			IdFilter，Filter
	是否接收数据按照如下规律:
	Mask	Filter	RevID	Receive
	0		x		x		yes
	1		0		0		yes
	1		0		1		no
	1		1		0		no
	1		1		1		yes
	
\*********************************************************************/
typedef struct{
	unsigned int Mask;
	unsigned int Filter;
	BOOL IsExt;	//是否是扩展ID
}CanFilter,*PCanFilter;



#define CAN_REVBUFFER_SIZE		16
#define CAN_SNDBUFFER_SIZE		16

typedef struct{
	CanData Rev_buffer[CAN_REVBUFFER_SIZE];
	CanData Snd_buffer[CAN_SNDBUFFER_SIZE];
	OS_EVENT *Rev_sem;                   //receive sem
	OS_EVENT *Snd_sem;                   //send sem
	U8 nCanRevpos;					//接收缓冲区位置
	U8 nCanReadpos;					//读取缓冲区位置
	U8 nCanSndpos;					//发送缓冲区位置
	U8 nCanWritepos;				//写入缓冲区位置
}struct_CAN_Buffer;


/*********************************************************************\
	CAN设备初始化函数
	参数:	bandrate，CAN波特率
			LoopBack，是否是LoopBack模式
			pfilter，CAN过滤器结构体指针，
					如果为NULL，则接受所有数据
\*********************************************************************/
typedef int (*can_init_func_t)(CanBaudRate baudrate, BOOL LoopBack, PCanFilter pfilter);

typedef void (*can_open_func_t)(void);
typedef void (*can_close_func_t)(void);
typedef int (*can_write_func_t)(void);
typedef int (*can_flush_input_func_t)(void);
typedef int (*can_flush_output_func_t)(void);


typedef struct {
	can_init_func_t init;
	can_open_func_t open;
	can_close_func_t close;
	can_write_func_t write;

	can_flush_input_func_t flush_input;
	can_flush_output_func_t flush_output;
} can_driver_t;

void Init_Can(int ndev, CanBaudRate baudrate, PCanFilter pfilter);
void Open_Can(int ndev);
void Close_Can(int ndev);

BOOL CanRead(int ndev, PCanData data, int ntimeout);
BOOL CanWrite(int ndev, PCanData pdata, int ntimeout);

PCanData CanPut_RevBuffer(int ndev);
PCanData CanGet_SndBuffer(int ndev);

void CAN_Test(void);
void can_isr_rev(int ndev);
void can_isr_snd(int ndev);

void CanService_task(void *p_arg);


#endif
