#ifndef __APP_DDS_H__
#define __APP_DDS_H__

#include "drv_led.h"
#include "bsp_tmr0.h"

#include "drv_lcd.h"

#include <stdbool.h>

/* For a 32-bit DDS accumulator, 16000000Hz / 256 = 62.5KHz
   
   fo = M * fc / 2^n
   M:  amount of the phase accumulator is incremented each clock cycle;
   fc: clock frequency;
   fo: output clock frequency.
   
   n = 32, fo = 1KHz, fc = 62.5KHz
   
   increment_adj = 2^32 * 256 * Freq / 16000000 = 68719 * Freq
   Freq = 1000Hz, increment_adj = 68719000 */
#define DDS32_INCREMENT			68719000UL

#define SINE_RAMP_UP_END		250		/*  4 Complete sine waves: 4  ms */
#define SINE_RAMP_DOWN_START	625     /* 10 Complete sine waves: 40 ms */
#define SINE_RAMP_DOWN_END		875		/* 14 Complete sine waves: 56 ms */

typedef enum {
	SYLLABLE_INIT = (0),
	SYLLABLE_PREP = (1),
	SYLLABLE_PLAY = (2),
	SYLLABLE_OVER = (3),
	SYLLABLE_STOP = (4)
} syllableplay_t;

extern bool	 dds_play_ctrl;

void App_DDS_Init(void);
void App_DDS_Para_Calc(uint16_t *para_arr);
void App_DDS_Task_Exec(void);

#endif