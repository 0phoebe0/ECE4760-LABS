#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

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
	SysCapMeasure.SysTaskTimeMs = 50;
	SysCapMeasure.SysFunction = App_Cap_DisChargeDone;
	SysCapMeasure.SysTaskRun = false;
	App_SysTaskMgMt_Init(&SysCapMeasure);

	SysLCDDisplay.SysTaskFlag = false;
	SysLCDDisplay.SysTaskTimeMs = 200;
	SysLCDDisplay.SysFunction = App_Cap_LCD_Refresh;
	SysLCDDisplay.SysTaskRun = true;
	App_SysTaskMgMt_Init(&SysLCDDisplay);
}

int main(void) {
	
	cli();

	Bsp_Init();
	Drv_Init();

	Drv_LCD_GotoXY(0, 0);
	Drv_LCD_String((void*)"CORNELL-ECE 4760", 16);
	
	Drv_LCD_GotoXY(0, 1);
	Drv_LCD_Printf("Float: %.4f", 3.65);
	
	sei();	
	TMR0_DISABLE_ISR();
	TMR1_DISABLE_CAPT_ISR();
	ACOMP_DISABLE_ISR();

	Drv_Debug_Printf("Hello, AVR!\r\n");
	Drv_Debug_Printf("ECE 4760 LAB 1!\r\n");
	Drv_Debug_Printf("Float Test: %.4f\r\n", 3.65);
	Drv_Debug_Printf("LCD Display End!\r\n");	
	
	TMR0_CLR_CNTR();
	TMR0_ENABLE_ISR();
//	TMR1_ENABLE_CAPT_ISR();
//	TMR1_ENABLE_OVF_ISR();
//	ACOMP_ENABLE_ISR();

	App_Task_Register();
	
	while(1) {
		App_Task_Exec();
		App_Cap_Measure_Task(true);
	}
}
