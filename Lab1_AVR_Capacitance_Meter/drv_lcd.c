#include "drv_lcd.h"
#include <util/delay.h>

void Drv_LCD_SendChar(uint8_t Char) {

#if (LCD_OP_MODE == LCD_MODE_4BIT)
	LCD_DAT_PORT &= 0x0F;
	LCD_DAT_PORT |= (Char & 0xF0);			/*!> Send most significant 4 bits 	*/
	LCD_CMD_PORT |= (1 << LCD_RS);			/*!> Set RS & EN to send data		*/
	LCD_CMD_PORT |= (1 << LCD_EN);
	_delay_ms(2);							/*!> Essential delay				*/
	LCD_CMD_PORT &= ~(1 << LCD_EN);			/*!> Clear EN firstly to send data  */
	LCD_CMD_PORT &= ~(1 << LCD_RS);			/*!> Clear RS for safety 			*/
	_delay_ms(2);							/*!> Essential delay				*/
	
	LCD_DAT_PORT &= 0x0F;
	LCD_DAT_PORT |= ((Char & 0x0F) << 4);	/*!> Send least significant 4 bits 	*/
	LCD_CMD_PORT &= ~(1 << LCD_EN);			/*!> Clear EN firstly to send data  */
	LCD_CMD_PORT &= ~(1 << LCD_RS);			/*!> Clear RS for safety 			*/
	_delay_ms(2);							/*!> Essential delay				*/

	LCD_CMD_PORT &= ~(1 << LCD_EN);
	LCD_CMD_PORT &= ~(1 << LCD_RS);			/*!> Clear RS & EN    				*/
	_delay_ms(2);							/*!> Essential delay				*/
#endif

}

void Drv_LCD_SendCmd(uint8_t Cmd) {

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
	Drv_LCD_SendCmd(0x06);
	Drv_LCD_SendCmd(0x01);
#endif

}

void Drv_LCD_Clear(void) {
	Drv_LCD_SendCmd(1 << LCD_CLR);
}

void Drv_LCD_Home(void) {
	Drv_LCD_SendCmd(1 << LCD_HOME);
}

void Drv_LCD_String(uint8_t* Data, uint8_t nBytes) {

	uint8_t BytePos;

//	if (!Data)	return;

	for (BytePos = 0; BytePos < nBytes; BytePos++) {
		Drv_LCD_SendChar(Data[BytePos]);
	}
}

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
