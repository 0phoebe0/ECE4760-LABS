/**
  ******************************************************************************
  * @file    drv_debug.c
  * @author  Wancheng Zhou
  * @version V1.0.0
  * @date    05-September-2014
  * @brief   This file provides application debug interface via USART0 hardware 
  *          peripheral:
  *           - Print formatted string which is totally compatible with standard
  *				printf function.
  *         
  *  @verbatim
  *
  *          ===================================================================
  *                                   How to use this driver
  *          ===================================================================
  *
  *			 1. Include the "drv_debug.h" in application files;
  *
  *          2. Change AVR GCC project properties:
  *				- Add "libprintf_flt.a" and "libm.a" in "Libraries";
  *				- Add "-Wl, -u, vfprintf" parameters in "Custom Options".
  *        
  *          3. Call "Drv_Debug_Printf" in application layer to output essential
  *				information for debugging.
  *          
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 CORNELL-ECE Wancheng Zhou </center></h2>
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "drv_debug.h"
#include "bsp_usart0.h"
#include "bsp_tmr0.h"
#include "bsp_tmr1.h"

#include <stdio.h>
#include <stdarg.h>

/** @addtogroup AVR_ATMega128_Demo_Debug_Driver
  * @{
  */

/** @defgroup LCD Print
  * @brief LCD Print Driver Modules
  * @{
  */

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static volatile uint8_t Drv_Tx_Buff[DRV_DEBUG_TX_BUFF_SIZE];
static volatile uint8_t Drv_Tx_Head;
static volatile uint8_t Drv_Tx_Tail;

/* Private function prototypes -----------------------------------------------*/
static void Drv_PutChar(uint8_t Data);

/* Private functions ---------------------------------------------------------*/

/** @defgroup Printf_Private_Functions
  * @{
  */ 

/** @defgroup Serial Printf Functions
 *  @brief    Total compatible printf function implementation.
 *
@verbatim    
 ===============================================================================
                      Driver Printf Functions
 ===============================================================================  
  This section provides functions allowing to:
   - Use USART0 to print out formatted string.
   
@endverbatim
  * @{
  */

/**
  * @brief  Print out user-defined formatted string for debugging purpose.
  * @param  fmt: pointer to the user-defined formatted string.
  * @retval None
  */

void Drv_Debug_Printf(const char *fmt, ...) {
	char DataBuff[64] = { 0 };
	va_list ArgPtr;
	int8_t  ChCnt = 0;
	int8_t  ChNbr = 0;
	
	/* Parse the formatted string via library function "vsnprintf" */
	/* Ref: #define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )  */
	/* Ref: #define va_end(ap) ( ap = (va_list)0 )  */
	va_start(ArgPtr, fmt);
	ChCnt = vsnprintf(DataBuff, 64, fmt, ArgPtr);
//	ChCnt = vsprintf(DataBuff, fmt, ArgPtr);
	va_end(ArgPtr);

	/* Put the formatted string out byte-by-byte */

	do {
		Drv_PutChar(DataBuff[ChNbr]);
		ChNbr ++;
	} while(ChNbr < ChCnt);

	return;
}

/**
  * @brief  Print out data via USART0 and internal Ring-Buffer.
  * @param  Data: 8-bit data needed to output.
  * @retval None
  */

static void Drv_PutChar(uint8_t Data) {
	
	uint8_t Next_Head;
	
	/* Pointer to next head to check ring buffer */
	Next_Head = (Drv_Tx_Head + 1) & DRV_DEBUG_TX_BUFF_SIZE_Msk;

	/* Transmit data directly only if Ring Buffer is empty and
	   the register UDR is empty */
	if ((Drv_Tx_Tail == Drv_Tx_Head) && IS_USART0_UDR_EMPTY()) {
		USART0_TX_DAT(Data);
	}
	/* Ring Buffer is not empty or register UDR is not empty */
	else {
		/* Check whether there still exists space in ring buffer */
		if(Next_Head != Drv_Tx_Tail) {
			Drv_Tx_Buff[Drv_Tx_Head] = Data;
			Drv_Tx_Head = Next_Head;
		}
		else
			while(!IS_USART0_UDR_EMPTY());
	}

	return;
}

/**
  * @brief  USART0 Transmit Complete Call Back Function
  * @param  None
  * @retval None
  */

void USART0_TXC_cbISR(void) {
	/* There still exists contents waiting to be sent out */
	if (Drv_Tx_Tail != Drv_Tx_Head) {
		USART0_TX_DAT(Drv_Tx_Buff[Drv_Tx_Tail]);
		Drv_Tx_Tail = (Drv_Tx_Tail + 1) & DRV_DEBUG_TX_BUFF_SIZE_Msk;
	}

	return;
}

/**
  * @brief  Debug Function Instantiate
  * @param  user-define
  * @retval user-define
  */


	

/************************ (C) COPYRIGHT Cornell ECE4760 ********END OF FILE****/
