/*
 * AVR_CricketCall.c
 *
 * Created: 2014/9/26 23:32:38
 *  Author: Michael
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "AVR_CricketCall.h"

volatile uint16_t tmr_counter = 0;
uint8_t  key_input;

sys_mode_t SystemMode = SYS_DDS_CONFIG;

int main(void)
{
	Bsp_Init();
	Drv_Init();
	
	Drv_LCD_GotoXY(0, 0);
	Drv_LCD_Clear();
	Drv_LCD_Printf("Cricket Call");
	
	Drv_Debug_Printf("System Initialized!\r\n");

	App_DDS_Init();
	App_DDS_Para_Calc();	/*!> Call this function to initialize DDS parameters */
	
	TMR0_START_COUNT();
	TMR0_ENABLE_OVF_ISR();
	
	sei();
	
    while(1)
    {
        //TODO:: Please write your application code 
		if (tmr_counter >= 625) {
			tmr_counter = 0;
			key_input = Drv_KeyPadScan();
			if (key_input != NO_KEY) {
				Drv_Debug_Printf("KeyValue: %d\r\n", key_input);
			}
		}
		
		switch (SystemMode) {
			case SYS_DDS_CONFIG:
			break;
			
			case SYS_DDS_RUN:
				App_DDS_Task_Exec();
			break;
			
			case SYS_DDS_STOP:
			break;
			
			default:
				/*!> Handle System Exception Here! */
			break;
		}
		
    }
}
