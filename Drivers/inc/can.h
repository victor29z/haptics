#ifndef __CAN_H__
#define __CAN_H__

#include "includes.h"
#include "marco.h"

#define CANTEST		0

//CAN poll �����ķ���ֵ
#define CANPOLL_Rev			1	//���յ�����
#define CANPOLL_SendEnd	2	//�������

typedef enum{
	BaudRate_125kbps=125000,
	BaudRate_250kbps=250000,
	BaudRate_500kbps=500000,
	BaudRate_1Mbps=1000000,
}CanBaudRate;

typedef struct {
	unsigned int id;	//CAN����ID
	unsigned char data[8];		//CAN��������
	unsigned char dlc;		//���ݳ���
	BOOL IsExt; 	//�Ƿ�����չ����
	BOOL IsRTR;	//�Ƿ���Զ��֡
}CanData, *PCanData;

/*********************************************************************\
	CAN�豸���ý��չ������ṹ��
	����:	IdMask��Mask
			IdFilter��Filter
	�Ƿ�������ݰ������¹���:
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
	BOOL IsExt;	//�Ƿ�����չID
}CanFilter,*PCanFilter;



#define CAN_REVBUFFER_SIZE		16
#define CAN_SNDBUFFER_SIZE		16

typedef struct{
	CanData Rev_buffer[CAN_REVBUFFER_SIZE];
	CanData Snd_buffer[CAN_SNDBUFFER_SIZE];
	OS_EVENT *Rev_sem;                   //receive sem
	OS_EVENT *Snd_sem;                   //send sem
	U8 nCanRevpos;					//���ջ�����λ��
	U8 nCanReadpos;					//��ȡ������λ��
	U8 nCanSndpos;					//���ͻ�����λ��
	U8 nCanWritepos;				//д�뻺����λ��
}struct_CAN_Buffer;


/*********************************************************************\
	CAN�豸��ʼ������
	����:	bandrate��CAN������
			LoopBack���Ƿ���LoopBackģʽ
			pfilter��CAN�������ṹ��ָ�룬
					���ΪNULL���������������
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
