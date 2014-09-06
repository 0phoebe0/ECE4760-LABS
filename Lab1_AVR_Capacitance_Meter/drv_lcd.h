#ifndef __DRV_LCD_H__
#define __DRV_LCD_H__

#include <avr/io.h>

#define LCD_MODE_8BIT  	0
#define LCD_MODE_4BIT	1
#define LCD_OP_MODE		LCD_MODE_4BIT

#define LCD_RS			0
#define LCD_RW			1
#define LCD_EN			2

#if 	(LCD_OP_MODE == LCD_MODE_4BIT)
	#define LCD_CMD_PORT 	PORTC
	#define LCD_DAT_PORT	PORTC
	#define LCD_CMD_PDDR	DDRC
	#define LCD_DAT_PDDR	DDRC
#elif 	(LCD_OP_MODE == LCD_MODE_8BIT)
	#define LCD_CMD_PORT 	PORTA
	#define LCD_DAT_PORT	PORTB
	#define LCD_CMD_PDDR	DDRA
	#define LCD_DAT_PDDR	DDRB
	#define LCD_D0			0
	#define LCD_D1			1
	#define LCD_D2			2
	#define LCD_D3			3
#endif

#define LCD_D4			4
#define LCD_D5			5
#define LCD_D6			6
#define LCD_D7			7

#define LCD_CMD			0
#define LCD_DAT			1

#define LCD_CLR             0	/*!< DB0: clear display						*/
#define LCD_HOME            1	/*!< DB1: return to home position			*/
#define LCD_ENTRY_MODE      2	/*!< DB2: set entry mode					*/
#define LCD_ENTRY_INC       1	/*!< DB1: increment							*/
#define LCD_ENTRY_SHIFT     0	/*!< DB2: shift								*/
#define LCD_ON_CTRL         3	/*!< DB3: turn lcd/cursor on				*/
#define LCD_ON_DISPLAY      2	/*!< DB2: turn display on					*/
#define LCD_ON_CURSOR       1	/*!< DB1: turn cursor on					*/
#define LCD_ON_BLINK        0	/*!< DB0: blinking cursor					*/
#define LCD_MOVE            4	/*!< DB4: move cursor/display				*/
#define LCD_MOVE_DISP       3	/*!< DB3: move display (0-> move cursor)	*/
#define LCD_MOVE_RIGHT      2	/*!< DB2: move right (0-> left)				*/
#define LCD_FUNCTION        5	/*!< DB5: function set						*/
#define LCD_FUNCTION_8BIT   4	/*!< DB4: set 8BIT mode (0->4BIT mode)		*/
#define LCD_FUNCTION_2LINES 3	/*!< DB3: two lines (0->one line)			*/
#define LCD_FUNCTION_10DOTS 2	/*!< DB2: 5x10 font (0->5x7 font)			*/
#define LCD_CGRAM           6	/*!< DB6: set CG RAM address				*/
#define LCD_DDRAM           7	/*!< DB7: set DD RAM address				*/

#define LCD_LINE0_DDRAMADDR		0x00
#define LCD_LINE1_DDRAMADDR		0x40
#define LCD_LINE2_DDRAMADDR		0x14
#define LCD_LINE3_DDRAMADDR		0x54

void Drv_LCD_GotoXY		(uint8_t x, uint8_t y);
void Drv_LCD_String		(uint8_t *Data, uint8_t nBytes);
void Drv_LCD_Home		(void);
void Drv_LCD_Clear		(void);
void Drv_LCD_Init		(void);

#endif
