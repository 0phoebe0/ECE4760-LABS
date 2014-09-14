#include "bsp_tmr2.h"
#include <avr/io.h>

void Bsp_TMR2_Init(void) {
	ASSR   = (0 << EXCLK)  | (0<<AS2);
	TCCR2A = (0 << COM2A1) | (1<<COM2A0) | (0<<COM2B1) | (0<<COM2B0) | (1<<WGM21) | (0<<WGM20);
	TCCR2B = (0 << WGM22)  | (1<<CS22) 	 | (0<<CS21)   | (0<<CS20);
	TCNT2  = 0x00;
	OCR2A  = 0xF9;
	OCR2B  = 0x00;
	DDRD   = (1<<PIND7);
}

