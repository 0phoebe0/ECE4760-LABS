#ifndef __BSP_TMR0_H__
#define __BSP_TMR0_H__

#include <avr/io.h>

#define F_CPU 16000000UL

/* Common Macros and Configurations */
#define TMR0_CLR_CNTR()			TCNT0 = 0

/* Clock Select bit  */
#define TMR0_CLK_PRESCALE      	1 

/* CTC Configuration */
#define TMR0_OCR_FREQ			1000
/* Make Sure OCR value is 8-bit when TMR0 is in CTC mode */
#define TMR0_OCR_VALUE			(uint8_t)(F_CPU / TMR0_CLK_PRESCALE / 1000 - 1)

/* Remember to change this macro if the pre-scaler is not 1 */
#define TMR0_STOP_COUNT()		TCCR0B &= ~((1 << CS02  ) | (1 << CS01) | ( 1 << CS00))  /* No Clock Source */

#if		(TMR0_CLK_PRESCALE == 1   )
	#define TMR0_START_COUNT()		TCCR0B |=  ((0 << CS02  ) | (0 << CS01) | ( 1 << CS00))
#elif	(TMR0_CLK_PRESCALE == 8   )
	#define TMR0_START_COUNT()		TCCR0B |=  ((0 << CS02  ) | (1 << CS01) | ( 0 << CS00))
#elif	(TMR0_CLK_PRESCALE == 64  )
	#define TMR0_START_COUNT()		TCCR0B |=  ((0 << CS02  ) | (1 << CS01) | ( 1 << CS00))
#elif	(TMR0_CLK_PRESCALE == 256 )
	#define TMR0_START_COUNT()		TCCR0B |=  ((1 << CS02  ) | (0 << CS01) | ( 0 << CS00))
#elif	(TMR0_CLK_PRESCALE == 1024)
	#define TMR0_START_COUNT()		TCCR0B |=  ((1 << CS02  ) | (0 << CS01) | ( 1 << CS00))
/* External Clock Source Macros Waiting to be added */
#endif


/* TIMER0 works in CTC mode */
#define TMR0_DISABLE_CTC_ISR()	TIMSK0 &= ~(1 << OCIE0A)
#define TMR0_ENABLE_CTC_ISR()	TIMSK0 |=  (1 << OCIE0A)

#define TMR0_DISABLE_OVF_ISR()	TIMSK0 &= ~(1 << TOIE0)
#define TMR0_ENABLE_OVF_ISR()	TIMSK0 |=  (1 << TOIE0)


void Bsp_TMR0_Init			(void);
void Bsp_TMR0_CTC_cbISR		(void);
void Bsp_TMR0_OVF_cbISR		(void);

#endif

