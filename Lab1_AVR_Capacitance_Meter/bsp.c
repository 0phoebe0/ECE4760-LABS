#include "bsp.h"

void Bsp_Init(void) {
	Bsp_USART0_Init	();
	Bsp_TMR0_Init	();
	Bsp_TMR1_Init	();
//	Bsp_TMR2_Init	();
	Bsp_AC_Init		();
}

