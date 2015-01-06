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
* Filename      : bsp.h
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               BSP present pre-processor macro definition.
*
*           (2) This file and its dependencies requires IAR v6.20 or later to be compiled.
*
*********************************************************************************************************
*/

#ifndef  BSP_PRESENT
#define  BSP_PRESENT


/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/

#ifdef   BSP_MODULE
#define  BSP_EXT
#else
#define  BSP_EXT  extern
#endif


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/

#include  <stdio.h>
#include  <stdarg.h>

#include  <cpu.h>
#include  <cpu_core.h>

#include  <lib_def.h>
#include  <lib_ascii.h>

#include  <stm32f4xx_gpio.h>
#include  <stm32f4xx_spi.h>

#include "core_cm4.h"
#include "os_cfg.h"
/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 MACRO'S
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                               INT DEFINES
*********************************************************************************************************
*/

#define  BSP_INT_ID_WWDG                                   0u   /* Window WatchDog Interrupt                            */
#define  BSP_INT_ID_PVD                                    1u   /* PVD through EXTI Line detection Interrupt            */
#define  BSP_INT_ID_TAMP_STAMP                             2u   /* Tamper and TimeStamp Interrupt                       */
#define  BSP_INT_ID_RTC_WKUP                               3u   /* RTC wakeup Interrupt through the EXTI line           */
#define  BSP_INT_ID_FLASH                                  4u   /* FLASH global Interrupt                               */
#define  BSP_INT_ID_RCC                                    5u   /* RCC global Interrupt                                 */
#define  BSP_INT_ID_EXTI0                                  6u   /* EXTI Line0 Interrupt                                 */
#define  BSP_INT_ID_EXTI1                                  7u   /* EXTI Line1 Interrupt                                 */
#define  BSP_INT_ID_EXTI2                                  8u   /* EXTI Line2 Interrupt                                 */
#define  BSP_INT_ID_EXTI3                                  9u   /* EXTI Line3 Interrupt                                 */
#define  BSP_INT_ID_EXTI4                                 10u   /* EXTI Line4 Interrupt                                 */
#define  BSP_INT_ID_DMA1_CH0                              11u   /* DMA1 Channel 0 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH1                              12u   /* DMA1 Channel 1 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH2                              13u   /* DMA1 Channel 2 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH3                              14u   /* DMA1 Channel 3 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH4                              15u   /* DMA1 Channel 4 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH5                              16u   /* DMA1 Channel 5 global Interrupt                      */
#define  BSP_INT_ID_DMA1_CH6                              17u   /* DMA1 Channel 6 global Interrupt                      */
#define  BSP_INT_ID_ADC                                   18u   /* ADC1, ADC2 and ADC3 global Interrupt                 */
#define  BSP_INT_ID_CAN1_TX                               19u   /* CAN1 TX Interrupts                                   */
#define  BSP_INT_ID_CAN1_RX0                              20u   /* CAN1 RX0 Interrupts                                  */
#define  BSP_INT_ID_CAN1_RX1                              21u   /* CAN1 RX1 Interrupt                                   */
#define  BSP_INT_ID_CAN1_SCE                              22u   /* CAN1 SCE Interrupt                                   */
#define  BSP_INT_ID_EXTI9_5                               23u   /* External Line[9:5] Interrupts                        */
#define  BSP_INT_ID_TIM1_BRK_TIM9                         24u   /* TIM1 Break Interrupt and TIM9 global interrupt       */
#define  BSP_INT_ID_TIM1_UP_TIM10                         25u   /* TIM1 Update Interrupt and TIM10 global interrupt     */
#define  BSP_INT_ID_TIM1_TRG_COM_TIM11                    26u   /* TIM1 Trigger & Commutation Int. & TIM11 global Int.  */
#define  BSP_INT_ID_TIM1_CC                               27u   /* TIM1 Capture Compare Interrupt                       */
#define  BSP_INT_ID_TIM2                                  28u   /* TIM2 global Interrupt                                */
#define  BSP_INT_ID_TIM3                                  29u   /* TIM3 global Interrupt                                */
#define  BSP_INT_ID_TIM4                                  30u   /* TIM4 global Interrupt                                */
#define  BSP_INT_ID_I2C1_EV                               31u   /* I2C1 Event Interrupt                                 */
#define  BSP_INT_ID_I2C1_ER                               32u   /* I2C1 Error Interrupt                                 */
#define  BSP_INT_ID_I2C2_EV                               33u   /* I2C2 Event Interrupt                                 */
#define  BSP_INT_ID_I2C2_ER                               34u   /* I2C2 Error Interrupt                                 */
#define  BSP_INT_ID_SPI1                                  35u   /* SPI1 global Interrupt                                */
#define  BSP_INT_ID_SPI2                                  36u   /* SPI2 global Interrupt                                */
#define  BSP_INT_ID_USART1                                37u   /* USART1 global Interrupt                              */
#define  BSP_INT_ID_USART2                                38u   /* USART2 global Interrupt                              */
#define  BSP_INT_ID_USART3                                39u   /* USART3 global Interrupt                              */
#define  BSP_INT_ID_EXTI15_10                             40u   /* External Line[15:10] Interrupts                      */
#define  BSP_INT_ID_RTC_ALARM                             41u   /* RTC Alarms (A and B) through EXTI Line Interrupt     */
#define  BSP_INT_ID_OTG_FS_WKUP                           42u   /* USB WakeUp from suspend through EXTI Line Interrupt  */

#define  BSP_INT_ID_TIM8_BRK_TIM12                        43u   /* TIM8 Break Interrupt and TIM12 global Interrupt      */
#define  BSP_INT_ID_TIM8_UP_TIM13                         44u   /* TIM8 Update Interrupt and TIM13 global Interrupt     */
#define  BSP_INT_ID_TIM8_TRG_COM_TIM14                    45u   /* TIM8 Trigger/Commutation and TIM14 global Interrupt  */
#define  BSP_INT_ID_TIM8_CC                               46u   /* TIM8 Capture Compare Interrupt                       */
#define  BSP_INT_ID_DMA1_STREAM7                          47u   /* DMA1 Stream7 Interrupt                               */
#define  BSP_INT_ID_FSMC                                  48u   /* FSMC global Interrupt                                */
#define  BSP_INT_ID_SDIO                                  49u   /* SDIO global Interrupt                                */

#define  BSP_INT_ID_TIM5                                  50u   /* TIM5 global Interrupt                                */
#define  BSP_INT_ID_SPI3                                  51u   /* SPI3 global Interrupt                                */
#define  BSP_INT_ID_USART4                                52u   /* USART4 global Interrupt                              */
#define  BSP_INT_ID_USART5                                53u   /* USART5 global Interrupt                              */
#define  BSP_INT_ID_TIM6_DAC                              54u   /* TIM6 global Interrupt, DAC1 & DAC2 underrun err int. */
#define  BSP_INT_ID_TIM7                                  55u   /* TIM7 global Interrupt                                */
#define  BSP_INT_ID_DMA2_CH0                              56u   /* DMA2 Channel 0 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH1                              57u   /* DMA2 Channel 1 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH2                              58u   /* DMA2 Channel 2 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH3                              59u   /* DMA2 Channel 3 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH4                              60u   /* DMA2 Channel 4 global Interrupt                      */

#define  BSP_INT_ID_ETH                                   61u   /* ETH  global Interrupt                                */
#define  BSP_INT_ID_ETH_WKUP                              62u   /* ETH  WakeUp from EXTI line interrupt                 */
#define  BSP_INT_ID_CAN2_TX                               63u   /* CAN2 TX Interrupts                                   */
#define  BSP_INT_ID_CAN2_RX0                              64u   /* CAN2 RX0 Interrupts                                  */
#define  BSP_INT_ID_CAN2_RX1                              65u   /* CAN2 RX1 Interrupt                                   */
#define  BSP_INT_ID_CAN2_SCE                              66u   /* CAN2 SCE Interrupt                                   */
#define  BSP_INT_ID_OTG_FS                                67u   /* OTG global Interrupt                                 */

#define  BSP_INT_ID_DMA2_CH5                              68u   /* DMA2 Channel 5 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH6                              69u   /* DMA2 Channel 6 global Interrupt                      */
#define  BSP_INT_ID_DMA2_CH7                              70u   /* DMA2 Channel 7 global Interrupt                      */
#define  BSP_INT_ID_USART6                                71u   /* USART5 global Interrupt                              */
#define  BSP_INT_ID_I2C3_EV                               72u   /* I2C3 Event  Interrupt                                */
#define  BSP_INT_ID_I2C3_ER                               73u   /* I2C3 Error  Interrupt                                */
#define  BSP_INT_ID_OTG_HS_EP1_OUT                        74u   /* OTG HS EP1 OUT global Interrupt                      */
#define  BSP_INT_ID_OTG_HS_EP1_IN                         75u   /* OTG HS EP1 IN global Interrupt                       */
#define  BSP_INT_ID_OTG_HS_WKUP                           76u   /* OTG HS Wakeup Interrupt                              */
#define  BSP_INT_ID_OTG_HS                                77u   /* OTG HS global Interrupt                              */
#define  BSP_INT_ID_DCMI                                  78u   /* DCMI global Interrupt                                */
#define  BSP_INT_ID_CRYP                                  79u   /* CRYPT crypto global Interrupt                        */
#define  BSP_INT_ID_HASH_RNG                              80u   /* HASH and RNG global Interrupt                        */
#define  BSP_INT_ID_FPU                                   81u   /* FPU global Interrupt                                 */


/*
*********************************************************************************************************
*                                             PERIPH DEFINES
*********************************************************************************************************
*/

#define  BSP_PERIPH_ID_GPIOA                               0u
#define  BSP_PERIPH_ID_GPIOB                               1u
#define  BSP_PERIPH_ID_GPIOC                               2u
#define  BSP_PERIPH_ID_GPIOD                               3u
#define  BSP_PERIPH_ID_GPIOE                               4u
#define  BSP_PERIPH_ID_GPIOF                               5u
#define  BSP_PERIPH_ID_GPIOG                               6u
#define  BSP_PERIPH_ID_GPIOH                               7u
#define  BSP_PERIPH_ID_GPIOI                               8u
#define  BSP_PERIPH_ID_CRC                                12u
#define  BSP_PERIPH_ID_BKPSRAM                            18u
#define  BSP_PERIPH_ID_CCMDATARAM                         20u
#define  BSP_PERIPH_ID_DMA1                               21u
#define  BSP_PERIPH_ID_DMA2                               22u
#define  BSP_PERIPH_ID_ETHMAC                             25u
#define  BSP_PERIPH_ID_ETHMACTX                           26u
#define  BSP_PERIPH_ID_ETHMACRX                           27u
#define  BSP_PERIPH_ID_ETHMACPTP                          28u
#define  BSP_PERIPH_ID_OTGHS                              29u
#define  BSP_PERIPH_ID_OTGHSULPI                          30u

#define  BSP_PERIPH_ID_DCMI                               32u
#define  BSP_PERIPH_ID_CRYP                               36u
#define  BSP_PERIPH_ID_HASH                               37u
#define  BSP_PERIPH_ID_RNG                                38u
#define  BSP_PERIPH_ID_OTGFS                              39u

#define  BSP_PERIPH_ID_FSMC                               64u

#define  BSP_PERIPH_ID_TIM2                               96u
#define  BSP_PERIPH_ID_TIM3                               97u
#define  BSP_PERIPH_ID_TIM4                               98u
#define  BSP_PERIPH_ID_TIM5                               99u
#define  BSP_PERIPH_ID_TIM6                              100u
#define  BSP_PERIPH_ID_TIM7                              101u
#define  BSP_PERIPH_ID_TIM12                             102u
#define  BSP_PERIPH_ID_TIM13                             103u
#define  BSP_PERIPH_ID_TIM14                             104u
#define  BSP_PERIPH_ID_WWDG                              107u
#define  BSP_PERIPH_ID_SPI2                              110u
#define  BSP_PERIPH_ID_SPI3                              111u
#define  BSP_PERIPH_ID_USART2                            113u
#define  BSP_PERIPH_ID_USART3                            114u
#define  BSP_PERIPH_ID_USART4                            115u
#define  BSP_PERIPH_ID_USART5                            116u
#define  BSP_PERIPH_ID_I2C1                              117u
#define  BSP_PERIPH_ID_I2C2                              118u
#define  BSP_PERIPH_ID_I2C3                              119u
#define  BSP_PERIPH_ID_CAN1                              121u
#define  BSP_PERIPH_ID_CAN2                              122u
#define  BSP_PERIPH_ID_PWR                               124u
#define  BSP_PERIPH_ID_DAC                               125u

#define  BSP_PERIPH_ID_TIM1                              128u
#define  BSP_PERIPH_ID_TIM8                              129u
#define  BSP_PERIPH_ID_USART1                            132u
#define  BSP_PERIPH_ID_USART6                            133u
#define  BSP_PERIPH_ID_ADC1                              136u
#define  BSP_PERIPH_ID_ADC2                              137u
#define  BSP_PERIPH_ID_ADC3                              138u
#define  BSP_PERIPH_ID_SDIO                              139u
#define  BSP_PERIPH_ID_SPI1                              140u
#define  BSP_PERIPH_ID_SYSCFG                            142u
#define  BSP_PERIPH_ID_TIM9                              144u
#define  BSP_PERIPH_ID_TIM10                             145u
#define  BSP_PERIPH_ID_TIM11                             146u

/*
*********************************************************************************************************
*                                             UART DEFINES
*********************************************************************************************************
*/

#define __bsp_uart_console	2	//usart3
#define CONSOLE		USART3


/*
*********************************************************************************************************
*                                             CH378 DEFINES
*********************************************************************************************************
*/
#define CH378_DAT_PORT		GPIOD

#define CH378_INT_PORT		GPIOE
#define CH378_INT_PIN		GPIO_Pin_0

#define CH378_A0_PORT		GPIOE
#define CH378_A0_PIN		GPIO_Pin_1

#define CH378_nRD_PORT		GPIOE
#define CH378_nRD_PIN		GPIO_Pin_3

#define CH378_nWR_PORT		GPIOE
#define CH378_nWR_PIN		GPIO_Pin_4

#define CH378_nPCS_PORT		GPIOE
#define CH378_nPCS_PIN		GPIO_Pin_2

#define CH378_SET_A0()			GPIO_SetBits(CH378_A0_PORT,CH378_A0_PIN)
#define CH378_RESET_A0()		GPIO_ResetBits(CH378_A0_PORT,CH378_A0_PIN)

#define CH378_SET_nRD()			GPIO_SetBits(CH378_nRD_PORT,CH378_nRD_PIN)
#define CH378_RESET_nRD()		GPIO_ResetBits(CH378_nRD_PORT,CH378_nRD_PIN)

#define CH378_SET_nWR()			GPIO_SetBits(CH378_nWR_PORT,CH378_nWR_PIN)
#define CH378_RESET_nWR()		GPIO_ResetBits(CH378_nWR_PORT,CH378_nWR_PIN)

#define CH378_SET_nPCS()		GPIO_SetBits(CH378_nPCS_PORT,CH378_nPCS_PIN)
#define CH378_RESET_nPCS()		GPIO_ResetBits(CH378_nPCS_PORT,CH378_nPCS_PIN)

#define CH378_WRITE_DB(dat)		do{u16 temp = CH378_DAT_PORT->ODR&0xff00;temp+=dat;CH378_DAT_PORT->ODR=temp;}while(0)
#define CH378_READ_DB()			((CH378_DAT_PORT->IDR)&0xff)

/*
*********************************************************************************************************
*                                             LEDS AND KEYS DEFINES
*********************************************************************************************************
*/

#define LED1_PORT		GPIOE
#define LED1_PIN		GPIO_Pin_6

#define LED2_PORT		GPIOC
#define LED2_PIN		GPIO_Pin_13

#define LED3_PORT		GPIOC
#define LED3_PIN		GPIO_Pin_14

#define LED4_PORT		GPIOC
#define LED4_PIN		GPIO_Pin_15

#define KEY1_PORT		GPIOA
#define KEY1_PIN		GPIO_Pin_2

#define KEY2_PORT		GPIOA
#define KEY2_PIN		GPIO_Pin_3

#define KEY3_PORT		GPIOA
#define KEY3_PIN		GPIO_Pin_4

#define KEY4_PORT		GPIOA
#define KEY4_PIN		GPIO_Pin_5


/*
*********************************************************************************************************
*                                             MOTOR CONTROL DEFINES
*********************************************************************************************************
*/

#define M_DIS_PORT		GPIOD
#define M_DIS_PIN		GPIO_Pin_10

#define M_SDI_PORT		GPIOB
#define M_SDI_PIN		GPIO_Pin_15

#define M_SCLK_PORT		GPIOB
#define M_SCLK_PIN		GPIO_Pin_13

#define M_SDO_PORT		GPIOB
#define M_SDO_PIN		GPIO_Pin_14

#define M1_CSN_PORT		GPIOB
#define M1_CSN_PIN		GPIO_Pin_3

#define M1_DIR_PORT		GPIOE
#define M1_DIR_PIN		GPIO_Pin_13

#define M1_PWM_PORT		GPIOA
#define M1_PWM_PIN		GPIO_Pin_11


#define DISABLE_MOTOR()     GPIO_SetBits(M_DIS_PORT, M_DIS_PIN)
#define ENABLE_MOTOR()      GPIO_ResetBits(M_DIS_PORT, M_DIS_PIN)  

#define SET_SCK_HIGH()     GPIO_SetBits(M_SCLK_PORT, M_SCLK_PIN)
#define SET_SCK_LOW()      GPIO_ResetBits(M_SCLK_PORT, M_SCLK_PIN)  

#define SET_SDI_HIGH()     GPIO_SetBits(M_SDI_PORT, M_SDI_PIN)
#define SET_SDI_LOW()      GPIO_ResetBits(M_SDI_PORT, M_SDI_PIN)  


#define SET_M_CSN_HIGH(n)	GPIO_SetBits(M##n##_CSN_PORT, M##n##_CSN_PIN)
#define SET_M_CSN_LOW(n)	GPIO_ResetBits(M##n##_CSN_PORT, M##n##_CSN_PIN)

#define SET_M_DIR_HIGH(n)	GPIO_SetBits(M##n##_DIR_PORT, M##n##_DIR_PIN)
#define SET_M_DIR_LOW(n)	GPIO_ResetBits(M##n##_DIR_PORT, M##n##_DIR_PIN)

/*
*********************************************************************************************************
*                                             ENCODER DEFINES
*********************************************************************************************************
*/
#define ENC1	TIM3
#define ENC2	TIM5
#define ENC3	TIM4




/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void        BSP_Init                          (void);

void        BSP_IntDisAll                     (void);

CPU_INT32U  BSP_CPU_ClkFreq                   (void);
void systick_init(void) ;
void  BSP_LED_Init(void);
void SetEncoder(uint32_t dat, uint8_t n);
uint32_t GetEncoder(uint8_t n);
uint8_t GetKeys(void);





/*
*********************************************************************************************************
*                                     PERIPHERAL POWER/CLOCK SERVICES
*********************************************************************************************************
*/

CPU_INT32U   BSP_PeriphClkFreqGet        (CPU_DATA       pwr_clk_id);

void         BSP_PeriphEn                (CPU_DATA       pwr_clk_id);

void         BSP_PeriphDis               (CPU_DATA       pwr_clk_id);


/*
*********************************************************************************************************
*                                              LED SERVICES
*********************************************************************************************************
*/

void        BSP_LED_On                        (CPU_INT08U     led);

void        BSP_LED_Off                       (CPU_INT08U     led);

void        BSP_LED_Toggle                    (CPU_INT08U     led);


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                         
/* End of module include.                               */

