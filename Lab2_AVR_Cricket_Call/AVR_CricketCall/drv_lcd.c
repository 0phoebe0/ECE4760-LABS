/**
  ******************************************************************************
  * @file    drv_lcd.c
  * @author  Wancheng Zhou
  * @version V1.0.0
  * @date    05-September-2014
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the LCD1602 (HD4408):
  *           - Initialization and Configuration
  *           - Set coordinate of the LCD
  *			  - Clear LCD
  *			  - Set cursor to home
  *           - Print out formatted string defined by the user
  *         
  *  @verbatim
  *
  *          ===================================================================
  *                                   How to use this driver
  *          ===================================================================
  *
  *			 1. Include the "drv_lcd.h" in application files;
  *
  *          2. Use Drv_LCD_GotoXY to cooperate with Drv_LCD_Printf to print out
  *				formatted information defined by user.
  *          
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * 8-bit parallel interface has not been implemented.
  *
  * <h2><center>&copy; COPYRIGHT 2014 CORNELL-ECE Wancheng Zhou </center></h2>
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#define  F_CPU               16000000UL

#include "drv_lcd.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdarg.h>

/** @addtogroup AVR_ATMega128_Demo_LCD_Driver
  * @{
  */

/** @defgroup LCD
  * @brief LCD driver modules
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void Drv_LCD_SendChar(uint8_t Char);
static void Drv_LCD_SendCmd	(uint8_t Cmd);

/** @defgroup LCD_Private_Functions
  * @{
  */ 

/** @defgroup LCD Printf Functions.
 *  @brief    output formatted information via LCD module 
 *
@verbatim    
 ===============================================================================
                      Initialization and Output Functions
 ===============================================================================  
  This section provides functions allowing to:
   - Initialize the USART0 hardware interface;
   - ISR call back function (weak implementation, need user's instantiation).
   
@endverbatim
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Send data to LCD (RS is pulled high while EN is used as clock)
  * @param  Char: Data need to be sent out.
  * @retval None
  */

static void Drv_LCD_SendChar(uint8_t Char) {

#if (LCD_OP_MODE == LCD_MODE_4BIT)
	LCD_DAT_PORT &= 0x0F;
	LCD_DAT_PORT |= (Char & 0xF0);			/*!> Send most significant 4 bits 	*/
	LCD_CMD_PORT |= (1 << LCD_RS);			/*!> Set RS & EN to send data		*/
	LCD_CMD_PORT |= (1 << LCD_EN);
	_delay_ms(2);							/*!> Essential delay				*/
	LCD_CMD_PORT &= ~(1 << LCD_EN);			/*!> Clear EN firstly to send data  */
	_delay_ms(2);							/*!> Essential delay				*/
	
	LCD_DAT_PORT &= 0x0F;
	LCD_DAT_PORT |= ((Char & 0x0F) << 4);	/*!> Send least significant 4 bits 	*/
	LCD_CMD_PORT |= (1 << LCD_EN);
	_delay_ms(2);							/*!> Essential delay				*/
	LCD_CMD_PORT &= ~(1 << LCD_EN);			/*!> Clear EN firstly to send data  */
	LCD_CMD_PORT &= ~(1 << LCD_RS);			/*!> Clear RS & EN    				*/
	_delay_ms(2);							/*!> Essential delay				*/
#endif

}

/**
  * @brief  Send command to LCD (RS is pulled down while EN is used as clock)
  * @param  Cmd: Command need to be sent out.
  * @retval None
  */

static void Drv_LCD_SendCmd(uint8_t Cmd) {

#if (LCD_OP_MODE == LCD_MODE_4BIT)
	LCD_DAT_PORT &= 0x0F;
	LCD_DAT_PORT |= (Cmd & 0xF0); 
	LCD_CMD_PORT |= (1 << LCD_EN);
	_delay_ms(2);							/*!> Essential delay				*/
	LCD_CMD_PORT &= ~(1 << LCD_EN);
	_delay_ms(2);							/*!> Essential delay				*/

	LCD_DAT_PORT &= 0x0F;
	LCD_DAT_PORT |= ((Cmd & 0x0F) << 4); 
	LCD_CMD_PORT |= (1 << LCD_EN);
	_delay_ms(2);							/*!> Essential delay				*/
	LCD_CMD_PORT &= ~(1 << LCD_EN);
	_delay_ms(2);							/*!> Essential delay				*/
#endif

}

/**
  * @brief  Initialize the LCD module.
  * @param  None
  * @retval None
  */

void Drv_LCD_Init(void) {

#if (LCD_OP_MODE == LCD_MODE_4BIT)

	_delay_ms(15);
	LCD_DAT_PORT = 0x00;
	LCD_CMD_PORT = 0x00;
	LCD_DAT_PDDR |= (1 << LCD_D7) | (1 << LCD_D6) | (1 << LCD_D5) | (1 << LCD_D4);
	LCD_CMD_PDDR |= (1 << LCD_EN) | (1 << LCD_RW) | (1 << LCD_RS);

	LCD_DAT_PORT = 0x30;
	LCD_CMD_PORT |= (1 << LCD_EN) | (0 << LCD_RW) | (0 << LCD_RS);
	_delay_ms(2);
	LCD_CMD_PORT &= ~(1 << LCD_EN);
	_delay_ms(2);

	LCD_DAT_PORT = 0x30;
	LCD_CMD_PORT |= (1 << LCD_EN) | (0 << LCD_RW) | (0 << LCD_RS);
	_delay_ms(2);
	LCD_CMD_PORT &= ~(1 << LCD_EN);
	_delay_ms(2);

	LCD_DAT_PORT = 0x20;
	LCD_CMD_PORT |= (1 << LCD_EN) | (0 << LCD_RW) | (0 << LCD_RS);
	_delay_ms(2);
	LCD_CMD_PORT &= ~(1 << LCD_EN);
	_delay_ms(2);

	Drv_LCD_SendCmd(0x28);
	Drv_LCD_SendCmd(0x0C);
//	Drv_LCD_SendCmd(0x06);
//	Drv_LCD_SendCmd(0x01);
#endif

}

/**
  * @brief  Clear all information LCD displayed.
  * @param  None
  * @retval None
  */

void Drv_LCD_Clear(void) {
	Drv_LCD_SendCmd(1 << LCD_CLR);
	return;
}

/**
  * @brief  Reset coordinate of LCD.
  * @param  None
  * @retval None
  */

void Drv_LCD_Home(void) {
	Drv_LCD_SendCmd(1 << LCD_HOME);
}

/**
  * @brief  Print out string.
  * @param  None
  * @retval None
  */

void Drv_LCD_String(char* Data, uint8_t nBytes) {

	uint8_t BytePos;

	if (!Data)	return;

	for (BytePos = 0; BytePos < nBytes; BytePos++) {
		Drv_LCD_SendChar(Data[BytePos]);
	}
}

/**
  * @brief  Print out user-defined formatted information.
  * @param  None
  * @retval None
  */

void Drv_LCD_Printf(const char *fmt, ...) {
	char DataBuff[64] = { 0 };
	va_list ArgPtr;
	int8_t  ChCnt;
	
	va_start(ArgPtr, fmt);
	ChCnt = vsnprintf(DataBuff, 64, fmt, ArgPtr);
	va_end(ArgPtr);
	
	Drv_LCD_String(DataBuff, ChCnt);
}

/**
  * @brief  Set cursor coordinate of LCD.
  * @param  None
  * @retval None
  */

void Drv_LCD_GotoXY(uint8_t x, uint8_t y) {

	uint8_t DDRAMAddr;

	switch(y) {
		case 	0: DDRAMAddr = LCD_LINE0_DDRAMADDR + x; break;
		case 	1: DDRAMAddr = LCD_LINE1_DDRAMADDR + x; break;
		case 	2: DDRAMAddr = LCD_LINE2_DDRAMADDR + x; break;
		case 	3: DDRAMAddr = LCD_LINE3_DDRAMADDR + x; break;
		default	 : DDRAMAddr = LCD_LINE0_DDRAMADDR + x;
	}

	Drv_LCD_SendCmd((1 << LCD_DDRAM) | DDRAMAddr);
}

/************************ (C) COPYRIGHT Cornell ECE4760 ********END OF FILE****/
