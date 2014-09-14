#include "bsp_tmr1.h"
#include <avr/interrupt.h>

void Bsp_TMR1_Init(void) {
	TCCR1B = (1 << ICES1) | (1 << CS10);	/* No prescaling, full CPU clock 	*/
//	TIMSK1 = (1 << ICIE1);					/* Enable Input Capture 			*/
}

void __attribute__((weak)) Bsp_TMR1_CAPT_cbISR( void ) {
	return;
}

ISR (TIMER1_CAPT_vect) {
	Bsp_TMR1_CAPT_cbISR();
}



