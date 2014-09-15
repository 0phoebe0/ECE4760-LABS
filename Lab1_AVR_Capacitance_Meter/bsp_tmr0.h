#ifndef __BSP_TMR0_H__
#define __BSP_TMR0_H__

#include <avr/io.h>

#define F_CPU 16000000UL

/* Clock Select bit  */
#define TMR0_CLK_PRESCALE      	64 
#define TMR0_OCR_FREQ			1000
#define TMR0_OCR_VALUE			(F_CPU / TMR0_CLK_PRESCALE / 1000 - 1)

#define TMR0_DISABLE_ISR()		TIMSK0 &= ~(1 << OCIE0A)
#define TMR0_ENABLE_ISR()		TIMSK0 |= (1 << OCIE0A )
#define TMR0_CLR_CNTR()			TCNT0 = 0

void Bsp_TMR0_Init			(void);
void Bsp_TMR0_CTC_cbISR		(void);

#endif

