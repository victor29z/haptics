/*
*********************************************************************************************************
*                                     MICIRUM BOARD SUPPORT PACKAGE
*
*                             (c) Copyright 2014; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                        BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                         STM3240G-EVAL
*                                        Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define   BSP_MODULE
#include  <bsp.h>
//#include  <bsp_os.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define  BSP_BIT_RCC_PLLCFGR_PLLM               8u
#define  BSP_BIT_RCC_PLLCFGR_PLLN              336u
#define  BSP_BIT_RCC_PLLCFGR_PLLP                2u
#define  BSP_BIT_RCC_PLLCFGR_PLLQ                7u



/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
const unsigned int encCalib[6] = {61645,61645,52727,0,0,0}; 	//	initial position for encoder
//const unsigned int encCalib[6] = {-3891,-3891,-12809,0,0,0};		//	initial position for encoder
//const unsigned int encCalib[6] = {-7782,-7782,-25618,0,0,0};		//	initial position for encoder
//const unsigned int encCalib[6] = {32768,32768,32768,0,0,0};
const unsigned int EncMid[6] = {32768,32768,32768,32768,32768,32768};
uint16_t enc_value_ext[5];



/*
*********************************************************************************************************
*                                             REGISTERS
*********************************************************************************************************
*/

#define  BSP_REG_DEM_CR                       (*(CPU_REG32 *)0xE000EDFC)
#define  BSP_REG_DWT_CR                       (*(CPU_REG32 *)0xE0001000)
#define  BSP_REG_DWT_CYCCNT                   (*(CPU_REG32 *)0xE0001004)
#define  BSP_REG_DBGMCU_CR                    (*(CPU_REG32 *)0xE0042004)

/*
*********************************************************************************************************
*                                            REGISTER BITS
*********************************************************************************************************
*/

#define  BSP_DBGMCU_CR_TRACE_IOEN_MASK                   0x10
#define  BSP_DBGMCU_CR_TRACE_MODE_ASYNC                  0x00
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_01                0x40
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_02                0x80
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_04                0xC0
#define  BSP_DBGMCU_CR_TRACE_MODE_MASK                   0xC0

#define  BSP_BIT_DEM_CR_TRCENA                    DEF_BIT_24

#define  BSP_BIT_DWT_CR_CYCCNTENA                 DEF_BIT_00

/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static void  CH378_Port_Init(void);
static void PWM_Config(void);
static void ENC_Init(void);
static void  BSP_MotorControl_Init(void);
static void BSP_UART_DMA_Init(void)  ;



/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*
*               (2) CPU instruction / data tracing requires the use of the following pins :
*                   (a) (1) Aysynchronous     :  PB[3]
*                       (2) Synchronous 1-bit :  PE[3:2]
*                       (3) Synchronous 2-bit :  PE[4:2]
*                       (4) Synchronous 4-bit :  PE[6:2]
*
*                   (c) The application may wish to adjust the trace bus width depending on I/O
*                       requirements.
*********************************************************************************************************
*/

void  BSP_Init (void)
{
//    BSP_IntInit();//set vector table
    
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);                                  /* HSE = 8MHz ext. crystal.                            */
    RCC_WaitForHSEStartUp();
    
    RCC_HCLKConfig(RCC_SYSCLK_Div1);                            /* HCLK    = AHBCLK  = PLL / AHBPRES(1) = 168MHz.       */
    RCC_PCLK2Config(RCC_HCLK_Div2);                             /* APB2CLK = AHBCLK  / APB2DIV(2)       = 84MHz.        */
    RCC_PCLK1Config(RCC_HCLK_Div4);                             /* APB1CLK = AHBCLK  / APB1DIV(4)       = 42MHz (max).  */
    
                                                                /* PLLCLK    = HSE * (PLLN / PLLM)      = 336MHz.       */
                                                                /* SYSCLK    = PLLCLK / PLLP            = 168MHz.       */
                                                                /* OTG_FSCLK = PLLCLK / PLLQ            =  48MHz.       */ 
    RCC_PLLConfig(RCC_PLLCFGR_PLLSRC_HSE,
                  BSP_BIT_RCC_PLLCFGR_PLLM,
                  BSP_BIT_RCC_PLLCFGR_PLLN, 
                  BSP_BIT_RCC_PLLCFGR_PLLP, 
                  BSP_BIT_RCC_PLLCFGR_PLLQ);
    
    RCC_PLLCmd(ENABLE);
    
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {       /* Wait for PLL to lock.                                */
        ;
    }
    
    FLASH_SetLatency(FLASH_Latency_5);                          /* 5 Flash wait states when HCLK > 120MHz.              */
    FLASH_PrefetchBufferCmd(ENABLE);
    
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);                  /* HCLK = SYSCLK = PLL = 168MHz.                        */
    while (RCC_GetSYSCLKSource() != RCC_CFGR_SWS_PLL) {
        ;
    }
	BSP_LED_KEY_Init();
	BSP_MotorControl_Init();
    CH378_Port_Init();
	PWM_Config();
	DISABLE_MOTOR();
    ENC_Init();
	BSP_UART_DMA_Init();
#ifdef TRACE_EN                                                 /* See project / compiler preprocessor options.         */
    BSP_CPU_REG_DBGMCU_CR |=  BSP_DBGMCU_CR_TRACE_IOEN_MASK;    /* Enable tracing (see Note #2).                        */
    BSP_CPU_REG_DBGMCU_CR &= ~BSP_DBGMCU_CR_TRACE_MODE_MASK;    /* Clr trace mode sel bits.                             */
    BSP_CPU_REG_DBGMCU_CR |=  BSP_DBGMCU_CR_TRACE_MODE_SYNC_04; /* Cfg trace mode to synch 4-bit.                       */
#endif
}

void systick_init(void) 
{ 
        RCC_ClocksTypeDef  rcc_clocks; 
        RCC_GetClocksFreq(&rcc_clocks);   //调用标准库函数，获取系统时钟。
        SysTick_Config(rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC); //初始化并使能SysTick
}

/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;


    RCC_GetClocksFreq(&rcc_clocks);
    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}




/*
*********************************************************************************************************
*                                           BSP_LED_Init()
*
* Description : Initialize any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    inialize ALL  LEDs
*                       1    inialize user LED1
*                       2    inialize user LED2
*                       3    inialize user LED3
*                       4    inialize user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_KEY_Init(void)
{
    GPIO_InitTypeDef  gpio_init;

    BSP_PeriphEn(BSP_PERIPH_ID_GPIOA); 
	BSP_PeriphEn(BSP_PERIPH_ID_GPIOC);
	BSP_PeriphEn(BSP_PERIPH_ID_GPIOE);

//leds init
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;
	
    gpio_init.GPIO_Pin   = LED1_PIN;
    GPIO_Init(LED1_PORT, &gpio_init);

	gpio_init.GPIO_Pin	 = LED2_PIN;
	GPIO_Init(LED2_PORT, &gpio_init);
	
    gpio_init.GPIO_Pin   = LED3_PIN;
    GPIO_Init(LED3_PORT, &gpio_init);
	
    gpio_init.GPIO_Pin   = LED4_PIN;
    GPIO_Init(LED4_PORT, &gpio_init);

	BSP_LED_Off(0);
	BSP_LED_Off(1);
	BSP_LED_Off(2);
	BSP_LED_Off(3);
// keys init
	gpio_init.GPIO_Mode  = GPIO_Mode_IN;
	
    gpio_init.GPIO_Pin   = KEY1_PIN;
    GPIO_Init(KEY1_PORT, &gpio_init);

	gpio_init.GPIO_Pin	 = KEY2_PIN;
	GPIO_Init(KEY2_PORT, &gpio_init);
	
    gpio_init.GPIO_Pin   = KEY3_PIN;
    GPIO_Init(KEY3_PORT, &gpio_init);
	
    gpio_init.GPIO_Pin   = KEY4_PIN;
    GPIO_Init(KEY4_PORT, &gpio_init);

	//interrupt line init
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource3);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStructure);

}


/*
*********************************************************************************************************
*                                             BSP_LED_On()
*
* Description : Turn ON any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns ON ALL  LEDs
*                       1    turns ON user LED1
*                       2    turns ON user LED2
*                       3    turns ON user LED3
*                       4    turns ON user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_On (CPU_INT08U  led)
{
    switch (led) {
        case 0u:
             GPIO_ResetBits(LED1_PORT, LED1_PIN);
             break;

        case 1u:
             GPIO_ResetBits(LED2_PORT, LED2_PIN);
             break;

        case 2u:
             GPIO_ResetBits(LED3_PORT, LED3_PIN);
             break;

        case 3u:
             GPIO_ResetBits(LED4_PORT, LED4_PIN);
             break;      

        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                              BSP_LED_Off()
*
* Description : Turn OFF any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    turns OFF ALL the LEDs
*                       1    turns OFF user LED1
*                       2    turns OFF user LED2
*                       3    turns OFF user LED3
*                       4    turns OFF user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Off (CPU_INT08U led)
{
     switch (led) {
        case 0u:
             GPIO_SetBits(LED1_PORT, LED1_PIN);
             break;

        case 1u:
             GPIO_SetBits(LED2_PORT, LED2_PIN);
             break;

        case 2u:
             GPIO_SetBits(LED3_PORT, LED3_PIN);
             break;

        case 3u:
             GPIO_SetBits(LED4_PORT, LED4_PIN);
             break;      

        default:
             break;
    }
}


/*
*********************************************************************************************************
*                                            BSP_LED_Toggle()
*
* Description : TOGGLE any or all the LEDs on the board.
*
* Argument(s) : led     The ID of the LED to control:
*
*                       0    TOGGLE ALL the LEDs
*                       1    TOGGLE user LED1
*                       2    TOGGLE user LED2
*                       3    TOGGLE user LED3
*                       4    TOGGLE user LED4
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_LED_Toggle (CPU_INT08U  led)
{
    switch (led) {
        case 0u:
             GPIO_ToggleBits(LED1_PORT, LED1_PIN);
             break;

        case 1u:
             GPIO_ToggleBits(LED2_PORT, LED2_PIN);
             break;

        case 2u:
             GPIO_ToggleBits(LED3_PORT, LED3_PIN);
             break;

        case 3u:
             GPIO_ToggleBits(LED4_PORT, LED4_PIN);
             break;      

        default:
             break;
    }
}


static void  CH378_Port_Init(void)
{
    GPIO_InitTypeDef  gpio_init;

    BSP_PeriphEn(BSP_PERIPH_ID_GPIOD);
	BSP_PeriphEn(BSP_PERIPH_ID_GPIOE);
	
	// data bus PD0-PD7
    gpio_init.GPIO_Pin   = 	   GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
							 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_OD;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(CH378_DAT_PORT, &gpio_init);
	CH378_WRITE_DB(0xff);
	// INT
	gpio_init.GPIO_Pin   = CH378_INT_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_IN;
//	gpio_init.GPIO_OType = GPIO_OType_OD;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(CH378_INT_PORT, &gpio_init);
	//GPIO_SetBits(CH378_INT_PORT,CH378_INT_PIN);
	// A0
	gpio_init.GPIO_Pin   = CH378_A0_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(CH378_A0_PORT, &gpio_init);
	CH378_SET_A0();
	// RD
	gpio_init.GPIO_Pin   = CH378_nRD_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(CH378_nRD_PORT, &gpio_init);
	CH378_SET_nRD();
	// WR
	gpio_init.GPIO_Pin   = CH378_nWR_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(CH378_nWR_PORT, &gpio_init);
	CH378_SET_nWR();
	// PCS
	gpio_init.GPIO_Pin   = CH378_nPCS_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(CH378_nPCS_PORT, &gpio_init);
	CH378_SET_nPCS();

	// RESET
	gpio_init.GPIO_Pin   = CH378_nRES_PIN;
	GPIO_Init(CH378_nRES_PORT, &gpio_init);
	CH378_SET_nRES();
	
	//interrupt line init
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	/* Connect EXTI Line1 to PG1 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);

	/* Configure EXTI Line1 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI Line1 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

static void SPI_Send_Byte(uint8_t dat){
	uint8_t i,j;
	for(i=0;i<8;i++){
		if(dat & 0x01)
			SET_SDI_HIGH();
		else
			SET_SDI_LOW();
		dat >>= 1;
		SET_SCK_HIGH();
		j=0xff;
		while(j--);
		SET_SCK_LOW();
		j=0xff;
		while(j--);
	}
}

static void  BSP_MotorControl_Init(void)
{
	GPIO_InitTypeDef  gpio_init;
	//SPI_InitTypeDef  SPI_InitStructure;
	BSP_PeriphEn(BSP_PERIPH_ID_GPIOA);
	BSP_PeriphEn(BSP_PERIPH_ID_GPIOB);
	BSP_PeriphEn(BSP_PERIPH_ID_GPIOD);
	BSP_PeriphEn(BSP_PERIPH_ID_GPIOE);

	

	/*!< Enable the SPI clock */
	//BSP_PeriphEn(BSP_PERIPH_ID_SPI2);

	
	/*!< SPI pins configuration *************************************************/

	


	// DIS
	gpio_init.GPIO_Pin   = M_DIS_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(M_DIS_PORT, &gpio_init);
	DISABLE_MOTOR();
/*	
	// MOSI
	gpio_init.GPIO_Pin   = M_SDI_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_DOWN;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(M_SDI_PORT, &gpio_init);

	// SCLK
	gpio_init.GPIO_Pin   = M_SCLK_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_DOWN;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(M_SCLK_PORT, &gpio_init);

	// MISO
	gpio_init.GPIO_Pin   = M_SDO_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_IN;
    gpio_init.GPIO_OType = GPIO_OType_OD;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_DOWN;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(M_SDO_PORT, &gpio_init);
*/
	// DIS
	gpio_init.GPIO_Pin   = M1_DIS_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(M1_DIS_PORT, &gpio_init);

	gpio_init.GPIO_Pin   = M2_DIS_PIN;
	GPIO_Init(M2_DIS_PORT, &gpio_init);

	gpio_init.GPIO_Pin   = M3_DIS_PIN;
	GPIO_Init(M3_DIS_PORT, &gpio_init);

	gpio_init.GPIO_Pin   = M4_DIS_PIN;
	GPIO_Init(M4_DIS_PORT, &gpio_init);

	gpio_init.GPIO_Pin   = M5_DIS_PIN;
	GPIO_Init(M5_DIS_PORT, &gpio_init);

	gpio_init.GPIO_Pin   = M6_DIS_PIN;
	GPIO_Init(M6_DIS_PORT, &gpio_init);

	gpio_init.GPIO_Pin   = M7_DIS_PIN;
	GPIO_Init(M7_DIS_PORT, &gpio_init);

	//SET_M_CSN_HIGH(1);

	gpio_init.GPIO_Pin   = M1_DIR_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(M1_DIR_PORT, &gpio_init);
	
	gpio_init.GPIO_Pin   = M2_DIR_PIN;
    GPIO_Init(M2_DIR_PORT, &gpio_init);
	gpio_init.GPIO_Pin   = M3_DIR_PIN;
    GPIO_Init(M3_DIR_PORT, &gpio_init);
	gpio_init.GPIO_Pin   = M4_DIR_PIN;
    GPIO_Init(M4_DIR_PORT, &gpio_init);
	gpio_init.GPIO_Pin   = M5_DIR_PIN;
    GPIO_Init(M5_DIR_PORT, &gpio_init);
	gpio_init.GPIO_Pin   = M6_DIR_PIN;
    GPIO_Init(M6_DIR_PORT, &gpio_init);
	gpio_init.GPIO_Pin   = M7_DIR_PIN;
    GPIO_Init(M7_DIR_PORT, &gpio_init);

	//SET_M_CSN_LOW(1);
	//SPI_Send_Byte(0x44);
	//SET_M_CSN_HIGH(1);

	SET_M_DIR_HIGH(1);


}

void PWM_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t TimerPeriod = 0;
	uint16_t Channel1Pulse = 0, Channel2Pulse = 0, Channel3Pulse = 0, Channel4Pulse = 0;
	//pin config
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA  , ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_TIM1);
    
	//   timer config 17.57khz
	TimerPeriod = (SystemCoreClock / 17570 ) - 1;
	/* Compute CCR1 value to generate a duty cycle at 50% for channel 1 and 1N */
	Channel1Pulse = (uint16_t) (((uint32_t) 0 * (TimerPeriod - 1)) / 10);
	/* Compute CCR2 value to generate a duty cycle at 37.5%  for channel 2 and 2N */
	Channel2Pulse = (uint16_t) (((uint32_t) 0 * (TimerPeriod - 1)) / 1000);
	/* Compute CCR3 value to generate a duty cycle at 25%  for channel 3 and 3N */
	Channel3Pulse = (uint16_t) (((uint32_t) 0 * (TimerPeriod - 1)) / 100);
	/* Compute CCR4 value to generate a duty cycle at 12.5%  for channel 4 */
	Channel4Pulse = (uint16_t) (((uint32_t) 0 * (TimerPeriod- 1)) / 1000);

	/* TIM1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = Channel4Pulse;
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);

	/* TIM1 counter enable */
	TIM_Cmd(TIM1, ENABLE);

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
////////////////////////////////for TIM8 init//////////////////////
//pin config
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC  , ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);

    
	/* TIM1 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8 , ENABLE);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	TIM_OC1Init(TIM8, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = Channel3Pulse;
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);

	/* TIM1 counter enable */
	TIM_Cmd(TIM8, ENABLE);

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM8, ENABLE);

	
}

void ENC_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
	TIM_ICInitTypeDef TIM_ICInitStructure;  
	GPIO_InitTypeDef GPIO_InitStructure;
	//NVIC_InitTypeDef NVIC_InitStructure;

// ENC1 init
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//GPIO_StructInit(&GPIO_InitStructure);
	/* Configure PA.06,07 as encoder input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM3);

	/* Enable the TIM3 Update Interrupt */
	//  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//  NVIC_Init(&NVIC_InitStructure);

	/* Timer configuration in Encoder mode */ 
	//TIM_DeInit(TIM3);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // No prescaling 
	TIM_TimeBaseStructure.TIM_Period = 60000; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	// Clear all pending interrupts
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM3, ENABLE); 

// ENC2 init
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	//GPIO_StructInit(&GPIO_InitStructure);
	/* Configure PA.06,07 as encoder input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5);

	/* Enable the TIM3 Update Interrupt */
	//  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//  NVIC_Init(&NVIC_InitStructure);

	/* Timer configuration in Encoder mode */ 
	//TIM_DeInit(TIM3);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // No prescaling 
	TIM_TimeBaseStructure.TIM_Period = 60000; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TIM5, &TIM_ICInitStructure);

	// Clear all pending interrupts
	TIM_ClearFlag(TIM5, TIM_FLAG_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM5, ENABLE); 

// ENC3 init
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	//GPIO_StructInit(&GPIO_InitStructure);
	/* Configure PA.06,07 as encoder input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);


	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4);

	/* Enable the TIM3 Update Interrupt */
	//  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//  NVIC_Init(&NVIC_InitStructure);

	/* Timer configuration in Encoder mode */ 
	//TIM_DeInit(TIM3);
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

	TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // No prescaling 
	TIM_TimeBaseStructure.TIM_Period = 60000; 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);

	// Clear all pending interrupts
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM4, ENABLE); 


}
#define EXTCMD_HEAD		0xaa
#define EXTCMD_SET_ENC1	0xe0
#define EXTCMD_SET_ENC2	0xe1
#define EXTCMD_SET_ENC3	0xe2
#define EXTCMD_SET_ENC4	0xe3



void SetEncoder(uint32_t dat, uint8_t n){
	switch(n){
	case 0:
		ENC1->CNT = dat & 0xffff;
	break;

	case 1:
		ENC2->CNT = dat & 0xffff;
	break;

	case 2:
		ENC3->CNT = dat & 0xffff;
	break;
	case 3:
		USART_SendData(USART3,EXTCMD_HEAD);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3,EXTCMD_SET_ENC1);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3,dat&0xff);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3,(dat>>8)&0xff);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		
	break;
	case 4:
		USART_SendData(USART3,EXTCMD_HEAD);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3,EXTCMD_SET_ENC2);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3,dat&0xff);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3,(dat>>8)&0xff);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		
	break;
	case 5:
		USART_SendData(USART3,EXTCMD_HEAD);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3,EXTCMD_SET_ENC3);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3,dat&0xff);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		USART_SendData(USART3,(dat>>8)&0xff);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
		
	break;
	
			
	}
}
uint32_t enc_value[6];
uint32_t GetEncoder(uint8_t n){
	//for monitor encoder value
	enc_value[0] = ENC1->CNT ;
	enc_value[1] = ENC2->CNT ;
	enc_value[2] = ENC3->CNT ;
	switch(n){
	case 0:
		
		return enc_value[0];
	break;

	case 1:
		return enc_value[1];
	break;

	case 2:
		return enc_value[2];
	break;
	case 3:
		return enc_value_ext[3];
	break;
	case 4:
		return enc_value_ext[1];
	break;
	case 5:
		return enc_value_ext[2];
	break;
			
	}
}

void CalEncoder(void){
	uint8_t i;
	for(i=0;i<6;i++)
		SetEncoder(EncMid[i],i);
}

uint8_t GetKeys(void){
	uint8_t temp = 0;
	temp = 	GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN) | 
			GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN)<<1 |
			GPIO_ReadInputDataBit(KEY3_PORT,KEY3_PIN)<<2 | 
			GPIO_ReadInputDataBit(KEY4_PORT,KEY4_PIN)<<3 |
			(enc_value_ext[0]&0x0003)<<4;
}



static void BSP_UART_DMA_Init(void)  
{  
    //定义中断结构体  
    NVIC_InitTypeDef NVIC_InitStructure ;  
    //定义IO初始化结构体  
    GPIO_InitTypeDef GPIO_InitStructure;  
    //定义串口结构体    
    USART_InitTypeDef USART_InitStructure;  
    //定义DMA结构体  
    DMA_InitTypeDef DMA_InitStructure;  
  
    //打开串口对应的外设时钟    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  
       
    //串口发DMA配置    
    //启动DMA时钟  
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  
    //DMA发送中断设置  
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream1_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
    NVIC_Init(&NVIC_InitStructure);  
    //DMA通道配置  
    DMA_DeInit(DMA1_Stream1);  
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;   
    //外设地址  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART3->DR);  
    //内存地址  
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)enc_value_ext;  
    //dma传输方向  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  
    //设置DMA在传输时缓冲区的长度  
    DMA_InitStructure.DMA_BufferSize = 10;  
    //设置DMA的外设递增模式，一个外设  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    //设置DMA的内存递增模式  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //外设数据字长  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //内存数据字长  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
    //设置DMA的传输模式  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  
    //设置DMA的优先级别  
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
      
    //指定如果FIFO模式或直接模式将用于指定的流 ： 不使能FIFO模式    
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;      
    //指定了FIFO阈值水平  
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;          
    //指定的Burst转移配置内存传输   
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;         
    //指定的Burst转移配置外围转移 */    
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;   
  
    //配置DMA1的通道           
    DMA_Init(DMA1_Stream1, &DMA_InitStructure);    
    //使能中断  
    DMA_ITConfig(DMA1_Stream1,DMA_IT_TC,ENABLE);     
  
/*
	//串口收DMA配置    
    //启动DMA时钟  
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  
    //DMA通道配置  
    DMA_DeInit(DMA1_Stream5);  
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
    //外设地址  
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);  
    //内存地址  
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Rx_Buf_Gsm;  
    //dma传输方向  
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  
    //设置DMA在传输时缓冲区的长度  
    DMA_InitStructure.DMA_BufferSize = RX_LEN_GSM;  
    //设置DMA的外设递增模式，一个外设  
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
    //设置DMA的内存递增模式  
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
    //外设数据字长  
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
    //内存数据字长  
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
    //设置DMA的传输模式  
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
    //设置DMA的优先级别  
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
      
    //指定如果FIFO模式或直接模式将用于指定的流 ： 不使能FIFO模式    
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;      
    //指定了FIFO阈值水平  
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;          
    //指定的Burst转移配置内存传输   
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;         
    //指定的Burst转移配置外围转移 
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;   
      
    //配置DMA1的通道           
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);    
    //使能通道  
    DMA_Cmd(DMA1_Stream5,ENABLE);  
*/      
    //初始化串口参数    
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;    
    USART_InitStructure.USART_StopBits = USART_StopBits_1;    
    USART_InitStructure.USART_Parity = USART_Parity_No;    
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      
    USART_InitStructure.USART_BaudRate = 115200;   
    //初始化串口   
    USART_Init(USART3,&USART_InitStructure);    
      
    
  
    //配置中断    
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;               //通道设置为串口中断    
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;       //中断占先等级  
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;              //中断响应优先级   
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //打开中断    
    NVIC_Init(&NVIC_InitStructure);     
          
	//采用DMA方式接收  
    USART_DMACmd(USART3,USART_DMAReq_Rx,ENABLE);  
    USART_DMACmd(USART3,USART_DMAReq_Tx,DISABLE);      
  
    //中断配置  
    USART_ITConfig(USART3,USART_IT_TC,DISABLE);  
    USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);  
    USART_ITConfig(USART3,USART_IT_TXE,DISABLE); 
	USART_ITConfig(USART3, USART_IT_PE, ENABLE);    //开启PE错误接收中断Bit 8PEIE: PE interrupt enable
  	//CR2 开启ERR中断
  	USART_ITConfig(USART3, USART_IT_ERR, ENABLE);
    //USART_ITConfig(USART3,USART_IT_IDLE,ENABLE);    
    //启动串口    
    USART_Cmd(USART3, ENABLE);      
  
    //设置IO口时钟        
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);   
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);    
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3);  
  
    //管脚模式:输出口  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      
    //类型:推挽模式  
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    
    //上拉下拉设置  
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      
    //IO口速度  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
    //管脚指定  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;  
    //初始化  
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
      
    //管脚模式:输入口  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;      
    //上拉下拉设置  
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;      
    //管脚指定  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  
    //初始化  
    GPIO_Init(GPIOD, &GPIO_InitStructure); 
	DMA_Cmd(DMA1_Stream1,DISABLE);  
}

/*
*********************************************************************************************************
*                                          CPU_TS_TmrInit()
*
* Description : Initialize & start CPU timestamp timer.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : CPU_TS_Init().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but MUST NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrInit() is an application/BSP function that MUST be defined by the developer 
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR' 
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater 
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR' 
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be 
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets 
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple 
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the 
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                   (c) When applicable, timer period SHOULD be less than the typical measured time 
*                       but MUST be less than the maximum measured time; otherwise, timer resolution 
*                       inadequate to measure desired times.
*
*                   See also 'CPU_TS_TmrRd()  Note #2'.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void  CPU_TS_TmrInit (void)
{
    CPU_INT32U  fclk_freq;


    fclk_freq = BSP_CPU_ClkFreq();

    BSP_REG_DEM_CR     |= (CPU_INT32U)BSP_BIT_DEM_CR_TRCENA;    /* Enable Cortex-M4's DWT CYCCNT reg.                   */
    BSP_REG_DWT_CYCCNT  = (CPU_INT32U)0u;
    BSP_REG_DWT_CR     |= (CPU_INT32U)BSP_BIT_DWT_CR_CYCCNTENA;

    CPU_TS_TmrFreqSet((CPU_TS_TMR_FREQ)fclk_freq);
}
#endif


/*
*********************************************************************************************************
*                                           CPU_TS_TmrRd()
*
* Description : Get current CPU timestamp timer count value.
*
* Argument(s) : none.
*
* Return(s)   : Timestamp timer count (see Notes #2a & #2b).
*
* Caller(s)   : CPU_TS_Init(),
*               CPU_TS_Get32(),
*               CPU_TS_Get64(),
*               CPU_IntDisMeasStart(),
*               CPU_IntDisMeasStop().
*
*               This function is an INTERNAL CPU module function & MUST be implemented by application/
*               BSP function(s) [see Note #1] but SHOULD NOT be called by application function(s).
*
* Note(s)     : (1) CPU_TS_TmrRd() is an application/BSP function that MUST be defined by the developer 
*                   if either of the following CPU features is enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR' 
*                       data type.
*
*                       (1) If timer has more bits, truncate timer values' higher-order bits greater 
*                           than the configured 'CPU_TS_TMR' timestamp timer data type word size.
*
*                       (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR' 
*                           timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be 
*                           configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
*
*                           In other words, if timer size is not a binary-multiple of 8-bit octets 
*                           (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple 
*                           octet word size SHOULD be configured (e.g. to 16-bits).  However, the 
*                           minimum supported word size for CPU timestamp timers is 8-bits.
*
*                       See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
*                              & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
*
*                   (b) Timer SHOULD be an 'up'  counter whose values increase with each time count.
*
*                       (1) If timer is a 'down' counter whose values decrease with each time count,
*                           then the returned timer value MUST be ones-complemented.
*
*                   (c) (1) When applicable, the amount of time measured by CPU timestamps is 
*                           calculated by either of the following equations :
*
*                           (A) Time measured  =  Number timer counts  *  Timer period
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured 
*                                       Timer period            Timer's period in some units of 
*                                                                   (fractional) seconds
*                                       Time measured           Amount of time measured, in same 
*                                                                   units of (fractional) seconds 
*                                                                   as the Timer period
*
*                                                  Number timer counts
*                           (B) Time measured  =  ---------------------
*                                                    Timer frequency
*
*                                   where
*
*                                       Number timer counts     Number of timer counts measured
*                                       Timer frequency         Timer's frequency in some units 
*                                                                   of counts per second
*                                       Time measured           Amount of time measured, in seconds
*
*                       (2) Timer period SHOULD be less than the typical measured time but MUST be less 
*                           than the maximum measured time; otherwise, timer resolution inadequate to 
*                           measure desired times.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR  CPU_TS_TmrRd (void)
{
    CPU_TS_TMR  ts_tmr_cnts;

                                                                
    ts_tmr_cnts = (CPU_TS_TMR)BSP_REG_DWT_CYCCNT;

    return (ts_tmr_cnts);
}
#endif


/*
*********************************************************************************************************
*                                         CPU_TSxx_to_uSec()
*
* Description : Convert a 32-/64-bit CPU timestamp from timer counts to microseconds.
*
* Argument(s) : ts_cnts   CPU timestamp (in timestamp timer counts [see Note #2aA]).
*
* Return(s)   : Converted CPU timestamp (in microseconds           [see Note #2aD]).
*
* Caller(s)   : Application.
*
*               This function is an (optional) CPU module application programming interface (API) 
*               function which MAY be implemented by application/BSP function(s) [see Note #1] & 
*               MAY be called by application function(s).
*
* Note(s)     : (1) CPU_TS32_to_uSec()/CPU_TS64_to_uSec() are application/BSP functions that MAY be 
*                   optionally defined by the developer when either of the following CPU features is 
*                   enabled :
*
*                   (a) CPU timestamps
*                   (b) CPU interrupts disabled time measurements
*
*                   See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
*                     & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
*
*               (2) (a) The amount of time measured by CPU timestamps is calculated by either of 
*                       the following equations :
*
*                                                                        10^6 microseconds
*                       (1) Time measured  =   Number timer counts   *  -------------------  *  Timer period
*                                                                            1 second
*
*                                              Number timer counts       10^6 microseconds
*                       (2) Time measured  =  ---------------------  *  -------------------
*                                                Timer frequency             1 second
*
*                               where
*
*                                   (A) Number timer counts     Number of timer counts measured
*                                   (B) Timer frequency         Timer's frequency in some units 
*                                                                   of counts per second
*                                   (C) Timer period            Timer's period in some units of 
*                                                                   (fractional)  seconds
*                                   (D) Time measured           Amount of time measured, 
*                                                                   in microseconds
*
*                   (b) Timer period SHOULD be less than the typical measured time but MUST be less 
*                       than the maximum measured time; otherwise, timer resolution inadequate to 
*                       measure desired times.
*
*                   (c) Specific implementations may convert any number of CPU_TS32 or CPU_TS64 bits 
*                       -- up to 32 or 64, respectively -- into microseconds.
*********************************************************************************************************
*/

#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS32_to_uSec (CPU_TS32  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;

    
    fclk_freq = BSP_CPU_ClkFreq();    
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif


#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_INT64U  CPU_TS64_to_uSec (CPU_TS64  ts_cnts)
{
    CPU_INT64U  ts_us;
    CPU_INT64U  fclk_freq;
    

    fclk_freq = BSP_CPU_ClkFreq();    
    ts_us     = ts_cnts / (fclk_freq / DEF_TIME_NBR_uS_PER_SEC);

    return (ts_us);
}
#endif
