#include "bsp_ac.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void Bsp_AC_Init(void) {
	ACSR = 	(0 << ACD)   | (0 << ACBG) |
			(0 << ACO)   | (0 << ACI ) |
			(0 << ACIE)  | (1 << ACIC) |
			(0 << ACIS1) | (0 << ACIS0);
	ADCSRB = (0 << ACME);
//	DIDR1  = (0 << AIN0D) | (0<<AIN1D);
	DDRB  &= ~(1 << 3);
	DDRB  &= ~(1 << 2);
}

ISR (ANALOG_COMP_vect) {
	return;
}
