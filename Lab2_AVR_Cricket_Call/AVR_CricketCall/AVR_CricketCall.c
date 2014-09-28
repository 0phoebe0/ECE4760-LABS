/*
 * AVR_CricketCall.c
 *
 * Created: 2014/9/26 23:32:38
 *  Author: Michael
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "AVR_CricketCall.h"

int main(void)
{
	Bsp_Init();
	Drv_Init();
	
//	TMR0_START_COUNT();
//	TMR0_ENABLE_CTC_ISR();
//	TMR0_ENABLE_OVF_ISR();
	
	Drv_Debug_Printf("System Initialized!\r\n");
	
	App_DDS_Init();
	App_DDS_Para_Calc();
	
	sei();
	
    while(1)
    {
        //TODO:: Please write your application code 
		App_DDS_Task_Exec();
    }
}