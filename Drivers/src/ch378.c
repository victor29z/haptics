#include "ch378.h"
#include "includes.h"
/* ���ӵ�USB����״̬���� */
#define	ERR_USB_UNKNOWN			   0xFA							 /* δ֪����,��Ӧ�÷��������,����Ӳ�����߳������ */

void xWriteCH378Data( UINT8 mData )  		
{
    u8 i=0xff;
	CH378_RESET_A0();
	CH378_RESET_nPCS();
	CH378_WRITE_DB(mData);
	CH378_SET_nRD();
	CH378_RESET_nWR();
	//OSTimeDly(1);
	while(i--);
	CH378_SET_nWR();
	CH378_SET_nPCS();
}


void xWriteCH378Cmd( UINT8 mCmd )  		
{
	u8 i=0xff;
	CH378_SET_A0();
	CH378_RESET_nPCS();
	CH378_WRITE_DB(mCmd);
	CH378_SET_nRD();
	CH378_RESET_nWR();
	//OSTimeDly(1);
	while(i--);
	CH378_SET_nWR();
	CH378_SET_nPCS();
}

u8  xReadCH378Data( void  )  		
{
	UINT8 mData;
	u8 i=0xff;
	CH378_RESET_A0();
	CH378_WRITE_DB(0xff);//clear the bus
	CH378_RESET_nPCS();
	CH378_SET_nWR();
	CH378_RESET_nRD();
	//OSTimeDly(1);
	while(i--);
	mData = CH378_READ_DB();
	CH378_SET_nRD();
	CH378_SET_nPCS();

	return (mData);
}     

u8 mInitCH378Device( void ) 
{
	u8  res;
	
	/* ���CH378�����Ƿ����� */	
//	CH378_PORT_INIT( );  										 				/* �ӿ�Ӳ����ʼ�� */
	xWriteCH378Cmd( CMD11_CHECK_EXIST );  						 				/* ���Ե�Ƭ����CH378֮���ͨѶ�ӿ� */
	xWriteCH378Data( 0x65 );
	res = xReadCH378Data( );
//	printf( "res1: %02X\n", (UINT16)res );
//	xEndCH378Cmd( );
	if( res != 0x9A ) 
	{
		/* ͨѶ�ӿڲ�����,����ԭ����:�ӿ������쳣,�����豸Ӱ��(Ƭѡ��Ψһ),���ڲ�����,һֱ�ڸ�λ,���񲻹��� */
		return( ERR_USB_UNKNOWN );  
	}	

#if  1
	/* ����VID��PID */
	xWriteCH378Cmd( CMD40_SET_USB_ID );  						 				/* �豸оƬVID��PID */
	xWriteCH378Data( 0x43 );									 			
	xWriteCH378Data( 0x48 );									 			
	xWriteCH378Data( 0x55 );									 			
	xWriteCH378Data( 0x37 );									 			
#endif
									
#if  0
	/* ����USB �ٶ� */
	xWriteCH378Cmd( CMD10_SET_USB_SPEED );  						 			/* �豸оƬUSB�ٶ� */
	xWriteCH378Data( 0x00 );													/* ȫ��ģʽ */									 												 			
	//xEndCH378Cmd( );
#endif

	/* ����CH378����ģʽ */
	xWriteCH378Cmd( CMD11_SET_USB_MODE );  						 				/* �豸USB����ģʽ */
	xWriteCH378Data( 0x02 );									 				/* �������ù̼�ģʽ */

	/* �ȴ�ģʽ�������,���ڲ���SD�������Ҫ10mS����ʱ��,���ڲ���USB�豸�����Ҫ35mS����ʱ�� */	
	
	OSTimeDlyHMSM(0,0,0,70);
	res = xReadCH378Data( );
//	printf( "res2: %02X\n", (UINT16)res );
	

#ifndef	CH378_INT_WIRE
	xWriteCH378Cmd( CMD20_SET_SDO_INT );  						 				/* ����SPI��SDO���ŵ��жϷ�ʽ */
	xWriteCH378Data( 0x16 );
	xWriteCH378Data( 0x01 );  									 				/* SDO������SCSƬѡ��Чʱ�����ж�������� */
	
#endif
	if( res == CMD_RET_SUCCESS ) 
	{
		return( ERR_SUCCESS );
	}
	else 
	{
		return( ERR_USB_UNKNOWN );  							 				/* ����ģʽ���� */
	}
}	
u8 Out_Packet[15] = {0};     // Last packet received from host
u8 In_Packet[15]  = {0};     // Next packet to sent to host
// dev_feedback control word
#define 	DEV_POS_STA			1
// USB micro
#define		CMD_LENGTH_HOST2DEV			13
#define		CMD_LENGTH_DEV2HOST			14
// control word
#define CTW_COMMON			0
#define CTW_SIMPLE			1
#define	CTW_PMW_SETING		2
#define CTW_OPEN_FORCE		3
#define	CTW_SET_POS			4
#define CTW_PID				5

void	mCH378Interrupt( void )
{
	u8  status;
	u8 len, l,i;
	u8  *pbuf;

	u32		u32Temp = 0;
	u8		IState[3] = {0};
//	U8 		devState = 0;
//	U8 		watchT[2]={0};
	
//	O2=1;
	xWriteCH378Cmd( CMD01_GET_STATUS );									/* ���Ͷ�ȡ�ж�״̬���� */
	status = xReadCH378Data( );
//		xEndCH378Cmd( );
	switch( status )
	{
	case USB_INT_INDEX1_IN:											/* ����1��Ӧ�Ķ˵��ϴ��ɹ��ж� */
//		printf("INDEX1_IN\n");
	break;

	case USB_INT_INDEX2_OUT:										/* ����2��Ӧ�Ķ˵��´��ɹ��ж� */
//		printf("INDEX2_OUT\n");

	break;

	case USB_INT_INDEX3_IN:											/* ����3��Ӧ�Ķ˵��ϴ��ɹ��ж� */
//		printf("INDEX3_IN\n");
	break;

	case USB_INT_INDEX4_OUT:										/* ����4��Ӧ�Ķ˵��´��ɹ��ж�,378 circle */
//		printf("INDEX4_OUT \n");
//		printf("INDEX4_OUT \n");
		// for performance check
		GPIO_ToggleBits(GPIOH,GPIO_Pin_11);

		pbuf = Out_Packet;
		xWriteCH378Cmd( CMD1x_RD_USB_DATA );						/* ��ָ��USB�жϵĶ˵㻺������ȡ���ݿ�,���ͷŻ����� */		
		xWriteCH378Data( ENDP_INDEX_4 );		   					/* �˵����� */
		len = xReadCH378Data( );
		len += ( ( u16 )xReadCH378Data( ) << 8 );				/* ��ȡ2���ֽ����ݳ��� */
//		printf( "len2: %02X\n", (UINT16)len );
		l = len;
		if( len )													/* ѭ����ȡ���ݿ� */
		{
			do
			{		
				// *pbuf = ~(xReadCH378Data( ));		// inverse type
				*pbuf = xReadCH378Data( );
				pbuf++;
			}while( --l );		
		}
			// input part
		pbuf = In_Packet;
		In_Packet[0] = DEV_POS_STA;					// new modification
	//	pbuf = Out_Packet;
		xWriteCH378Cmd( CMD30_WR_USB_DATA );					 	/* ��ָ��USB�˵�ķ��ͻ�����д�����ݿ� */		
		xWriteCH378Data( ENDP_INDEX_3 );		   					/* �˵����� */
		len = CMD_LENGTH_DEV2HOST;
		xWriteCH378Data( ( UINT8 )len );
		xWriteCH378Data( ( UINT8 )( len >> 8 ) );
	//	mDelayuS( 1 ); 												/* ���ڸ���MCU,���ڴ˴��ʵ�������ʱ */							
		l = len;
		if( len )
		{
			do
			{		
				xWriteCH378Data( *pbuf++ );							/* ѭ��д�����ݿ� */
			}while( --l );		
		}
		
		/* ͨ����Ӧ���ϴ��˵�ش����ݰ� */
		
		// Output process********************
		
		switch(Out_Packet[0])		// this is the token to identify the data form
		{
		case CTW_PID:
				// SP setting

			for (i=0;i<6;i++)
			{
				//?/DesCur[i] = (Out_Packet[1+2*i+1]<<8)|(Out_Packet[1+2*i]);			// initial data buffer							
			}
			break;
		case CTW_SET_POS:
			for(i=0;i<6;i++)
			{
									u32Temp =(Out_Packet[2*i+2]<<8)|Out_Packet[2*i+1];
				//?/Write_7166_PR(u32Temp,i+ENC_STA); // ENC_STA +5 route
			}
			break;
		default:
			break;
		}	
	// switch Out_packet[0]
// prepare In_packet data:
		In_Packet[0] = DEV_POS_STA;
		for (i=0;i<6;i++)
		{
			//?/u32Temp = Read_7166_OL(i+ENC_STA);
			In_Packet[2*i+1] = 0xff & u32Temp;
			In_Packet[2*i+2] = 0xff &(u32Temp>>8);					
		}		

		//?/IState[0] = ~I1;
		//?/IState[1] = ~I2;
		//?/IState[2] = ~I3;		
		In_Packet[13]=IState[0]|(IState[1]<<1)|(IState[2]<<2);
		break;

	case USB_INT_BUS_SUSP:											/* USB���߹����ж� */
//		printf("BUS_SUSP\n");	
	break;

	case USB_INT_BUS_RESET:											/* USB���߸�λ�ж� */
//		printf("BUS_RESET\n");	
	break;

	default:
	break;
	}	
			
}

