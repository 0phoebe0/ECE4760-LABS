/*
 * AVR_CricketCall.c
 *
 * Created: 2014/9/26 23:32:38
 *  Author: Michael
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "AVR_CricketCall.h"

ui_config_t			user_conf_stat;
volatile uint16_t	tmr_counter = 0;
uint16_t            ui_refresh_frame;
uint8_t				key_input = NO_KEY;

sys_mode_t			SystemMode = SYS_DDS_CONFIG;

int main(void)
{
	Bsp_Init();
	Drv_Init();
	
	Drv_LCD_Clear();
	
	Drv_Debug_Printf("System Initialized!\r\n");
	
	App_UI_Display_Welcome();
	App_DDS_Init();
	
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
				if ((key_input == KEY_EXT_C) && (user_conf_stat == UI_CONF_RES_WAIT)) {
					Drv_Debug_Printf("Begin!\r\n");
					App_DDS_Para_Calc(usr_conf_para);
					SystemMode = SYS_DDS_RUN;
				}
			break;
			
			case SYS_DDS_RUN:
				if (key_input == KEY_EXT_D) {
					dds_play_ctrl = false;
					SystemMode = SYS_DDS_STOP;
				}
				App_DDS_Task_Exec();
			break;
			
			case SYS_DDS_STOP:
				SystemMode = SYS_DDS_CONFIG;
			break;
			
			default:
				/*!> Handle System Exception Here! */
			break;
		}
		
		user_conf_stat = App_UserInput_Handler(&key_input, &ui_refresh_frame);
    }
}
