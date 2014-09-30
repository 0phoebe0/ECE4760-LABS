/*
 * app_ui.c
 *
 * Created: 2014/9/29 10:28:38
 *  Author: Michael
 */ 

#include "app_ui.h"
#include "drv_lcd.h"
#include "drv_keypad.h"
#include "drv_debug.h"

#include <avr/interrupt.h>

const uint8_t msg_config_array[5][16] = {
	MSG_CONFIG_PARA1_LINE,
	MSG_CONFIG_PARA2_LINE,
	MSG_CONFIG_PARA3_LINE,
	MSG_CONFIG_PARA4_LINE,
	MSG_CONFIG_PARA5_LINE
};

const para_range_t para_range_val[5] = {
	{PARA1_MIN, PARA1_MAX},
	{PARA2_MIN, PARA2_MAX},
	{PARA3_MIN, PARA3_MAX},
	{PARA4_MIN, PARA4_MAX},
	{PARA5_MIN, PARA5_MAX},
};

static ui_config_t ui_conf_stat = UI_DISP_INIT;
uint16_t    usr_conf_para[5];

void App_Display(const uint8_t* disp_content) {
//	Drv_LCD_Printf((PGM_P)disp_content);
}

void App_UI_Display_Welcome(void) {
	Drv_LCD_GotoXY(0, 0);
	Drv_LCD_Printf(MSG_WELCOME_1ST_LINE);
	Drv_LCD_GotoXY(0, 1);
	Drv_LCD_Printf(MSG_WELCOME_2ND_LINE);
}

/* If this variable was config as local parameter, exception happens 
   Find this problem source! It's something relevant about compiler */
static volatile uint8_t para_conf_ptr;   

ui_config_t App_UserInput_Handler(uint8_t* key_value, uint16_t *ui_refresh) {
	
	static uint8_t para_array[5][5];
	static int8_t  calc_digits;
	static uint8_t inp_digits;
	static uint8_t refresh_index;
	uint16_t       multiply_const = 1;
	uint8_t		   dbg_tmp;
	
	switch (ui_conf_stat) {
		
		case UI_DISP_INIT:
			if (*key_value != NO_KEY)
				ui_conf_stat = UI_DISP_HINT;
		break;
		
		case UI_DISP_HINT:
			Drv_LCD_Clear();
			Drv_LCD_GotoXY(0, 0);
			Drv_LCD_Printf((void*)msg_config_array[para_conf_ptr]);
			Drv_LCD_GotoXY(0, 1);
			Drv_LCD_Printf(MSG_CONFIG_INP_LINE);
			inp_digits = 0;
			
			ui_conf_stat = UI_GET_INPUT;
		break;
		
		case UI_DISP_ERROR:
			Drv_Debug_Printf("Display Error!\r\n");
			Drv_LCD_Clear();
			Drv_LCD_GotoXY(0, 0);
			Drv_LCD_Printf(MSG_ERROR_1ST_LINE);
			Drv_LCD_GotoXY(0, 1);
			Drv_LCD_Printf(MSG_ERROR_2ND_LINE);
			ui_conf_stat = UI_RE_INPUT;
		break;
		
		case UI_RE_INPUT:
			if (*key_value == KEY_EXT_A) {
				ui_conf_stat = UI_DISP_HINT;
				*key_value = NO_KEY;
			}
		break;
		
		case UI_GET_INPUT:
			if (*key_value != NO_KEY) {
				if ((*key_value < KEY_EXT_A) && (inp_digits < 4)) {
					para_array[para_conf_ptr][inp_digits] = *key_value;
					Drv_LCD_SendChar(KeyPad_ASCII_Table[*key_value]);
					inp_digits ++;
				}
				if ((*key_value == KEY_EXT_A) || (inp_digits == 4)) {
					Drv_Debug_Printf("Digits:%d\r\n", inp_digits);
					para_array[para_conf_ptr][inp_digits + 1] = 0xFF;
					usr_conf_para[para_conf_ptr] = 0; /* Clear Firstly */
					for(calc_digits = (inp_digits - 1); calc_digits >= 0; calc_digits--) {
						usr_conf_para[para_conf_ptr] += para_array[para_conf_ptr][calc_digits]	* multiply_const;
						multiply_const *= 10;
					}
					inp_digits = 0; /**/
					Drv_Debug_Printf("Input Over: %d, para count: %d\r\n", usr_conf_para[para_conf_ptr], para_conf_ptr);
					
					if (IS_PARA_INRANGE(usr_conf_para[para_conf_ptr],
										para_range_val[para_conf_ptr].para_min_val,
										para_range_val[para_conf_ptr].para_max_val )) {
						para_conf_ptr ++;
						Drv_Debug_Printf("Current Para Count: %d\r\n", para_conf_ptr);
						if (para_conf_ptr == 5) {
							Drv_Debug_Printf("Config Over!\r\n");
							para_conf_ptr = 0;
							ui_conf_stat = UI_CONF_OVER;
						}
						else
							ui_conf_stat = UI_DISP_HINT;
					}
					else {
						usr_conf_para[para_conf_ptr] = 0;
						Drv_Debug_Printf("Error!\r\n");
						inp_digits = 0;
						ui_conf_stat = UI_DISP_ERROR;
					}
				}
				
				*key_value = NO_KEY;
			}
		break;	
		
		case UI_CONF_PARA:
		break;
		
		case UI_CONF_OVER:		
			for (dbg_tmp = 0; dbg_tmp < 5; dbg_tmp++) {
				Drv_Debug_Printf("Parameter: %d = %d\r\n", dbg_tmp, usr_conf_para[dbg_tmp]);
			}
			Drv_LCD_GotoXY(0, 0);
			Drv_LCD_Printf(MSG_PREP_PLAY_1ST_LINE);
			Drv_LCD_GotoXY(0, 1);
			Drv_LCD_Printf(MSG_PREP_PLAY_2ND_LINE);
			ui_conf_stat = UI_CONF_RES_WAIT;
		break;
		
		case UI_CONF_RES_WAIT:
//			if (*key_value == KEY_EXT_D)
//				ui_conf_stat = UI_DISP_HINT;
			if (*key_value == KEY_EXT_C)
				ui_conf_stat = UI_DISP_REFRESH;
		break;
		
		case UI_DISP_REFRESH:
			if (*key_value == KEY_EXT_D) {
				ui_conf_stat = UI_DISP_HINT;
			}
			if (*ui_refresh >= 62500) {
				*ui_refresh = 0;
				
#if 0
                Drv_LCD_IntClear();
				Drv_LCD_IntGotoXY(0, 0);
				Drv_LCD_IntPrintf((void*)msg_config_array[refresh_index]);
				Drv_LCD_IntGotoXY(0, 1);
                switch (refresh_index) {
                    case 0:
                    case 2:
                    case 3:
                        Drv_LCD_IntPrintf("VAL: %d ms", usr_conf_para[refresh_index]);
                    break;

                    case 4:
                        Drv_LCD_IntPrintf("VAL: %d Hz", usr_conf_para[refresh_index]);
                    break;

                    case 1:
                        Drv_LCD_IntPrintf("VAL: %d   ", usr_conf_para[refresh_index]);
                    break;
                }
				
#endif

#if 1
                Drv_LCD_Clear();
                Drv_LCD_GotoXY(0, 0);
                Drv_LCD_Printf((void*)msg_config_array[refresh_index]);
                Drv_LCD_GotoXY(0, 1);
                switch (refresh_index) {
                    case 0:
                    case 2:
                    case 3:
                        Drv_LCD_Printf("VAL: %d ms", usr_conf_para[refresh_index]);
                    break;

                    case 4:
                        Drv_LCD_Printf("VAL: %d Hz", usr_conf_para[refresh_index]);
                    break;

                    case 1:
                        Drv_LCD_Printf("VAL: %d   ", usr_conf_para[refresh_index]);
                    break;
                }
#endif
				refresh_index++;
				if (refresh_index >= 5) {
					refresh_index = 0; 
				} 
			}
		break;
	}
	
	return ui_conf_stat;
}
