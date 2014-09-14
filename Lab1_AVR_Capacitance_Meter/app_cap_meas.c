#include "app_cap_meas.h"
#include "app_systick.h"
#include "drv_debug.h"

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

static uint8_t	Cap_Measure_Stage = STATE_IDLE;
static bool		Cap_DisChargeOK	  = false;
static bool		Cap_ICP_OK		  = false;
static uint16_t Cap_ChargeTime	  = 0;
static float    Cap_TimeMs		  = 0.0f;

extern SysTaskMgMt_t SysCapMeasure;

void App_Cap_DisChargeDone(void) {
	Cap_DisChargeOK = true;
}

void App_Cap_Measure_Task(bool MeasCtrl) {
	switch (Cap_Measure_Stage) {
		case STATE_IDLE:
			if (MeasCtrl == true) {
				Cap_Measure_Stage = STATE_DISCHARGE;
			}
			break;

		case STATE_DISCHARGE:
			DDRB |=   (1 << 2);
			PORTB &= ~(1 << 2);
			SysCapMeasure.SysTaskTimeMs = 1000;
			SysCapMeasure.SysTaskRun = true;
			SysCapMeasure.SysTaskFlag = false;
			Cap_DisChargeOK = false;			
			Cap_Measure_Stage = STATE_DISCHARGE_CHECK;
			break;
		
		case STATE_DISCHARGE_CHECK:
			if (Cap_DisChargeOK) {
				Drv_Debug_Printf("Discharge OK\r\n");
				Cap_DisChargeOK = false;
				SysCapMeasure.SysTaskRun = false;
				Cap_Measure_Stage = STATE_CHARGE;
			}
			break;

		case STATE_CHARGE:
			Drv_Debug_Printf("Start Charge\r\n");	
			Cap_ICP_OK = false;
			TMR1_CLR_TCNT1();
			TMR1_CLR_ICR1();
			DDRB  &= ~(1 << 2);
			PORTB &= ~(1 << 2);
			Cap_Measure_Stage = STATE_CALC;
			break;
			
		case STATE_CALC:
			if (Cap_ICP_OK) {
				Cap_ICP_OK = false;	
				if (Cap_ChargeTime > 0x10) {
					Cap_TimeMs = (float)(Cap_ChargeTime / 16000.0f);
					Drv_Debug_Printf("ICR1 = %x, Capture: %.4f\r\n", Cap_ChargeTime, Cap_TimeMs);
				}
				else
					Drv_Debug_Printf("No Capacitor!\r\n");	
				
				Cap_Measure_Stage = STATE_IDLE;
			}			
			break;

	}
}

void Bsp_TMR1_CAPT_cbISR(void) {
	Cap_ICP_OK = true;
	Cap_ChargeTime = ICR1;
}
