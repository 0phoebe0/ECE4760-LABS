#ifndef __APP_CAP_MEAS_H__
#define __APP_CAP_MEAS_H__

#include "bsp_tmr1.h"
#include <stdbool.h>

/* In this lab, R(charge) = 10K, R(discharge) = 100
   The capacitance measure range is actually defined
   by time measuring range, let's assume time measuring
   range is 6.25uS~625uS (100 - 10000 @ 16MHz of TMR1).

   Use 10K to charge: 0.901684nF < C < 90.1684nF
   Use 100 to charge: 0.090168uF < C < 9.01684uF

   Default, we use 10K to charge, if we found that the TMR1
   Input capture value is larger than 10,000, siwtch range
   automatically by charge the capacitor using 100.
 */

#define CAP_TM_BUFF_SIZE		32
#define CAP_TM_BUFF_SIZE_MSK	(CAP_TM_BUFF_SIZE - 1)	

#define LN_TWO					(float)0.693147f
#define R_LARGE					10000
#define R_SMALL					100
#define F_CPU_MICRO				(float)((float)(F_CPU) / 1000000.0f)

#define CAP_DISCHARGE() {		\
		DDRB  |=  (1 << 0);		\
		PORTB &= ~(1 << 0);		\
}


/* PORTB.2, PORTB.0 are all input when use large resistor */

#define CAP_CHARGE_R_LARGE() {	\
		DDRB  &= ~(1 << 2); 	\
		DDRB  &= ~(1 << 0);		\
}

/* PORTB.0 Push pull while PORTB.2 remains input */

#define CAP_CHARGE_R_SMALL() {	\
		DDRB  &= ~(1 << 2);		\
		DDRB  |=  (1 << 0);		\
		PORTB |=  (1 << 0);		\
}

typedef enum {
	STATE_IDLE				= (0),
	STATE_DISCHARGE 		= (1),
	STATE_DISCHARGE_CHECK 	= (2),
	STATE_CHARGE			= (3),
	STATE_CALC				= (4)
} Cap_Measure_t;

typedef enum {
	LCD_CAP_TOOLARGE		= (0),
	LCD_CAP_TOOSMALL		= (1),
	LCD_CAP_NULL			= (2),
	LCD_CAP_NORMAL			= (3)
} LCD_Dsiplay_t;

typedef enum {
	MEAS_UNIT_NF			= (0),
	MEAS_UNIT_UF			= (1)
} Cap_MeasureUnit_t;

void App_Cap_DisChargeDone	(void);
void App_Cap_Measure_Task	(bool MeasCtrl);
void App_Cap_LCD_Refresh	(void);
#endif

