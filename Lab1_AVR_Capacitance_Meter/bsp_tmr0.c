#include "bsp_tmr0.h"
#include <avr/interrupt.h>

void Bsp_TMR0_Init(void) {
	TCCR0A |= (0 << COM0A1) | (0 << COM0A0) |
			  (0 << COM0B1) | (0 << COM0B0) |
			  (1 << WGM01 ) | (0 << WGM00 );
//	TCCR0B =  (0 << WGM02 ) | (0 << CS02  ) | (1 << CS01) | ( 1 << CS00);
	TCCR0B =  (0 << WGM02 ) | (0 << CS02  ) | (0 << CS01) | ( 0 << CS00); /* Stop Counting Firstly */
	OCR0A = TMR0_OCR_VALUE;
	OCR0B = 0x00;

	TIMSK0 = (0 << OCIE0B) | (1 << OCIE0A) | (0 << TOIE0);
}

void __attribute((weak)) Bsp_TMR0_CTC_cbISR(void) {
	return;
}

ISR (TIMER0_COMPA_vect) {
	Bsp_TMR0_CTC_cbISR();
}
