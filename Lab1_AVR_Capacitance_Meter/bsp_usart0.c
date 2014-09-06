/**
  ******************************************************************************
  * @file    bsp_usart0.c
  * @author  Wancheng Zhou
  * @version V1.0.0
  * @date    05-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the USART0 peripheral:
  *           - Initialization and Configuration
  *           - USART0 transmit complete Interrupt Service Routine and
  *			 	corresponding Call Back Function
  *         
  *  @verbatim
  *
  *          ===================================================================
  *                                   How to use this driver
  *          ===================================================================
  *
  *			 1. Include the "bsp_usart0.h" in driver files;
  *
  *          2. Configure Baud Rate parameters in "bsp_usart0.h";
  *        
  *          3. Call Bsp_USART0_Init() to initialize the USART0 interface;
  *
  *          4. Instantiate the function "USART0_TXC_cbISR()" and add necessary
  *				operations.
  *          
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  *	USART0 RX ISR has not been implemented yet.
  *
  * <h2><center>&copy; COPYRIGHT 2014 CORNELL-ECE Wancheng Zhou </center></h2>
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "bsp_usart0.h"
#include <avr/interrupt.h>

/** @addtogroup AVR_ATMega128_StdPeriph_Driver
  * @{
  */

/** @defgroup USART0
  * @brief USART0 driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup USART0_Private_Functions
  * @{
  */ 

/** @defgroup USART0 Initialization and ISR functions.
 *  @brief    Initialization and ISR Implementation. 
 *
@verbatim    
 ===============================================================================
                      Initialization and ISR functions
 ===============================================================================  
  This section provides functions allowing to:
   - Initialize the USART0 hardware interface;
   - ISR call back function (weak implementation, need user's instantiation).
   
@endverbatim
  * @{
  */

/**
  * @brief  Initialize the USART0 according to the parameters defined in
  *			"bsp_usart0.h", default configuration is:
  *			- 9600 bps
  *			- 8-bit Data Bits
  *			- 1-bit Stop Bit
  *			- No parity check
  *			- Asynchronous mode
  * @param  None
  * @retval None
  */

void Bsp_USART0_Init(void) {

	/* Baudrate Configuration */
	UBRR0H = USART0_UBRR_H;							
	UBRR0L = USART0_UBRR_L;
	
	/* X2 Option Configuration: Baud Rate Multipy */
#if (USART0_BAUD_X2 == 0)
	UCSR0A |= (0 << U2X0);
#else
	UCSR0A |= (1 << U2X0);
#endif
	
	/* 0xD8: RX Interrupt & TX Interrupt */
	UCSR0B &= ~ (1 << UCSZ02 );
	UCSR0B |=  ((1 << RXCIE0 ) | (1 << TXCIE0 ) | 
			    (1 << RXEN0  ) | (1 << TXEN0  ));   
	
	/* 0x06: Async Mode, 8-bit Data, 1-bit STO */
	UCSR0C &= ~((1 << UMSEL01) | (1 << UMSEL00) |
				(1 << UPM01  ) | (1 << UPM00  ) |
				(1 << USBS0  ) | (1 << UCPOL0 ));
	UCSR0C |=  ((1 << UCSZ01 ) | (1 << UCSZ00 ));	

	UDR0    = 0;
}

/**
  * @brief  Declare the prototype of call-back function. User must instantiate
  *			this function in corresponding driver.
  * @param  None
  * @retval None
  */
void __attribute((weak)) USART0_TXC_cbISR(void) 
{
	return;
}

/**
  * @brief  USART0 Transmit Complete (TXC ISR) Interrupt Service Routine
  * @param  None
  * @retval None
  */

ISR(USART0_TX_vect) {
	USART0_TXC_cbISR();
}

/************************ (C) COPYRIGHT Cornell ECE4760 ********END OF FILE****/
