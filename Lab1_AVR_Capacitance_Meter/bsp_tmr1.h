#ifndef __BSP_TMR1_H__
#define __BSP_TMR1_H__

#include <avr/io.h>

#define TMR1_ENABLE_CAPT_ISR()	TIMSK1 |=  (1 << ICIE1)
#define TMR1_ENABLE_OVF_ISR()	TIMSK1 |=  (1 << TOIE1)
#define TMR1_DISABLE_CAPT_ISR()	TIMSK1 &= ~(1 << ICIE1)
#define TMR1_DISABLE_OVF_ISR()	TIMSK1 |=  (1 << TOIE1)

#define TMR1_CLR_TCNT1()		TCNT1	= 0
#define TMR1_CLR_ICR1()			ICR1	= 0

void Bsp_TMR1_Init				(void);
void Bsp_TMR1_CAPT_cbISR		(void);
void Bsp_TMR1_OVF_cbISR			(void);

#endif

