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
	Drv_Debug_Printf("Float Test: %.4f\r\n", 3.65);
	
	Drv_LCD_GotoXY(0, 0);
	Drv_LCD_String((void*)"CORNELL-ECE 4761", 16);
	Drv_Debug_Printf("LCD Display End!\r\n");

	Drv_LCD_GotoXY(0, 1);
	Drv_LCD_Printf("Float: %.4f", 3.65);

	while(1) {
	}
}
