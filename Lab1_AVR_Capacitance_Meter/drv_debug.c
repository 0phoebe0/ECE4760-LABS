#include "drv_debug.h"
#include "bsp_usart0.h"

#include <stdio.h>
#include <stdarg.h>

static uint8_t Drv_Tx_Buff[DRV_DEBUG_TX_BUFF_SIZE];
static volatile uint8_t Drv_Tx_Head;
static volatile uint8_t Drv_Tx_Tail;

static void Drv_PutChar(uint8_t Data);

void Drv_Debug_Printf(const char *fmt, ...) {
	char DataBuff[64] = { 0 };
	va_list ArgPtr;
	int8_t  ChCnt;
	int8_t  ChNbr = 0;

	va_start(ArgPtr, fmt);
	ChCnt = vsnprintf(DataBuff, 64, fmt, ArgPtr);
	va_end(ArgPtr);

	do {
		Drv_PutChar(DataBuff[ChNbr]);
		ChNbr ++;
	} while(ChNbr < ChCnt);
}

static void Drv_PutChar(uint8_t Data) {
	
	uint8_t Next_Head;

	Next_Head = (Drv_Tx_Head + 1) & DRV_DEBUG_TX_BUFF_SIZE_Msk;

	if ((Drv_Tx_Tail == Drv_Tx_Head) && USART0_UDR_EMPTY) {
		UDR0 = Data;
	}
	else {
		if(Next_Head != Drv_Tx_Tail) {
			Drv_Tx_Buff[Drv_Tx_Head] = Data;
			Drv_Tx_Head = Next_Head;
		}
	}
}

void USART0_TXC_cbISR(void) {
	if (Drv_Tx_Tail != Drv_Tx_Head) {
		UDR0 = Drv_Tx_Buff[Drv_Tx_Tail];
		Drv_Tx_Tail = (Drv_Tx_Tail + 1) & DRV_DEBUG_TX_BUFF_SIZE_Msk;
	}
}
