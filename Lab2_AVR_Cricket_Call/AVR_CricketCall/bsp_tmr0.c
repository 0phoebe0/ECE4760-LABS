#include "bsp_tmr0.h"
#include <avr/interrupt.h>

void Bsp_TMR0_Init(void) {
	/* Fast PWM Mode, TOP = 0xFF	*/
	DDRB |= (1 << 3);
	DDRB |= (1 << 4);
	
	TCCR0A |= (1 << COM0A1) | (1 << COM0A0) |
			  (0 << COM0B1) | (0 << COM0B0) |
			  (1 << WGM01 ) | (1 << WGM00 );								
//	TCCR0B =  (0 << WGM02 ) | (0 << CS02  ) | (1 << CS01) | ( 1 << CS00);
	/*!> Stop Counting Firstly		*/
	TCCR0B =  (0 << WGM02 ) | (0 << CS02  ) | (0 << CS01) | ( 0 << CS00);	
	OCR0A = 128;
	OCR0B = 0x00;
	
	/*!> Enable OverFlow ISR / Disable Firstly in Initialization */
	TIMSK0 = (0 << OCIE0B) | (0 << OCIE0A) | (0 << TOIE0);					
}

void __attribute((weak)) Bsp_TMR0_CTC_cbISR(void) {
	return;
}

void __attribute((weak)) Bsp_TMR0_OVF_cbISR(void) {
	return;
}

ISR (TIMER0_COMPA_vect) {
	Bsp_TMR0_CTC_cbISR();
}

ISR (TIMER0_OVF_vect) {
	Bsp_TMR0_OVF_cbISR();
}