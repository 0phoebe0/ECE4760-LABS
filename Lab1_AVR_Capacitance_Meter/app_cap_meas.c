#include "app_cap_meas.h"
#include "app_systick.h"
#include "drv_debug.h"
#include "drv_lcd.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>

static uint8_t	Cap_Measure_Stage = STATE_IDLE;
static bool		Cap_DisChargeOK	  = false;
static bool		Cap_ICP_OK		  = false;
static bool     Cap_MeasOVF		  = false;
static uint16_t Cap_ChargeTime	  = 0;
static float    Cap_TimeMs		  = 0.0f;
static float    Cap_Value		  = 0.0f;

static Cap_MeasureUnit_t	CurrUnit = MEAS_UNIT_NF;
extern SysTaskMgMt_t 		SysCapMeasure;

/* Ring Buffer for calculation and display ------------------ */
static uint16_t CapRawTime[CAP_TM_BUFF_SIZE];
static uint8_t  CapBuffHead;
static uint8_t  CapBuffTail;
static bool     CapPushFlag = true;

static LCD_Dsiplay_t		LCDDispStat = LCD_CAP_NORMAL;

void App_Cap_DisChargeDone(void) {
	Cap_DisChargeOK = true;
}

static float App_Cap_ValCalc(uint16_t Time, Cap_MeasureUnit_t Unit) {
	float CapReslt = 0.0f;

	CapReslt = ((float)((float)Time / F_CPU_MICRO)) / LN_TWO;
	
	if (Unit == MEAS_UNIT_NF) {
		CapReslt = (CapReslt * 1000.0f / (float)(R_LARGE)) ;
		Drv_Debug_Printf("CapVal: %.3f nF\r\n", CapReslt);	
	}
	else {
		CapReslt = (CapReslt / (float)(R_SMALL));
		Drv_Debug_Printf("CapVal: %.3f uF\r\n", CapReslt);
	}

	return CapReslt;
}

void App_Cap_PushBuffer(uint16_t Data) {
	uint8_t Next_Head = (CapBuffHead + 1) & CAP_TM_BUFF_SIZE_MSK;
	if (Next_Head != CapBuffTail) {
		CapRawTime[CapBuffHead] = Data;
		CapBuffHead = Next_Head;
	}
}

void App_Cap_BufferFlush(void) {
	CapBuffHead = 0;
	CapBuffTail = 0;
}

int32_t App_Cap_PopBuffer(void) {

	uint16_t PopResult = 0;

	if (CapBuffTail != CapBuffHead) {
		PopResult = CapRawTime[CapBuffTail];
		CapBuffTail = (CapBuffTail + 1) & CAP_TM_BUFF_SIZE_MSK;
	}
	else
		PopResult = (-1);

	return PopResult;
}

void App_Cap_LCD_Refresh(void) {
	
	uint16_t PopData;
	static float  CapVal = 0.0f;
	
	PopData = App_Cap_PopBuffer();	

	if (PopData != -1) {
		CapVal = App_Cap_ValCalc(PopData, CurrUnit);
	}

	Drv_LCD_GotoXY(0, 1);

	switch (LCDDispStat) {
	
		case LCD_CAP_NULL:
			Drv_LCD_Printf("DO NOT FOOL ME! ");
		break;

		case LCD_CAP_NORMAL:
			if (PopData != -1) {
				if (CurrUnit == MEAS_UNIT_NF) {
					Drv_LCD_Printf("Cap Val: %.3fnF", CapVal);
				}
				else if (CurrUnit == MEAS_UNIT_UF) {
					Drv_LCD_Printf("Cap Val: %.3fuF", CapVal);
				}
			}
		break;

		case LCD_CAP_TOOSMALL:
			Drv_LCD_Printf("It is too small!");
		break;

		case LCD_CAP_TOOLARGE:
			Drv_LCD_Printf("It is too large!");
		break;
	}
}

void App_Cap_Measure_Task(bool MeasCtrl) {
	switch (Cap_Measure_Stage) {
		case STATE_IDLE:
			if (MeasCtrl == true) {
				Cap_Measure_Stage = STATE_DISCHARGE;
			}
			break;

		case STATE_DISCHARGE:
			CAP_DISCHARGE();
			SysCapMeasure.SysTaskTimeMs = 50;
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
			Cap_MeasOVF = false;
			TMR1_CLR_TCNT1();
			TMR1_CLR_ICR1();	
			if(CurrUnit == MEAS_UNIT_NF) {
				CAP_CHARGE_R_LARGE();
			}
			else {
				CAP_CHARGE_R_SMALL();
			}
			Cap_Measure_Stage = STATE_CALC;
			break;
			
		case STATE_CALC:
			if (Cap_ICP_OK) {
				Cap_ICP_OK = false;	
				Drv_Debug_Printf("ICR1 = %x\r\n", Cap_ChargeTime);
				if (Cap_ChargeTime < 20) {
					CapPushFlag = false;
					LCDDispStat = LCD_CAP_NULL;
					Drv_Debug_Printf("No cap!\r\n");
				}
				else if ((Cap_ChargeTime >= 20 ) && (Cap_ChargeTime < 40  )) {
					CapPushFlag = false;
					LCDDispStat = LCD_CAP_TOOSMALL;
					Drv_Debug_Printf("Too Small!\r\n");
				}
				else if ((Cap_ChargeTime > 60000) || (Cap_MeasOVF == true)) {
					CapPushFlag = false;
					LCDDispStat = LCD_CAP_TOOLARGE;
					Drv_Debug_Printf("Too Large!\r\n");
				}
				else {
					LCDDispStat = LCD_CAP_NORMAL;

					if     ((Cap_ChargeTime >= 40   ) && (Cap_ChargeTime < 100  )) {
						if (CurrUnit == MEAS_UNIT_UF) {
							CurrUnit = MEAS_UNIT_NF;
							CapPushFlag = false;
							App_Cap_BufferFlush();
							Drv_Debug_Printf("Switch Small\r\n");
						}
					}
					else if ((Cap_ChargeTime >= 100  ) && (Cap_ChargeTime < 10000)) {
						Cap_TimeMs = (float)(Cap_ChargeTime / 16000.0f);
						Cap_Value = App_Cap_ValCalc(Cap_ChargeTime, CurrUnit);
					}
					else if ((Cap_ChargeTime >= 10000) && (Cap_ChargeTime < 60000)) {
						if (CurrUnit == MEAS_UNIT_NF) {
							CurrUnit = MEAS_UNIT_UF;
							CapPushFlag = false;
							App_Cap_BufferFlush();
							Drv_Debug_Printf("Switch Large\r\n");
						}
						else
							Cap_Value = App_Cap_ValCalc(Cap_ChargeTime, CurrUnit);
					}
					
					if (CapPushFlag != false)
						App_Cap_PushBuffer(Cap_ChargeTime);	
					else
						CapPushFlag = true;
				}		
	
				Cap_Measure_Stage = STATE_IDLE;
			}			
			break;

	}
}

void Bsp_TMR1_CAPT_cbISR(void) {
	Cap_ICP_OK = true;
	Cap_ChargeTime = ICR1;
}

void Bsp_TMR1_OVF_cbISR(void) {
	Cap_MeasOVF = true;
}
