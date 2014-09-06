#define F_CPU 16000000UL

#include "app_main.h"
#include <avr/io.h>
#include <avr/interrupt.h>

int main(void) {
	
	Bsp_Init();
	Drv_Init();
	
	sei();	

	Drv_Debug_Printf("Hello, AVR!\r\n");
	Drv_Debug_Printf("ECE 4760 LAB 1!\r\n");
	
	Drv_LCD_GotoXY(0, 0);
	Drv_LCD_String((void*)"CORNELL-ECE 4760", 16);
	Drv_Debug_Printf("LCD Display End!\r\n");

	while(1) {
	}
}
