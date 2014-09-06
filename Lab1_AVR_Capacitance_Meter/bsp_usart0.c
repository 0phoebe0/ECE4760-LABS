#include "bsp_usart0.h"
#include <avr/interrupt.h>

void Bsp_USART0_Init(void) {

	UBRR0H = USART0_UBRR_H;
	UBRR0L = USART0_UBRR_L;

#if (USART0_BAUD_X2 == 0)
	UCSR0A |= (0 << U2X0);
#else
	UCSR0A |= (1 << U2X0);
#endif
	
	UCSR0B &= ~ (1 << UCSZ02 );
	UCSR0B |=  ((1 << RXCIE0 ) | (1 << TXCIE0 ) | 
			    (1 << RXEN0  ) | (1 << TXEN0  ) );  /*!> 0xD8: RX Interrupt & TX Interrupt 			*/

	UCSR0C &= ~((1 << UMSEL01) | (1 << UMSEL00) |
				(1 << UPM01  ) | (1 << UPM00  ) |
				(1 << USBS0  ) | (1 << UCPOL0 ));
	UCSR0C |=  ((1 << UCSZ01 ) | (1 << UCSZ00 ));	/*!> 0x06: Async Mode, 8-bit Data, 1-bit STO 	*/

	UDR0    = 0;
}

ISR(USART0_TX_vect) {
	USART0_TXC_cbISR();
}
