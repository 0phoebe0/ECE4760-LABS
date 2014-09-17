#ifndef __BSP_USART0_H__
#define __BSP_USART0_H__

#include <avr/io.h>

#define F_CPU 16000000UL

#define USART0_MODE_POLLING		0
#define USART0_MODE_INTERRUPT	1
#define USART0_WORK_MODE		USART0_MODE_POLLING

#define USART0_BAUD         	9600

#define USART0_ASYNC			0
#define USART0_SYNC				1
#define USART0_MODE				USART0_ASYNC   /* Change to USART0_SYNC if needed */
#define USART0_BAUD_X2			0

#define IS_USART0_UDR_EMPTY()	(UCSR0A & (1 << UDRE0))

#if (USART0_MODE == USART0_ASYNC)
	
	#if (USART0_BAUD_X2 == 0)
		#define USART0_UBRR_VAL	(F_CPU / USART0_BAUD / 16 - 1)
	#else
		#define USART0_UBRR_VAL	(F_CPU / USART0_BAUD / 8 - 1)
	#endif

	#define USART0_UBRR_H		(uint8_t)(USART0_UBRR_VAL >> 8)
	#define USART0_UBRR_L		(uint8_t)(USART0_UBRR_VAL)

#endif

#define USART0_TX_DAT(dat)		UDR0 = dat

void Bsp_USART0_Init	(void);
void USART0_TXC_cbISR	(void);

#endif
