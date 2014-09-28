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
*                                            LOCAL DEFINES
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* --------------- APPLICATION GLOBALS ---------------- */
static  OS_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  OS_STK  UMon_task_stk[SMALL_TASK_STK_SIZE ];  
static  OS_STK  Console_task_stk[LARGE_TASK_STK_SIZE ]; 


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart          (void  *p_arg);


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

    BSP_Init();                                                 /* Initialize BSP functions                             */
	BSP_LED_Init();                                             /* Init LEDs.                                           */
	systick_init();                                            /* Start Tick Initialization                            */
    BSP_LED_Off(0u);
    OSTaskCreateExt(UartMonitor_task,
						(void *)0,								  
						&UMon_task_stk[SMALL_TASK_STK_SIZE - 1], 
						UMON_TASK_PRIO, 					
						UMON_TASK_PRIO,
						&UMon_task_stk[0],						  
						SMALL_TASK_STK_SIZE,
						(void *)0,		
						OS_TASK_OPT_NONE);
	

	OSTaskCreateExt(ConsoleService_task,
						(void *)0,								
						&Console_task_stk[LARGE_TASK_STK_SIZE - 1],
						CONSOLE_TASK_PRIO,						
						CONSOLE_TASK_PRIO,
						&Console_task_stk[0], 					
						LARGE_TASK_STK_SIZE,
						(void *)0,								
						OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	
	mInitCH378Device();
	while (DEF_TRUE) {                                          /* Task body, always written as an infinite loop.       */
        BSP_LED_Toggle(0u);
		OSTimeDly(50);
              
    }
}


