#include "app_dds.h"
#include "drv_debug.h"
#include <math.h>
#include <stdbool.h>

static uint16_t				SysIndCntr;							/*!> System Running Indicator	*/
static int8_t				sineTable[256];						/*!> sine wave local table		*/
static uint8_t				rampTable[256];						/*!> Ramp up / down table		*/
static syllableplay_t		dds_play_status = SYLLABLE_INIT;	/*!> State Machine Variable		*/

static volatile uint32_t	dds_accumulator;					/*!> DDS accumulator			*/
static volatile uint8_t		dds_accum_high8;					/*!> Adjustment to get sine val */

static volatile uint8_t		rampt_index;
static volatile uint16_t	sample_cntr;

static volatile uint32_t	syllable_tmr_count;
static volatile bool		dds_synthesis_flag = true;

/* All units must be in millisecond and frequency in Hertz unit! */

									/*!>	Description				   Unit			*/
uint16_t chirp_rept_interval;		/*!> chirp repeat interval:			ms			*/
uint8_t  syllable_number;			/*!> syllable number in a chirp					*/
uint8_t  syllable_duration;			/*!> syllable duration (length)		ms			*/
uint8_t  syllable_rept_interval;	/*!> syllable repeat interval       ms			*/
uint16_t burst_freq;				/*!> burst frequency				Hz			*/

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
		sineTable[sineIndex] = (uint8_t)(127.0f * sin(6.283f * ((float)sineIndex) / 256.0f));
		
		/*!> Please take care of the range! should not exceed 128! */
		rampTable[sineIndex] = (sineIndex >> 1);
	}
}

void App_DDS_Para_Calc(void) {
	
	/*!> Following parameters should be got from user input! */
	chirp_rept_interval		= 1500;		
	syllable_number			= 5;
	syllable_duration		= 20;
	syllable_rept_interval	= 50;
	burst_freq				= 2500;
	
	/*!> Must do some kind of verification to check whether the parameters are right */
	
	dds_increment = (DDS32_INCREMENT / 1000) * burst_freq;
	sines_per_syllable = (uint16_t)(syllable_duration * burst_freq / 1000);  /* Do not need control */
	ramp_down_sta = (syllable_duration * 62.5) -  SINE_RAMP_UP_END - SINE_RAMP_UP_END;
	ramp_down_sto = ramp_down_sta + SINE_RAMP_UP_END;
	
	Drv_Debug_Printf("dds_increment: %ld, ramp_down_sta: %d, ramp_down_sto: %d\r\n", dds_increment, ramp_down_sta, ramp_down_sto);
} 

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