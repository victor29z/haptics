#include "ch378.h"
#include "includes.h"
/* 附加的USB操作状态定义 */
#define	ERR_USB_UNKNOWN			   0xFA							 /* 未知错误,不应该发生的情况,需检查硬件或者程序错误 */

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
	
	/* 检测CH378连接是否正常 */	
//	CH378_PORT_INIT( );  										 				/* 接口硬件初始化 */
	xWriteCH378Cmd( CMD11_CHECK_EXIST );  						 				/* 测试单片机与CH378之间的通讯接口 */
	xWriteCH378Data( 0x65 );
	res = xReadCH378Data( );
//	printf( "res1: %02X\n", (UINT16)res );
//	xEndCH378Cmd( );
	if( res != 0x9A ) 
	{
		/* 通讯接口不正常,可能原因有:接口连接异常,其它设备影响(片选不唯一),串口波特率,一直在复位,晶振不工作 */
		return( ERR_USB_UNKNOWN );  
	}	

#if  1
	/* 设置VID、PID */
	xWriteCH378Cmd( CMD40_SET_USB_ID );  						 				/* 设备芯片VID、PID */
	xWriteCH378Data( 0x43 );									 			
	xWriteCH378Data( 0x48 );									 			
	xWriteCH378Data( 0x55 );									 			
	xWriteCH378Data( 0x37 );									 			
#endif
									
#if  0
	/* 设置USB 速度 */
	xWriteCH378Cmd( CMD10_SET_USB_SPEED );  						 			/* 设备芯片USB速度 */
	xWriteCH378Data( 0x00 );													/* 全速模式 */									 												 			
	//xEndCH378Cmd( );
#endif

	/* 设置CH378工作模式 */
	xWriteCH378Cmd( CMD11_SET_USB_MODE );  						 				/* 设备USB工作模式 */
	xWriteCH378Data( 0x02 );									 				/* 设置内置固件模式 */

	/* 等待模式设置完毕,对于操作SD卡大概需要10mS左右时间,对于操作USB设备大概需要35mS左右时间 */	
	
	OSTimeDlyHMSM(0,0,0,70);
	res = xReadCH378Data( );
//	printf( "res2: %02X\n", (UINT16)res );
	

#ifndef	CH378_INT_WIRE
	xWriteCH378Cmd( CMD20_SET_SDO_INT );  						 				/* 设置SPI的SDO引脚的中断方式 */
	xWriteCH378Data( 0x16 );
	xWriteCH378Data( 0x01 );  									 				/* SDO引脚在SCS片选无效时兼做中断请求输出 */
	
#endif
	if( res == CMD_RET_SUCCESS ) 
	{
		return( ERR_SUCCESS );
	}
	else 
	{
		return( ERR_USB_UNKNOWN );  							 				/* 设置模式错误 */
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
	xWriteCH378Cmd( CMD01_GET_STATUS );									/* 发送读取中断状态命令 */
	status = xReadCH378Data( );
//		xEndCH378Cmd( );
	switch( status )
	{
	case USB_INT_INDEX1_IN:											/* 索引1对应的端点上传成功中断 */
//		printf("INDEX1_IN\n");
	break;

	case USB_INT_INDEX2_OUT:										/* 索引2对应的端点下传成功中断 */
//		printf("INDEX2_OUT\n");

	break;

	case USB_INT_INDEX3_IN:											/* 索引3对应的端点上传成功中断 */
//		printf("INDEX3_IN\n");
	break;

	case USB_INT_INDEX4_OUT:										/* 索引4对应的端点下传成功中断,378 circle */
//		printf("INDEX4_OUT \n");
//		printf("INDEX4_OUT \n");
		// for performance check
		GPIO_ToggleBits(GPIOH,GPIO_Pin_11);

		pbuf = Out_Packet;
		xWriteCH378Cmd( CMD1x_RD_USB_DATA );						/* 从指定USB中断的端点缓冲区读取数据块,并释放缓冲区 */		
		xWriteCH378Data( ENDP_INDEX_4 );		   					/* 端点索引 */
		len = xReadCH378Data( );
		len += ( ( u16 )xReadCH378Data( ) << 8 );				/* 读取2个字节数据长度 */
//		printf( "len2: %02X\n", (UINT16)len );
		l = len;
		if( len )													/* 循环读取数据块 */
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
		xWriteCH378Cmd( CMD30_WR_USB_DATA );					 	/* 向指定USB端点的发送缓冲区写入数据块 */		
		xWriteCH378Data( ENDP_INDEX_3 );		   					/* 端点索引 */
		len = CMD_LENGTH_DEV2HOST;
		xWriteCH378Data( ( UINT8 )len );
		xWriteCH378Data( ( UINT8 )( len >> 8 ) );
	//	mDelayuS( 1 ); 												/* 对于高速MCU,可在此处适当增加延时 */							
		l = len;
		if( len )
		{
			do
			{		
				xWriteCH378Data( *pbuf++ );							/* 循环写入数据块 */
			}while( --l );		
		}
		
		/* 通过对应的上传端点回传数据包 */
		
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

	case USB_INT_BUS_SUSP:											/* USB总线挂起中断 */
//		printf("BUS_SUSP\n");	
	break;

	case USB_INT_BUS_RESET:											/* USB总线复位中断 */
//		printf("BUS_RESET\n");	
	break;

	default:
	break;
	}	
			
}

