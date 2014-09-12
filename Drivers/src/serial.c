/***************************************************************************\
	Copyright (c) 2004-2007 threewater@up-tech.com, All rights reserved.
	by threewter	2004.4.26
\***************************************************************************/
	

/***************************************************************************\
    #说明: STR9xx串口驱动程序for uCOS-II
	----------------------------------  Bug  --------------------------------------

	----------------------------------  TODO list  --------------------------------------

	----------------------------------修正--------------------------------------
	2005-5-30	创建

\***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "uart.h"
#include "includes.h"
#include "serial.h"
#include "bsp.h"


#ifdef UART_RS485
#define UART_RS485_CTRLIO(n,v)		do{if(v) GPIO_SetBits(UART##n##_RS485_CTRL_PORT, UART##n##_RS485_CTRL_PIN); else GPIO_ResetBits(UART##n##_RS485_CTRL_PORT, UART##n##_RS485_CTRL_PIN);}while(0)
#define UART_RS485_CTRLIO_INIT(n)	do{	GPIO_InitTypeDef GPIO_InitStructure;\
									RCC_APB2PeriphClockCmd(UART##n##_RS485_CTRL_CLK, ENABLE);\
									GPIO_InitStructure.GPIO_Pin = UART##n##_RS485_CTRL_PIN;\
									GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;\
									GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;\
									GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
									GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
									GPIO_Init(UART##n##_RS485_CTRL_PORT, &GPIO_InitStructure);\
								}while(0)
#else
#define UART_RS485_CTRLIO(n,v)
#define UART_RS485_CTRLIO_INIT(n)
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SR_TXE					(1<<7)		//transmit
#define SR_TC					(1<<6)		//transmit complete
#define SR_RXNE					(1<<5)		//receive
#define SR_ORE					(1<<3)		//over load error
/* USART RUN Mask */
//#define CR1_RUN_Set               ((u16)0x2000)  /* USART Enable Mask */
//#define CR1_RWU_Set               ((u16)0x0002)  /* USART mute mode Enable Mask */
//#define CR1_SBK_Set               ((u16)0x0001)  /* USART Break Character send Mask */
//#define CR1_TE						(1<<3)		//enable transmitter
//#define CR1_RE						(1<<2)		//enable receiver
#define CR1_TXEIE					(1<<7)		//enable transmitter interrupt
#define CR1_RXNEIE					(1<<5)		//enable receiver interrupt

//#define CR1_CLEAR_Mask            ((u16)0xE9F3)  /* USART CR1 Mask */

//#define CR2_Address_Mask          ((u16)0xFFF0)  /* USART address Mask */
//#define CR2_LINE_Set              ((u16)0x4000)  /* USART LIN Enable Mask */
//#define CR2_CLEAR_Mask            ((u16)0xC0FF)  /* USART CR2 Mask */

//#define USART_IT_Mask             ((u16)0x001F)  /* USART Interrupt Mask */


/* USART SC Mask */
//#define CR3_SCEN_Set              ((u16)0x0020)  /* USART SC Enable Mask */
/* USART SC NACK Mask */
//#define CR3_NACK_Set              ((u16)0x0010)  /* USART SC NACK Enable Mask */
/* USART Half-Duplex Mask */
//#define CR3_HDSEL_Set             ((u16)0x0008)  /* USART Half-Duplex Enable Mask */
/* USART IrDA Mask */
//#define CR3_IRLP_Mask             ((u16)0xFFFB)  /* USART IrDA LowPower mode Mask */
/* USART LIN Break detection */
//#define CR3_LBDL_Mask             ((u16)0xFFDF)  /* USART LIN Break detection Mask */
/* USART WakeUp Method  */
//#define CR3_WAKE_Mask             ((u16)0xF7FF)  /* USART WakeUp Method Mask */
/* USART IrDA Mask */
//#define CR3_IREN_Set              ((u16)0x0002)  /* USART IrDA Enable Mask */
//#define CR3_CLEAR_Mask            ((u16)0xFCFF)  /* USART CR3 Mask */

//#define GTPR_LSB_Mask             ((u16)0x00FF)  /* Guard Time Register LSB Mask */
//#define GTPR_MSB_Mask             ((u16)0xFF00)  /* Guard Time Register MSB Mask */



//#define Serial_Isr(n)		((n<3) ? (USART1_OFFSET+n):(USART4_OFFSET+n-3))	//串口中断号

USART_TypeDef* const UARTx_Base[]={USART1, USART2, USART3, UART4, UART5};



static void serial_io_init(int ndev)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	switch(ndev){
	case 0:
#if (USART1_REMAP==0) //RX(GPIO A.10, input floating), TX(GPIO A.9, output push-pull)
		RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
		/* Configure GPIOA.9 as push-pull*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/* Configure GPIOA.10 as input floating*/
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
#else	//RX(GPIO B.7, input floating), TX(GPIO B.6, output push-pull)
		RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		//GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
		/* Configure GPIOB.6 as push-pull*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		/* Configure GPIOB.7 as input floating*/
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
#ifdef UART1_RS485
		UART_RS485_CTRLIO_INIT(1);
#endif
		break;
	case 1:	
#if (USART2_REMAP==0) //RX(GPIO A.3, input floating), TX(GPIO A.2, output push-pull)
		RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
		/* Configure GPIOA.2 as push-pull*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/* Configure GPIOA.3 as input floating*/
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

#else	//RX(GPIO D.6, input floating), TX(GPIO D.5, output push-pull)
		RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
		/* Configure GPIOD.6 as push-pull*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOD, &GPIO_InitStructure);
		/* Configure GPIOD.7 as input floating*/
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_USART2);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
		GPIO_Init(GPIOD, &GPIO_InitStructure);

#endif
#ifdef UART2_RS485
		UART_RS485_CTRLIO_INIT(2);
#endif
		break;
	case 2:
#if (USART3_REMAP==0) //RX(GPIO C.11, input floating), TX(GPIO C.10, output push-pull)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
		/* Configure GPIOC.10 as push-pull*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
		/* Configure GPIOC.11 as input floating*/
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
#elif (USART3_REMAP==1)//RX(GPIO C.11, input floating), TX(GPIO C.10, output push-pull)
		//to be fill

#elif (USART3_REMAP==3)//RX(GPIO D.9, input floating), TX(GPIO D.8, output push-pull)
		//to be fill
#else
	#error "ERROR: Invalid USART3_REMAP value."
#endif
#ifdef UART3_RS485
		UART_RS485_CTRLIO_INIT(3);
#endif
		break;
	case 3:	//UART4
		//to be fill
#ifdef UART4_RS485
		UART_RS485_CTRLIO_INIT(4);
#endif
		break;
	case 4:	//UART5
		//to be fill
#ifdef UART5_RS485
		UART_RS485_CTRLIO_INIT(5);
#endif
		break;
	}

}

/***********************************************\
	start_tx and stop_tx is used for rs485
\***********************************************/
static inline void start_tx(int ndev)
{
#ifdef UART_RS485
        int i;
	switch(ndev){
	case 0:
	#ifdef UART1_RS485
		UART_RS485_CTRLIO(1,1);
	#endif
		break;
	case 1:
	#ifdef UART2_RS485
		UART_RS485_CTRLIO(2,1);
	#endif
		break;
	case 2:
	#ifdef UART3_RS485
		UART_RS485_CTRLIO(3,1);
	#endif
		break;
	case 3:
	#ifdef UART4_RS485
		UART_RS485_CTRLIO(4,1);
	#endif
		break;
	case 4:
	#ifdef UART5_RS485
		UART_RS485_CTRLIO(5,1);
	#endif
		break;
	}
	for(i=0;i<20;i++);
#endif
}

static inline void stop_tx(int ndev)
{
#ifdef UART_RS485
	USART_TypeDef* USARTx = UARTx_Base[ndev];
	while(!(USARTx->SR & SR_TC));
	switch(ndev){
	case 0:
	#ifdef UART1_RS485
		UART_RS485_CTRLIO(1,0);
	#endif
		break;
	case 1:
	#ifdef UART2_RS485
		UART_RS485_CTRLIO(2,0);
	#endif
		break;
	case 2:
	#ifdef UART3_RS485
		UART_RS485_CTRLIO(3,0);
	#endif
	break;
		case 3:
	#ifdef UART4_RS485
		UART_RS485_CTRLIO(4,0);
	#endif
		break;
	case 4:
	#ifdef UART5_RS485
		UART_RS485_CTRLIO(5,0);
	#endif
		break;
	}
#endif
}

static int serial_init(int ndev, int baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	USART_TypeDef* USARTx = UARTx_Base[ndev];

	switch(ndev){
	case 0:
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		break;
	case 1:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		break;
	case 2:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		break;
	case 3:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
		break;
	case 4:
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
		break;
	}
	
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTx, &USART_InitStructure);

	serial_io_init(ndev);

	return 0;
}

static int serial_open(int ndev)
{
	USART_TypeDef* USARTx = UARTx_Base[ndev];
	NVIC_InitTypeDef NVIC_InitStructure;
	
	switch(ndev){
	case 0:
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
		break;
	case 1:
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;  
		break;
	case 2:
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;  
		break;
	case 3:
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;  
		break;
	case 4:
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;  
		break;
	}
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//enable recieve interrupt
	USARTx->CR1 |= CR1_RXNEIE;
	USART_Cmd(USARTx, ENABLE);
	return 0;
}

static int serial_close(int ndev)
{
	USART_TypeDef* USARTx = UARTx_Base[ndev];
	NVIC_InitTypeDef NVIC_InitStructure;
	
	switch(ndev){
	case 0:
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
		break;
	case 1:
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;  
		break;
	case 2:
		NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;  
		break;
	case 3:
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;  
		break;
	case 4:
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;  
		break;
	}
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//enable recieve interrupt
	USARTx->CR1 &= ~(uint16_t)CR1_RXNEIE;
	USART_Cmd(USARTx, DISABLE);
	return 0;
}


static int serial_read(int ndev)
{
	return UARTx_Base[ndev]->DR;
}

static int serial_write(int ndev)
{
	USART_TypeDef* USARTx = UARTx_Base[ndev];
	
#ifdef UART_RS485
	start_tx(ndev);
#endif
	USARTx->CR1 |= CR1_TXEIE;	//enable tx interrupt
	return 0;
}

static int serial_writepoll(int ndev, int ch)
{
	USART_TypeDef* USARTx = UARTx_Base[ndev];
#ifdef UART_RS485
	start_tx(ndev);
#endif
	USARTx->DR = ch;
	while(!(USARTx->SR&SR_TXE));

	return 0;
}

static int serial_poll(int ndev)
{
	if(UARTx_Base[ndev]->SR&SR_RXNE)
		return 1;

	return 0;
}

void serial_isr(int ndev)
{
	USART_TypeDef* USARTx = UARTx_Base[ndev];
	unsigned int lsr;
	uint8_t dat,err;
	for(;;){
		lsr = USARTx->SR;
		if(lsr & SR_ORE){// 处理过载错误
			dat = USARTx->DR;
		}
		lsr &= USARTx->CR1&(CR1_RXNEIE|CR1_TXEIE);
		if(lsr==0)
			break;

		if(lsr & SR_TXE){	// TX中断
			int data;

			data=Get_SndBuffer(ndev);
			if(data<0){	//发送缓冲区空
				stop_tx(ndev);
				USARTx->CR1 &= ~CR1_TXEIE;	//enable tx interrupt
			}
			else{
				USARTx->DR = data;
			}
		}

		if(lsr & SR_RXNE){	//接收中断
			dat = USARTx->DR;
			if(dat & 0x80)
				err++;
			Put_RevBuffer(ndev, dat);
		}
	}
}

static int serial_flush_input(int ndev)
{
	while(serial_poll(ndev))
		serial_read(ndev);
	return 0;
}

static int serial_flush_output(int ndev)
{
	while(!(UARTx_Base[ndev]->SR&SR_TXE));
	return 0;
}

#define DefSerialN(n)	static int STM32_serial##n##_init(int baud){return serial_init(n,baud);}\
					static int STM32_serial##n##_open(void){return serial_open(n);}\
					static int STM32_serial##n##_close(void){return serial_close(n);}\
					static int STM32_serial##n##_read(void){return serial_read(n);}\
					static int STM32_serial##n##_write(void){return serial_write(n);}\
					static int STM32_serial##n##_writepoll(int ch){return serial_writepoll(n, ch);}\
					static int STM32_serial##n##_poll(void){return serial_poll(n);}\
					static int STM32_serial##n##_flush_input(void){return serial_flush_input(n);}\
					static int STM32_serial##n##_flush_output(void){return serial_flush_output(n);}

/* export serial driver */
#define ExportSerialN(n)	const serial_driver_t STM32_serial##n##_driver = {	\
							STM32_serial##n##_init,	\
							STM32_serial##n##_open,\
							STM32_serial##n##_close,\
							STM32_serial##n##_read,\
							STM32_serial##n##_write,\
							STM32_serial##n##_writepoll,\
							STM32_serial##n##_poll,\
							STM32_serial##n##_flush_input,\
							STM32_serial##n##_flush_output,\
							}
DefSerialN(0)
DefSerialN(1)
DefSerialN(2)
DefSerialN(3)
DefSerialN(4)


ExportSerialN(0);
ExportSerialN(1);
ExportSerialN(2);
ExportSerialN(3);
ExportSerialN(4);

