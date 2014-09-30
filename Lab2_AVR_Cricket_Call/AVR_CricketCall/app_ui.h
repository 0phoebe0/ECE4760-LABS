/*
 * app_ui.h
 *
 * Created: 2014/9/29 10:29:05
 *  Author: Michael
 */ 


#ifndef APP_UI_H_
#define APP_UI_H_

#include <stdint.h>

#define MSG_FOR_WIDTH_REF		"0123456789ABCDEF"

#define MSG_WELCOME_1ST_LINE	"  ECE4760 LAB2  "
#define MSG_WELCOME_2ND_LINE	"Press ANY Key..."

#define MSG_ERROR_1ST_LINE		"Don't Fool me..."
#define MSG_ERROR_2ND_LINE		"You need ReInput"

#define MSG_PREP_PLAY_1ST_LINE	"AVR-(^_^)Cricket"
#define MSG_PREP_PLAY_2ND_LINE  "Press Start...  "

#define MSG_CONFIG_INP_LINE		"Input:"
#define MSG_CONFIG_PARA1_LINE	"CHIRP REPT TIME"
#define MSG_CONFIG_PARA2_LINE	"SYLLABLE NUMBER"
#define MSG_CONFIG_PARA3_LINE	"SYLLABLE PERIOD"
#define MSG_CONFIG_PARA4_LINE   "SYLLABLE REPEAT"
#define MSG_CONFIG_PARA5_LINE   "BURST FREQUENCY"

typedef enum {
	UI_DISP_INIT,
	UI_DISP_HINT,
	UI_DISP_ERROR,
	UI_DISP_REFRESH,
	UI_RE_INPUT,
	UI_GET_INPUT, 
	UI_CONF_PARA,
	UI_CONF_OVER,
	UI_CONF_RES_WAIT,
} ui_config_t;

#define CONF_PARA_NUM				4

#define PARA1_MIN					10
#define PARA1_MAX					1500
#define PARA2_MIN					1
#define PARA2_MAX					100
#define PARA3_MIN					5
#define PARA3_MAX					100
#define PARA4_MIN					10
#define PARA4_MAX					100
#define PARA5_MIN					1000
#define PARA5_MAX					6000

typedef struct {
	uint16_t para_min_val;
	uint16_t para_max_val;
} para_range_t;

#define IS_PARA_INRANGE(para, min, max)    ((para >= min) && (para <= max))

extern uint16_t    usr_conf_para[5];

ui_config_t App_UserInput_Handler	(uint8_t* key_value, uint16_t *ui_refresh);
void		App_UI_Display_Welcome	(void);

#endif /* APP_UI_H_ */
