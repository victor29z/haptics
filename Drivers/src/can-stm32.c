/***************************************************************************\
	Copyright (c) 2004-2007 threewater@up-tech.com, All rights reserved.
	by threewter	2004.4.26
\***************************************************************************/
	

/***************************************************************************\
    #说明: STM32 CAN 总线驱动程序for uCOS-II
	----------------------------------  Bug  --------------------------------------

	----------------------------------  TODO list  --------------------------------------

	----------------------------------修正--------------------------------------
	Created by charles 2008-4-8

\***************************************************************************/



/* Includes ------------------------------------------------------------------*/
#include "can.h"
#include "includes.h"
//#include "drv/register.h"

#define CAN1_REMAP	0
#define CAN2_REMAP	0

//#define DPRINTF		printf
#define DPRINTF		printfNULL

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/


/* Tx and Rx mailbox identifier register  */
#define CAN_IxR_TXRQ		((u32)0x00000001)	/* Transmit mailbox request */
#define CAN_IxR_STID		((u32)0x000007FF)	/* Standard Identifier */
#define CAN_IxR_SEID		((u32)0x1FFFFFFF)	/* Standard and Extended Identifier */
#define CAN_IxR_IDE			((u32)0x00000004)	/* The identifier type of message */
#define CAN_IxR_RTR			((u32)0x00000002)	/* Remote Transmission Request */



/* CAN operating mode */
#define CAN_Mode_SLBack	((u8)0x03)	/* loopback combined with silent mode */


/* Interrupt Service vector*/
#define CAN_ISR_TX(n)		(((n) == 0)? CAN_TX_OFFSET : CAN2_TX_OFFSET) /* CAN TX Interrupts */
#define CAN_ISR_RX1(n)		(((n) == 0)? CAN_RX1_OFFSET : CAN2_RX1_OFFSET) /* CAN RX1 Interrupt */


/* Private macro -------------------------------------------------------------*/

/* CAN Init result */

#define CAN_Filt_Num(n)		((u32)0x00000001 << ((n)%28))
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

//#define DPRINTK		printk
#define DPRINTK		printfNULL


static CAN_TypeDef* const CANx_Base[]={CAN1, CAN2};

/*******************************************************************************
* Function Name	: can_isr_rev
* Description		: Called by Receive Interrupt, move data from FIFO 0 to receive buffer.
* Input			: Interrupt Vector, Can device number ndev.
* Output			: None.
* Return			: None.
*******************************************************************************/
void can_isr_rev(int ndev)
{
	CanRxMsg rxmsg;
	int RxFx = 0;
	PCanData pdata;
	
	pdata = CanPut_RevBuffer(ndev);
	if(pdata == NULL){
		DPRINTK("pdata=NULL\n");
		CAN_FIFORelease(CAN1,CAN_FIFO0);// 释放一 FIFO0
		return;
	}
	
	/* Get the Id */

	pdata->IsExt = FALSE;
	pdata->IsRTR = FALSE;
	if(CAN_IxR_IDE & CANx_Base[ndev]->sFIFOMailBox[RxFx].RIR){
		pdata->IsExt = TRUE;
		pdata->id = CAN_IxR_SEID & (CANx_Base[ndev]->sFIFOMailBox[RxFx].RIR >> 3);
	}
	else
		pdata->id = CAN_IxR_STID & (CANx_Base[ndev]->sFIFOMailBox[RxFx].RIR >> 21);
	
	if(CAN_IxR_RTR & CANx_Base[ndev]->sFIFOMailBox[RxFx].RIR)
		pdata->IsRTR= TRUE;

	/* Get the DLC */
	pdata->dlc = (u32)0x0000000F & CANx_Base[ndev]->sFIFOMailBox[RxFx].RDTR;

	/* Get the FMI 
	RxMessage->FMI = (u32)0x000000FF & (CANx_Base[ndev]->sFIFOMailBox[RxFx].RDTR >> 8);
	*/

	/* Get the data field */
	*((u32*)pdata->data)= CANx_Base[ndev]->sFIFOMailBox[RxFx].RDLR;
	*(((u32*)pdata->data)+1)= CANx_Base[ndev]->sFIFOMailBox[RxFx].RDHR;

	/* Release the FIFO0 */

	CAN_FIFORelease(CAN1,CAN_FIFO0);// 释放一 FIFO0
}


/*******************************************************************************
* Function Name	: can_isr_snd
* Description		: Called by Sned Interrupt, put data into mailbox to receive buffer.
* Input			: Interrupt Vector, Can device number ndev.
* Output			: None.
* Return			: None.
*******************************************************************************/
void can_isr_snd(int ndev)
{
	int TxMx = 1;
		PCanData pdata;
			//struct_CAN_Buffer *pcan_Buffer;
		vu32 state;
		
			//pcan_Buffer=&CAN_Buffer[ndev];
			
			// Select one empty transmit mailbox 
		if ((CANx_Base[ndev]->TSR&CAN_TSR_TME0) == CAN_TSR_TME0)
			TxMx = 0;
		else if ((CANx_Base[ndev]->TSR&CAN_TSR_TME1) == CAN_TSR_TME1)
			TxMx = 1;
		else if ((CANx_Base[ndev]->TSR&CAN_TSR_TME2) == CAN_TSR_TME2)
			TxMx = 2;
		else
			return;
		
		// If transmit mailbox0..2 is empty, use it
//		if (TxMx != CAN_NO_MB)
			{
					pdata=CanGet_SndBuffer(ndev);
					if(pdata != NULL)
					{
					// Clear CAN_TIxR except TXRQ 
					CANx_Base[ndev]->sTxMailBox[TxMx].TIR &= CAN_IxR_TXRQ;
							
					// Set CAN_TIxR x=0..2 
					if(pdata->IsExt){
						CANx_Base[ndev]->sTxMailBox[TxMx].TIR |= CAN_IxR_IDE;
						CANx_Base[ndev]->sTxMailBox[TxMx].TIR |= (CAN_IxR_SEID & pdata->id)<<3;
					}
					else
						CANx_Base[ndev]->sTxMailBox[TxMx].TIR |= (CAN_IxR_STID & pdata->id)<<21;
					if(pdata->IsRTR){
						CANx_Base[ndev]->sTxMailBox[TxMx].TIR |= CAN_IxR_RTR;
					}
					// Set up the DLC 
					pdata->dlc &= (u8)0x0F;
					CANx_Base[ndev]->sTxMailBox[TxMx].TDTR &= (u32)0xFFFFFFF0;
					CANx_Base[ndev]->sTxMailBox[TxMx].TDTR |= (u32)pdata->dlc;
					// Set up the data field 
					CANx_Base[ndev]->sTxMailBox[TxMx].TDLR = *((u32*)pdata->data);
					CANx_Base[ndev]->sTxMailBox[TxMx].TDHR = *(((u32*)pdata->data)+1);
					// Request transmission 		
					CANx_Base[ndev]->sTxMailBox[TxMx].TIR |= CAN_IxR_TXRQ;
					// Read the TSR to clear 
					state = CANx_Base[ndev]->TSR;
						   
							//RoundINC(pcan_Buffer->nCanSndpos, CAN_SNDBUFFER_SIZE);
						//OSSemPost(pcan_Buffer->Snd_sem);
				}
					else
					{
	//			DPRINTK("pdata=NULL\n");
						CANx_Base[ndev]->IER &= ~CAN_IER_TMEIE;
					}
			}//TxMx != CAN_NO_MB

}


/*******************************************************************************
* Function Name	: TxStateCheck
* Description		: Check the send status.
* Input			: None.
* Output			: Tramsmit status
* Return			: None.
*******************************************************************************/
int TxStateCheck(int ndev)
{
	int state = 0;
	state |= ((CANx_Base[ndev]->TSR & CAN_TSR_RQCP0) << 2);
	state |= ((CANx_Base[ndev]->TSR & CAN_TSR_TXOK0) >> 0);
	state |= ((CANx_Base[ndev]->TSR & CAN_TSR_TME0) >> 26);
	switch (state){
	/* transmit pending  */
	case 0:
		state = CANTXPENDING;
		break;
	/* transmit failed  */
	case 5:
		state = CANTXFAILED;
		break;
	/* transmit succedeed  */
	case 7:
		state = CANTXOK;
		break;
	default:
		state = CANTXFAILED;
		break;
	}
	return state;
}

#define CANBTR_REG(sjw, bs1, bs2, brp)	((u32)(((sjw)<<24)|((bs1)<<16)|((bs2)<<20)|((brp))))

/**
 * cal_baud_rate: - set communication parameters.
 * @chip: pointer to chip state structure
 * @sjw: synchronization jump width (0-3) prescaled clock cycles
 * @sampl_pt: sample point in % (0-100) sets (TSEG1+1)/(TSEG1+TSEG2+2) ratio
 * @flags: fields %BTR1_SAM, %OCMODE, %OCPOL, %OCTP, %OCTN, %CLK_OFF, %CBP
 *
 * Return Value: negative value reports error.
 */
#define CAN_sampl_pt 	75
#define CAN_sjw 		0
#define MAX_TSEG1		15
#define MAX_TSEG2		7

static int cal_baud_rate(int rate, int clock)
{
	int best_error = 1000000000, error;
	int best_tseg=0, best_brp=0, best_rate=0, brp=0;
	int tseg=0, tseg1=0, tseg2=0;
	
	/* tseg even = round down, odd = round up */
	for (tseg=(0+0+2)*2; tseg<=(MAX_TSEG2+MAX_TSEG1+2)*2+1; tseg++) {
		brp = clock/((1+tseg/2)*rate)+tseg%2;
		if (brp == 0 || brp > 64)
			continue;
		error = rate - clock/(brp*(1+tseg/2));
		if (error < 0)
			error = -error;
		if (error <= best_error) {
			best_error = error;
			best_tseg = tseg/2;
			best_brp = brp-1;
			best_rate = clock/(brp*(1+tseg/2));
		}
	}
	if (best_error && (rate/best_error < 10)) {
		printk("baud rate %d is not possible with %d Hz clock\n",
								rate, 2*clock);
		printk("%d bps. brp=%d, best_tseg=%d, tseg1=%d, tseg2=%d\n",
				best_rate, best_brp, best_tseg, tseg1, tseg2);
		return -1;
	}
	tseg2 = best_tseg-(CAN_sampl_pt*(best_tseg+1))/100;
	if (tseg2 < 0)
		tseg2 = 0;
	if (tseg2 > MAX_TSEG2)
		tseg2 = MAX_TSEG2;
	tseg1 = best_tseg-tseg2-2;
	if (tseg1>MAX_TSEG1) {
		tseg1 = MAX_TSEG1;
		tseg2 = best_tseg-tseg1-2;
	}

	DPRINTF("Setting %d bps.\n", best_rate);
	DPRINTF("brp=%d, best_tseg=%d, tseg1=%d, tseg2=%d, sampl_pt=%d\n",
					best_brp, best_tseg, tseg1, tseg2,
					(100*(best_tseg-tseg2)/(best_tseg+1)));

	return CANBTR_REG(CAN_sjw, tseg1, tseg2, best_brp);
}



/*******************************************************************************
* Function Name	: can_io_init
* Description		: Initialize the GPIO
* Input			: None.
* Output			: None.
* Return			: None.
*******************************************************************************/
static void can_io_init(int ndev)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/* AFIO clock enable */	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	switch(ndev){
	case 0:	//CAN 1
#if(CAN1_REMAP==0)
	  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		/* Configure GPIOA.11 as Input floating mode for RX */	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/* Configure GPIOA.12 as Alternate Function output Push-Pull for TX */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
#elif (CAN1_REMAP==2)
	  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		/* Configure GPIOB.8 as Input floating mode for RX */	
		
		/* Configure GPIOB.9 as Alternate Function output Push-Pull for TX */
		
#elif(CAN1_REMAP==3)
	  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
		/* Configure GPIOD.0 as Input floating mode for RX */	
		
		/* Configure GPIOD.1 as Alternate Function output Push-Pull for TX */
		
#endif

		/* CAN Reset and Periph clock enable */
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

		break;
	case 1:	//CAN 2
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
#if(CAN2_REMAP==0)
		/* Configure GPIOB.12 as Input floating mode for RX */	
		
		/* Configure GPIOB.13 as Alternate Function output Push-Pull for TX */
		
#elif(CAN2_REMAP==1)
		/* Configure GPIOB.5 as Input floating mode for RX */	
		
		/* Configure GPIOB.6 as Alternate Function output Push-Pull for TX */
		
#endif
		/* CAN Reset and Periph clock enable */
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN2, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN2, DISABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
		break;
	}
	
}

static void Init_filter(int ndev, PCanFilter pfilter)
{
	

	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	
	CAN_FilterInitStructure.CAN_FilterNumber=0; 					//选择过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;	//标识符屏蔽位模式
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;	//32位过滤器

	CAN_FilterInitStructure.CAN_FilterIdHigh =pfilter->Filter >> 16;	//接收板的CAN地址
	CAN_FilterInitStructure.CAN_FilterIdLow = pfilter->Filter & 0xffff;				//选择扩展标识符（见手册CAN_RIxR）
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh =0; //pfilter->Mask >> 16;//0xffff;		//not use a filter
	CAN_FilterInitStructure.CAN_FilterMaskIdLow =0; //pfilter->Mask & 0xffff;			//not use a filter

	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0; 	//选择FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;			//使能过滤器
	CAN_FilterInit(&CAN_FilterInitStructure); 						//进入初始化函数

}


/*******************************************************************************
* Function Name	: can_init
* Description		: Initialize the CAN peripheral.
* Input			: CanBandRate bandrate, BOOL LoopBack, PCanFilter pfilter
* Output			: None.
* Return			: Constant indicates initialization succeed which will be 
* 				CANINITFAILED or CANINITOK.
*******************************************************************************/
static int can_init(int ndev, CanBaudRate baudrate, BOOL LoopBack, PCanFilter pfilter)
{
	int InitStatus;
//	int brp;
	RCC_ClocksTypeDef RCC_ClocksStatus;
	
	/* RCC Config------------------------------------------------------------------*/

	can_io_init(ndev);

	/*----------------------------------------------------------------------------*/
	/* Init CAN registers */
	/* Request initialisation */
	CANx_Base[ndev]->MCR = CAN_MCR_INRQ;

	/* Check acknowledged */
	if ((CANx_Base[ndev]->MSR & CAN_MSR_INAK) == 0){
		InitStatus = CANINITFAILED;
	}
	else{
		/* Set the time triggered communication mode */
		//CANx_Base[ndev]->MCR |= CAN_MCR_TTCM;
		CANx_Base[ndev]->MCR &= ~CAN_MCR_TTCM;

		/* Set the automatic bus-off management */		
		//CANx_Base[ndev]->MCR |= CAN_MCR_ABOM;
		CANx_Base[ndev]->MCR &= ~CAN_MCR_ABOM;

		/* Set the automatic wake-up mode */
		//CANx_Base[ndev]->MCR |= CAN_MCR_AWUM;
		CANx_Base[ndev]->MCR &= ~CAN_MCR_AWUM;

		/* Set the no automatic retransmission */
		//CANx_Base[ndev]->MCR |= CAN_MCR_NART;
		CANx_Base[ndev]->MCR &= ~CAN_MCR_NART;

		/* Set the receive FIFO locked mode */
		//CANx_Base[ndev]->MCR |= CAN_MCR_RFLM;
		CANx_Base[ndev]->MCR &= ~CAN_MCR_RFLM;

		/* Set the transmit FIFO priority */
		CANx_Base[ndev]->MCR |= CAN_MCR_TXFP;
		//CANx_Base[ndev]->MCR &= ~CAN_MCR_TXFP;

		/* Interrupt Register Clear */
		CANx_Base[ndev]->IER &= 0x00000000;
				
		/* Config BaudRate-----------------------------------------------------------------*/
		/* brp means Baud Rate Prescaler, tq=(BRP[9:0]+1)*tPCLK, tPCLK=1/PCLK1_Frequency--------*/
		RCC_GetClocksFreq(&RCC_ClocksStatus);
		CANx_Base[ndev]->BTR = cal_baud_rate(baudrate, RCC_ClocksStatus.PCLK1_Frequency);
		/*-----------------------------------------------------------------------*/

		/* Set operating mod */
		if(LoopBack)
//			CANx_Base[ndev]->BTR |= ((u32)CAN_Mode_SLBack<< 30);
			CANx_Base[ndev]->BTR |= ((u32)CAN_Mode_LoopBack << 30);
		else
			CANx_Base[ndev]->BTR |= ((u32)CAN_Mode_Normal << 30);

		/* Config CAN_Filter---------------------------------------------------------*/
		Init_filter(ndev, pfilter);

		InitStatus = CANINITOK;
		
		/* Request leave initialisation */
		CANx_Base[ndev]->MCR &= ~CAN_MCR_INRQ;
	}
//	if((CANx_Base[ndev]->MSR & CAN_MSR_INAK) != CAN_MSR_INAK)
//	    InitStatus = CANINITFAILED;
	return InitStatus;
}


/*******************************************************************************
* Function Name	: can_open
* Description		: Regedit the Can Receive and Send Interrupt service Routine, 
* 				  and enable the receive interrupt.
* Input			: Can device number
* Output			: None.
* Return			: None.
*******************************************************************************/
static void can_open (int ndev)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* regedit the can_isr */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;
	NVIC_Init(&NVIC_InitStructure);
	/* FIFO 1 message pending Interrupt Enable */
	CANx_Base[ndev]->IER |= CAN_IER_FMPIE0;
	
}


/*******************************************************************************
* Function Name	: can_close
* Description		: Disable the receive and send interrupt.
* Input			: None.
* Output			: None.
* Return			: None.
*******************************************************************************/
static void can_close (int ndev)
{
	/* FIFO 0 message pending Interrupt Disable */
	CANx_Base[ndev]->IER &= ~CAN_IER_FMPIE0;
	/* Transmit mailbox empty Interrupt Disable */
	CANx_Base[ndev]->IER &= ~CAN_IER_TMEIE;
}


/*******************************************************************************
* Function Name	: can_write
* Description		: enable the send  interrupt. 
* 				  At the first time should call the isr directly, 
* 				  then it will be called by send interrupt
* Input			: Can device number
* Output			: None.
* Return			: None.
*******************************************************************************/
static int can_write (int ndev)
{
	/* Transmit mailbox empty Interrupt Enable */
	DPRINTK("%s\n", __FUNCTION__);
	if((CANx_Base[ndev]->IER & CAN_IER_TMEIE) != CAN_IER_TMEIE){
		CANx_Base[ndev]->IER |= CAN_IER_TMEIE;
		can_isr_snd(ndev);
	}
	return 0;
}


/*******************************************************************************
* Function Name	: can_flush_input
* Description		:
* Input			:
* Output			: None.
* Return			: None.
*******************************************************************************/
static int can_flush_input (int ndev)
{
	return 0;
}


/*******************************************************************************
* Function Name	: can_flush_output
* Description		:
* Input			:
* Output			: None.
* Return			: None.
*******************************************************************************/
static int can_flush_output (int ndev)
{
	return 0;
}

#define DefCanN(n)	static int STM32_can##n##_init(CanBaudRate baudrate, BOOL LoopBack, PCanFilter pfilter)\
							{return can_init(n, baudrate,LoopBack,pfilter);}\
					static void STM32_can##n##_open(void){can_open(n);}\
					static void STM32_can##n##_close(void){can_close(n);}\
					static int STM32_can##n##_write(void){return can_write(n);}\
					static int STM32_can##n##_flush_input(void){return can_flush_input(n);}\
					static int STM32_can##n##_flush_output(void){return can_flush_output(n);}


/* export can driver */

#define ExportCanN(n)	const can_driver_t STM32_can##n##_driver = {\
								STM32_can##n##_init,\
								STM32_can##n##_open,\
								STM32_can##n##_close,\
								STM32_can##n##_write,\
								STM32_can##n##_flush_input,\
								STM32_can##n##_flush_output,\
								}

DefCanN(0)
DefCanN(1)

ExportCanN(0);
ExportCanN(1);

