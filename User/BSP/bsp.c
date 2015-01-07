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


#define  BSP_GPIOG_LED1                        DEF_BIT_06
#define  BSP_GPIOG_LED2                        DEF_BIT_08
#define  BSP_GPIOI_LED3                        DEF_BIT_09
#define  BSP_GPIOC_LED4                        DEF_BIT_07

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
	BSP_LED_Init();
	BSP_MotorControl_Init();
    CH378_Port_Init();
	PWM_Config();
	ENABLE_MOTOR();
    ENC_Init();
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

void  BSP_LED_Init(void)
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
             GPIO_SetBits(GPIOG, (BSP_GPIOG_LED1 | BSP_GPIOG_LED2));
             GPIO_SetBits(GPIOI, BSP_GPIOI_LED3);
             GPIO_SetBits(GPIOC, BSP_GPIOC_LED4);
             break;

        case 1u:
             GPIO_SetBits(GPIOG, BSP_GPIOG_LED1);
             break;

        case 2u:
             GPIO_SetBits(GPIOG, BSP_GPIOG_LED2);
             break;

        case 3u:
             GPIO_SetBits(GPIOI, BSP_GPIOI_LED3);
             break;
             
        case 4u:
             GPIO_SetBits(GPIOC, BSP_GPIOC_LED4);
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
             GPIO_ResetBits(GPIOH, (GPIO_Pin_2 | GPIO_Pin_3));
             GPIO_ResetBits(GPIOI, (GPIO_Pin_8 | GPIO_Pin_10));
             break;

        case 1u:
             GPIO_ResetBits(GPIOH, GPIO_Pin_2);
             break;

        case 2u:
             GPIO_ResetBits(GPIOH, GPIO_Pin_3);
             break;

        case 3u:
             GPIO_ResetBits(GPIOI, GPIO_Pin_8);
             break;
             
        case 4u:
             GPIO_ResetBits(GPIOI, GPIO_Pin_10);
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
    CPU_INT32U  pins;


    switch (led) {
        case 0u:
             pins  =  GPIO_ReadOutputData(GPIOH);
             pins ^= (GPIO_Pin_2 | GPIO_Pin_3);
             GPIO_SetBits(  GPIOH,   pins  & (GPIO_Pin_2 | GPIO_Pin_3));
             GPIO_ResetBits(GPIOH, (~pins) & (GPIO_Pin_2 | GPIO_Pin_3));

			
	         pins  =  GPIO_ReadOutputData(GPIOI);
			 pins ^= (GPIO_Pin_8 | GPIO_Pin_10);
			 GPIO_SetBits(	GPIOI,	 pins  & (GPIO_Pin_8 | GPIO_Pin_10));
			 GPIO_ResetBits(GPIOI, (~pins) & (GPIO_Pin_8 | GPIO_Pin_10));
             
             break;

        case 1u:
             pins  = GPIO_ReadOutputData(GPIOH);
             pins ^= GPIO_Pin_2;
             GPIO_SetBits(  GPIOH,   pins  & GPIO_Pin_2);
             GPIO_ResetBits(GPIOH, (~pins) & GPIO_Pin_2);
             break;
             
        case 2u:
             pins  = GPIO_ReadOutputData(GPIOH);
             pins ^= GPIO_Pin_3;
             GPIO_SetBits(  GPIOH,   pins  & GPIO_Pin_3);
             GPIO_ResetBits(GPIOH, (~pins) & GPIO_Pin_3);
             break;
             
        case 3u:
             pins  =  GPIO_ReadOutputData(GPIOI);
             pins ^= GPIO_Pin_8;
             GPIO_SetBits(  GPIOI,   pins  & GPIO_Pin_8);
             GPIO_ResetBits(GPIOI, (~pins) & GPIO_Pin_8);
             break;
             
        case 4u:
             pins  =  GPIO_ReadOutputData(GPIOI);
             pins ^= GPIO_Pin_10;
             GPIO_SetBits(  GPIOI,   pins  & GPIO_Pin_10);
             GPIO_ResetBits(GPIOI, (~pins) & GPIO_Pin_10);
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
	SPI_InitTypeDef  SPI_InitStructure;
	BSP_PeriphEn(BSP_PERIPH_ID_GPIOA);
	BSP_PeriphEn(BSP_PERIPH_ID_GPIOB);
	BSP_PeriphEn(BSP_PERIPH_ID_GPIOD);
	BSP_PeriphEn(BSP_PERIPH_ID_GPIOE);

	

	/*!< Enable the SPI clock */
	BSP_PeriphEn(BSP_PERIPH_ID_SPI2);

	
	/*!< SPI pins configuration *************************************************/

	


	// DIS
	gpio_init.GPIO_Pin   = M_DIS_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(M_DIS_PORT, &gpio_init);
	DISABLE_MOTOR();
	
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

	// CS1
	gpio_init.GPIO_Pin   = M1_CSN_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(M1_CSN_PORT, &gpio_init);

	SET_M_CSN_HIGH(1);

	gpio_init.GPIO_Pin   = M1_DIR_PIN;
    gpio_init.GPIO_Mode  = GPIO_Mode_OUT;
    gpio_init.GPIO_OType = GPIO_OType_PP;
    gpio_init.GPIO_PuPd  = GPIO_PuPd_UP;
    gpio_init.GPIO_Speed = GPIO_Speed_100MHz;

    GPIO_Init(M1_DIR_PORT, &gpio_init);
	SET_M_CSN_LOW(1);
	SPI_Send_Byte(0x44);
	SET_M_CSN_HIGH(1);

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
	NVIC_InitTypeDef NVIC_InitStructure;

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

void SetEncoder(uint32_t dat, uint8_t n){
	switch(n){
	case 0:
		ENC1->CNT = dat;
	break;

	case 1:
		ENC2->CNT = dat;
	break;

	case 2:
		ENC3->CNT = dat;
	break;
			
	}
}
unsigned int enc_value[6];
uint32_t GetEncoder(uint8_t n){
	//for monitor encoder value
	//enc_value[0] = ENC1->CNT;
	//enc_value[1] = ENC2->CNT;
	//enc_value[2] = ENC3->CNT;
	switch(n){
	case 0:
		
		return ENC1->CNT;
	break;

	case 1:
		return ENC2->CNT;
	break;

	case 2:
		return ENC3->CNT;
	break;
			
	}
}

uint8_t GetKeys(void){
	uint8_t temp;
	temp = 	GPIO_ReadInputDataBit(KEY1_PORT,KEY1_PIN) | 
			GPIO_ReadInputDataBit(KEY2_PORT,KEY2_PIN)<<1 |
			GPIO_ReadInputDataBit(KEY3_PORT,KEY3_PIN)<<2 | 
			GPIO_ReadInputDataBit(KEY4_PORT,KEY4_PIN)<<3;
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
