#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "app_main.h"

SysTaskMgMt_t SysLEDIndicator;
SysTaskMgMt_t SysCapMeasure;
SysTaskMgMt_t SysLCDDisplay;

void App_Task_Register(void) {
	SysLEDIndicator.SysTaskFlag = false;
	SysLEDIndicator.SysTaskTimeMs = 1000;
	SysLEDIndicator.SysFunction = Drv_LED_Toggle;
	SysLEDIndicator.SysTaskRun = true;
	App_SysTaskMgMt_Init(&SysLEDIndicator);
	
	SysCapMeasure.SysTaskFlag = false;
	SysCapMeasure.SysTaskTimeMs = 100;
	SysCapMeasure.SysFunction = App_Cap_DisChargeDone;
	SysCapMeasure.SysTaskRun = false;
	App_SysTaskMgMt_Init(&SysCapMeasure);

	SysLCDDisplay.SysTaskFlag = false;
	SysLCDDisplay.SysTaskTimeMs = 200;
	SysLCDDisplay.SysFunction = App_Cap_LCD_Refresh;
	SysLCDDisplay.SysTaskRun = false;
	App_SysTaskMgMt_Init(&SysLCDDisplay);
}

int main(void) {
	
	_delay_ms(100);	

	Bsp_Init();
	Drv_Init();
	
	Drv_LCD_Clear();
	Drv_LCD_GotoXY(0, 0);
	Drv_LCD_String((void*)"CORNELL ECE-4760", 16);

	sei();
//	cli();

	Drv_Debug_Printf("Hello, AVR!\r\n");
	
	TMR1_ENABLE_CAPT_ISR();
	TMR1_ENABLE_OVF_ISR();
//	ACOMP_ENABLE_ISR();

	App_Task_Register();

	TMR0_START_COUNT();
	TMR0_CLR_CNTR();
	TMR0_ENABLE_ISR();
	
	while(1) {
		App_Task_Exec();
		App_Cap_Measure_Task();
	}
}
