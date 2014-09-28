#include "app_dds.h"
#include "drv_debug.h"
#include <math.h>
#include <stdbool.h>

static uint16_t		SysIndCntr;
static int8_t		sineTable[256];
static uint8_t		rampTable[256];

volatile uint32_t	dds_accumulator;
volatile uint8_t	dds_accum_high8;

volatile uint8_t    rampt_index;
volatile uint16_t   sample_cntr;

volatile uint32_t   syllable_tmr_count;
volatile uint32_t   chirp_tmr_count;

/* All units must be in millisecond and frequency in Hertz unit! */
uint16_t chirp_rept_interval;
uint8_t  syllable_number;
uint8_t  syllable_duration;			/*!> sine wave quantity in one syllable	*/
uint8_t  syllable_rept_interval;	
uint16_t burst_freq;				/*!> calculate DDS Increment			*/

uint32_t dds_increment;
uint16_t sines_per_syllable;

uint16_t ramp_down_sta;
uint16_t ramp_down_sto;

bool	 dds_play_ctrl = true;

void App_DDS_Init(void) {
	
	uint16_t sineIndex = 0;
	
	for (sineIndex = 0; sineIndex < 256; sineIndex++)
	{
		/*!> Generate sinewave table which contains 256 sample points */
		sineTable[sineIndex] = (uint8_t)(127.0f * sin(6.283 * ((float)sineIndex) / 256.0f));
		
		/*!> Please take care of the range! should not exceed 128! */
		rampTable[sineIndex] = (sineIndex >> 1);
	}
}

void App_DDS_Para_Calc(void) {
	
	chirp_rept_interval = 1500;
	syllable_number = 10;
	syllable_duration = 10;
	syllable_rept_interval = 50;
	
	/*!> Must do some kind of verification to check whether the parameters are right */
	
	burst_freq = 3000;
	
	dds_increment = (DDS32_INCREMENT / 1000) * burst_freq;
	sines_per_syllable = (uint16_t)(syllable_duration * burst_freq / 1000);  /* Do not need control */
	ramp_down_sta = (syllable_duration * 62.5) -  SINE_RAMP_UP_END - SINE_RAMP_UP_END;
	ramp_down_sto = ramp_down_sta + SINE_RAMP_UP_END;
	
	Drv_Debug_Printf("dds_increment: %ld, ramp_down_sta: %d, ramp_down_sto: %d\r\n", dds_increment, ramp_down_sta, ramp_down_sto);
} 

typedef enum {
	SYLLABLE_INIT = (0),
	SYLLABLE_PREP = (1),
	SYLLABLE_PLAY = (2),
	SYLLABLE_OVER = (3),
	SYLLABLE_STOP = (4)
} syllableplay_t;

static syllableplay_t dds_play_status = SYLLABLE_INIT;
volatile bool dds_synthesis_flag = true;

void App_DDS_PlaySyllable(void) {
	
	static uint8_t syllable_play_cntr;
	
	switch(dds_play_status) {
		
		case SYLLABLE_INIT:
			syllable_tmr_count = 0;
			syllable_play_cntr = 0;
			dds_accumulator = 0;
			dds_synthesis_flag = false;
			TMR0_START_COUNT();
			TMR0_ENABLE_OVF_ISR();
			dds_play_status = SYLLABLE_PREP;
		break;
		
		case SYLLABLE_PREP:
			dds_synthesis_flag = true;
//			TMR0_CLR_CNTR();
			dds_play_status = SYLLABLE_PLAY;
		break;
		
		case SYLLABLE_PLAY:
			if (syllable_tmr_count >= ((syllable_duration + \
									   (syllable_rept_interval) * syllable_play_cntr) * 62.5)) {
				dds_synthesis_flag = false;
				dds_play_status = SYLLABLE_OVER;
			}
		break;
		
		case SYLLABLE_OVER:
			if (syllable_tmr_count >= (syllable_rept_interval * (syllable_play_cntr + 1) * 62.5)) {
				syllable_play_cntr ++;
				if (syllable_play_cntr < syllable_number)
					dds_play_status = SYLLABLE_PREP;
				else {
					dds_play_status = SYLLABLE_STOP;
				}
			}
		break;
		
		case SYLLABLE_STOP:
			if (syllable_tmr_count >= (chirp_rept_interval * 62.5)) {
				dds_play_status = SYLLABLE_INIT;
			}
		break;
	}
	
}

void App_DDS_Task_Exec(void) {
	
	if (dds_play_ctrl) {
		App_DDS_PlaySyllable();
	}
	else {
		TMR0_STOP_COUNT();		/*!> Just for testing purpose! */
	}
}

void Bsp_TMR0_CTC_cbISR(void) {
	return;
}

/*!> TMR0 is initialized @ 16MHz, Fast PWM Mode, 16000000 / 256 = 62500 */
void Bsp_TMR0_OVF_cbISR(void) {
	
	if (dds_synthesis_flag) {
		dds_accumulator += dds_increment;
		dds_accum_high8 =  (uint8_t)(dds_accumulator >> 24);
		
		OCR0A = 128 + (uint8_t)((uint16_t)( sineTable[dds_accum_high8] * \
		rampTable[rampt_index]) >> 7 );
		
		sample_cntr ++;
		
		if		(sample_cntr <= SINE_RAMP_UP_END)
			rampt_index++;
		else if (sample_cntr > ramp_down_sta && sample_cntr <= ramp_down_sto)
			rampt_index = 255;
		else if (sample_cntr > ramp_down_sto)
			rampt_index--;
	}
	else
		OCR0A = 128;
	
	syllable_tmr_count ++;
	
	if (++SysIndCntr == 62500) {
		Drv_LED_Toggle();
		SysIndCntr = 0;
	}
}