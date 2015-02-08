/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2014; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM3240G-EVAL
*                                         Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include  <includes.h>
#include "console.h"
#include "uart.h"
#include "stdio.h"
#include "ch378.h"

/*
*********************************************************************************************************
*                                        GLOBAL VARIABLES
*********************************************************************************************************
*/

short int DesCur[6]= {0};//目标电流
OS_EVENT* Sem_NewIOMSG;
OS_EVENT* Sem_KEY1_EVT;
OS_EVENT* Sem_KEY2_EVT;


unsigned char USBActive=0;
unsigned char ForceEnable=0;



/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/

#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
/**
* @brief
* @note
* @param
* @retval
*/
PUTCHAR_PROTOTYPE
{
/* Place your implementation of fputc here */
/* e.g. write a character to the USART */
	USART_SendData(CONSOLE, (u8) ch);
	/* Loop until the end of transmission */
	while(USART_GetFlagStatus(CONSOLE, USART_FLAG_TC) == RESET);
	return ch;
}


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* --------------- APPLICATION GLOBALS ---------------- */
static  OS_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK  UMon_task_stk[LARGE_TASK_STK_SIZE ];  
static  OS_STK  Console_task_stk[LARGE_TASK_STK_SIZE ]; 
static  OS_STK  Motor_task_stk[LARGE_TASK_STK_SIZE ];


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart          (void  *p_arg);
static  void  MotorControl_Task (void *p_arg);


/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/

int main(void)
{
#if (OS_TASK_NAME_EN > 0)
    CPU_INT08U  err;
#endif
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */
    Mem_Init();                                                 /* Initialize Memory Mmanagment Module                  */
    //Math_Init();                                                /* Initialize Mathematical Module                       */

    //BSP_IntDisAll();                                            /* Disable all Interrupts.                              */

    OSInit();                                                   /* Init uC/OS-II.                                       */

    OSTaskCreateExt((void (*)(void *)) AppTaskStart,            /* Create the start task                                */
                    (void           *) 0,
                    (OS_STK         *)&AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE - 1],
                    (INT8U           ) APP_CFG_TASK_START_PRIO,
                    (INT16U          ) APP_CFG_TASK_START_PRIO,
                    (OS_STK         *)&AppTaskStartStk[0],
                    (INT32U          ) APP_CFG_TASK_START_STK_SIZE,
                    (void           *) 0,
                    (INT16U          )(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

#if (OS_TASK_NAME_EN > 0)
    OSTaskNameSet(APP_CFG_TASK_START_PRIO, "Start Task", &err);
#endif
    
    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-III). */

    while (DEF_ON) {                                            /* Should Never Get Here.                               */
    };
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
   (void)p_arg;
	uint8_t i;
	uint8_t err;
	Sem_KEY1_EVT = OSSemCreate(0);
	Sem_KEY2_EVT = OSSemCreate(0);
    BSP_Init();                                                 /* Initialize BSP functions                             */
	//BSP_LED_Init();                                             /* Init LEDs.                                           */
	systick_init();                                            /* Start Tick Initialization                            */
    OSTaskCreateExt(UartMonitor_task,
						(void *)0,								  
						&UMon_task_stk[LARGE_TASK_STK_SIZE - 1], 
						UMON_TASK_PRIO, 					
						UMON_TASK_PRIO,
						&UMon_task_stk[0],						  
						LARGE_TASK_STK_SIZE,
						(void *)0,		
						OS_TASK_OPT_NONE);
	OSTaskCreateExt(MotorControl_Task,
						(void *)0,								  
						&Motor_task_stk[LARGE_TASK_STK_SIZE - 1], 
						MCTRL_TASK_PRIO, 					
						MCTRL_TASK_PRIO,
						&Motor_task_stk[0],						  
						LARGE_TASK_STK_SIZE,
						(void *)0,		
						OS_TASK_OPT_NONE);
	
/*
	OSTaskCreateExt(ConsoleService_task,
						(void *)0,								
						&Console_task_stk[LARGE_TASK_STK_SIZE - 1],
						CONSOLE_TASK_PRIO,						
						CONSOLE_TASK_PRIO,
						&Console_task_stk[0], 					
						LARGE_TASK_STK_SIZE,
						(void *)0,								
						OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	*/

	while(mInitCH378Device() != ERR_SUCCESS)
		OSTimeDly(20);
	i = 0;
	while(!(~(GetKeys()) & KEY1_MASK)){
		if(i < 30) i++;
		else{
			i = 0;
			BSP_LED_Toggle(0);	
		}
		OSTimeDly(1);
	}
	CalEncoder();
	ENABLE_MOTOR();
	ForceEnable = 1;
	BSP_LED_On(0);
	BSP_LED_On(1);
	while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
		
		OSSemPend(Sem_KEY2_EVT,0,&err);       
		//if((~(GetKeys()) & KEY1_MASK))
		//	CalEncoder();
		//if((~(GetKeys()) & KEY2_MASK))	{
			if(ForceEnable == 0){
				ForceEnable = 1;
				ENABLE_MOTOR();
				BSP_LED_On(1);
			}
			else{
				ForceEnable = 0;
				DISABLE_MOTOR();
				BSP_LED_Off(1);
			}
		//}
		
		
              
    }
}

#define SET_MOTOR1_PWM(n)	TIM_SetCompare4(TIM1,n)
#define SET_MOTOR2_PWM(n)	TIM_SetCompare3(TIM1,n)
#define SET_MOTOR3_PWM(n)	TIM_SetCompare2(TIM1,n)
#define SET_MOTOR4_PWM(n)	TIM_SetCompare1(TIM1,n)
#define SET_MOTOR5_PWM(n)	TIM_SetCompare3(TIM8,n)
#define SET_MOTOR6_PWM(n)	TIM_SetCompare2(TIM8,n)
#define SET_MOTOR7_PWM(n)	TIM_SetCompare1(TIM8,n)

static  void  MotorControl_Task (void *p_arg)
{
   	(void)p_arg;
   	uint8_t err;
	uint8_t i;
   	uint16_t PWMPeriod = (SystemCoreClock / 17570 ) - 1; //set PWM frequency 17.57KHz
	uint16_t ChannelPulse[6];
	uint8_t MotorDir[6];
   	Sem_NewIOMSG = OSSemCreate(0);

   	while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
		OSSemPend(Sem_NewIOMSG,0,&err);
		for(i = 0; i < 6; i++){
			if(DesCur[i] >= 0){
				MotorDir[i] = 0;
				//SET_M_DIR(i,1);
				
				ChannelPulse[i] = (uint16_t) (((uint32_t) DesCur[i] * (PWMPeriod- 1)) / 32760);
			}
			else{
				MotorDir[i] = 1;
				//SET_M_DIR(i,0);
				ChannelPulse[i] = (uint16_t) (((uint32_t) (0 - DesCur[i]) * (PWMPeriod- 1)) / 32760);
			}
			
			
		}
		SET_MOTOR1_PWM(ChannelPulse[0]);
		SET_M_DIR(1,MotorDir[0]);
		SET_MOTOR2_PWM(ChannelPulse[1]);
		SET_M_DIR(2,MotorDir[1]);
		SET_MOTOR3_PWM(ChannelPulse[2]);
		SET_M_DIR(3,MotorDir[2]);
		SET_MOTOR4_PWM(ChannelPulse[3]);
		SET_M_DIR(4,MotorDir[3]);
		SET_MOTOR5_PWM(ChannelPulse[4]);
		SET_M_DIR(5,MotorDir[4]);
		SET_MOTOR6_PWM(ChannelPulse[5]);
		SET_M_DIR(6,MotorDir[5]);
		//ChannelPulse = (uint16_t) (((uint32_t) 100 * (PWMPeriod- 1)) / 1000);
		
              
   }
}



