/**
  ******************************************************************************
  * @file    app_cap_meas.c
  * @author  Wancheng Zhou
  * @version V1.0.0
  * @date    05-September-2014
  * @brief   This file implements capacitor measurement function and LCD result
  *			 refreshing function. Auto-Ranging function is also implemented.
  *         
  *  @verbatim
  *
  *          ===================================================================
  *                           How to use this application
  *          ===================================================================
  *
  *			 1. Include the "app_cap_meas.h" in main application files;
  *
  *          2. Specify the special discharging task that is relevant to capacitor
  *				measurement process.
  *        
  *          3. Call App_Cap_Measure_Task() in main loop.
  *          
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 CORNELL-ECE Wancheng Zhou </center></h2>
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/

#include "app_cap_meas.h"
#include "app_systick.h"
#include "drv_debug.h"
#include "drv_lcd.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>

/* Private variables ---------------------------------------------------------*/

static uint8_t				Cap_Measure_Stage 	= STATE_IDLE;
static bool					Cap_DisChargeOK		= false;
static bool					MeasCtrl			= true;
static volatile uint8_t 	Cap_OVF_Cntr 		= 0;
static volatile bool 		Cap_ICP_OK	 		= false;
static volatile bool 		Cap_MeasOVF  		= false;
static volatile uint32_t	Cap_Cmp_Null_Time	= 0;
static volatile uint32_t 	Cap_ChargeTime	  	= 0;

static Cap_MeasureUnit_t	CurrUnit = MEAS_UNIT_NF;

/* Ring Buffer for calculation and display ---------------------------------- */

static uint32_t CapRawTime[CAP_TM_BUFF_SIZE];  
static uint8_t  CapBuffHead;					/*!> Ring Buffer Header 	  */
static uint8_t  CapBuffTail;					/*!> Ring Buffer Tail		  */
static bool     CapPushFlag = true;				/*!> Ring Buffer push control */

static uint8_t DispContent[16] = 
{'C', 'O', 'R', 'N', 'E', 'L', 'L', ' ', 'E', 'C', 'E', '-', '4', '7', '6', '0'};

static LCD_Dsiplay_t		LCDDispStat = LCD_CAP_NORMAL;

extern SysTaskMgMt_t 		SysCapMeasure;
extern SysTaskMgMt_t 		SysLCDDisplay;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Calculate capacitance based on the charging time.
  * @param  Time: Capacitor charging time.
  *			Unit: Current measurement unit (uF/nF)
  * @retval None
  */

static float App_Cap_ValCalc(uint32_t Time, Cap_MeasureUnit_t Unit) {
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

/**
  * @brief  Insert data into defined ring buffer.
  * @param  Data: User-specified data to be pushed into ring buffer.
  * @retval None
  */

static void App_Cap_PushBuffer(uint32_t Data) {
	uint8_t Next_Head = (CapBuffHead + 1) & CAP_TM_BUFF_SIZE_MSK;
	if (Next_Head != CapBuffTail) {
		CapRawTime[CapBuffHead] = Data;
		CapBuffHead = Next_Head;
	}
}

/**
  * @brief  Clear Ring-buffer simply reset the head and tail.
  * @param  None
  * @retval None
  */

static void App_Cap_BufferFlush(void) {
	CapBuffHead = 0;
	CapBuffTail = 0;
}

/**
  * @brief  Get one element from specified ring-buffer.
  * @param  None
  * @retval -1 		 : Ring Buffer is empty;
  *			PopResult: Returned element from ring buffer.
  */

static int64_t App_Cap_PopBuffer(void) {

	uint32_t PopResult = 0;

	if (CapBuffTail != CapBuffHead) {
		PopResult = CapRawTime[CapBuffTail];
		CapBuffTail = (CapBuffTail + 1) & CAP_TM_BUFF_SIZE_MSK;
	}
	else
		PopResult = (-1);

	return PopResult;
}

/**
  * @brief  Refresh LCD display to show the measurement result
  * @param  None
  * @retval None
  */

void App_Cap_LCD_Refresh(void) {
	
	uint16_t 		PopData;
	static float  	CapVal = 0.0f;
	static int8_t 	AnimatePos = 0;
	static bool     OpFlag = false;
	char            DispTemp[16] = { 0 };
	uint8_t         Cursor = 0;

	memcpy(DispTemp, DispContent, 16);

	Drv_LCD_GotoXY(0, 0);
	
	if (AnimatePos == 7) {
		OpFlag = false;
	}
	if (AnimatePos == -1) {
		OpFlag = true;
	}

	for (Cursor = 0; Cursor <= AnimatePos; Cursor ++)
		DispTemp[Cursor] = ' ';
    for (Cursor = 15; Cursor >= (15 - AnimatePos); Cursor--)
		DispTemp[Cursor] = ' ';
    Drv_LCD_String(DispTemp, 16);
	
	if (OpFlag) AnimatePos ++;
	else		AnimatePos --;

	Drv_LCD_GotoXY(0, 1);
	
	PopData = App_Cap_PopBuffer();	

	if (PopData != -1) {
		CapVal = App_Cap_ValCalc(PopData, CurrUnit);
	}

	switch (LCDDispStat) {
	
		case LCD_CAP_NULL:
			Drv_LCD_Printf("DO NOT FOOL ME! ");
		break;

		case LCD_CAP_NORMAL:
			if (PopData != -1) {
				if (CurrUnit == MEAS_UNIT_NF) {
					Drv_LCD_Printf("Cap Val:%.03fnF", CapVal);
				}
				else if (CurrUnit == MEAS_UNIT_UF) {
					Drv_LCD_Printf("Cap Val:%.03fuF", CapVal);
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

/**
  * @brief  Change ISR-relevant variable that is controlled by SysTick TMR0
  * @param  None
  * @retval None
  */

void App_Cap_DisChargeDone(void) {
	Cap_DisChargeOK = true;
}

/**
  * @brief  Core Measurement task that executes in real-time.
  * @param  None
  * @retval None
  */

void App_Cap_Measure_Task(void) {

	switch (Cap_Measure_Stage) {
		
		/*!> Some extra control can be added in Idle Stage 								*/

		case STATE_IDLE:
			if (MeasCtrl == true) {						
				Cap_Measure_Stage = STATE_DISCHARGE;	/*!> Switch to next stage 		*/
			}
			break;
		
		/*!> This stage simply instantiate the discharge task 							*/

		case STATE_DISCHARGE:
			CAP_DISCHARGE();
			SysCapMeasure.SysTaskTimeMs = 50;			/*!> Parameter initialization 	*/
			SysCapMeasure.SysTaskRun = true;			/*!> Open discharge task		*/
			SysCapMeasure.SysTaskFlag = false;
			Cap_DisChargeOK = false;			
			Cap_Measure_Stage = STATE_DISCHARGE_CHECK;	/*!> Switch to next stage 		*/
			break;
		
		/*!> This stage simply check whether the discharge process is over or not 		*/

		case STATE_DISCHARGE_CHECK:
			if (Cap_DisChargeOK) {
				Drv_Debug_Printf("Discharge OK\r\n");
				Cap_DisChargeOK = false;				/*!> Reset discharge results	*/
				SysCapMeasure.SysTaskRun = false;		/*!> Close discharge task 		*/
				Cap_Measure_Stage = STATE_CHARGE;		/*!> Switch to next stage 		*/
			}	
			break;
		
		/*!> This stage charge the capacitor according to current measurement range		*/

		case STATE_CHARGE:
			Drv_Debug_Printf("Start Charge\r\n");
				
			TMR1_CLR_TCNT1();							/*!> Clear TMR1 Counter			*/
			TMR1_CLR_ICR1();							/*!> Clear TMR1 ICP Register	*/
			TMR1_START_COUNT();							/*!> Start TMR1 to count		*/

			Cap_ICP_OK 		= false;					/*!> Reset relevant parameters  */
			Cap_MeasOVF 	= false;
			Cap_OVF_Cntr 	= 0;

			if(CurrUnit == MEAS_UNIT_NF) {				/*!> Judge measurement range	*/
				CAP_CHARGE_R_LARGE();					/*!> Use large resistor   		*/
			}
			else {										/*!> Measure large capacitor	*/
				CAP_CHARGE_R_SMALL();					/*!> Use small resistor   		*/
			}

			Cap_Measure_Stage = STATE_CALC;
			break;
		
		/*!> Calculation stage including Auto-Ranging Methodology						*/

		case STATE_CALC:
			if (Cap_ICP_OK || Cap_MeasOVF) {
				Cap_ICP_OK = false;
				Drv_Debug_Printf("ICP Value:%x\r\n", Cap_ChargeTime);

				/*!>  */

				if (CurrUnit == MEAS_UNIT_NF)  	Cap_Cmp_Null_Time = CAP_NULL_ICP_LARGE_RES;
				else							Cap_Cmp_Null_Time = CAP_NULL_ICP_SMALL_RES;
				
				/*!> Get Capacitor charging time with overflow counter and ICP value  	*/

				if (Cap_MeasOVF)
					Cap_ChargeTime = Cap_ChargeTime + ((uint32_t)Cap_OVF_Cntr << 16);
				
				/*!> Different judgment in order to change LCD display and switch range */

				if (Cap_ChargeTime <= Cap_Cmp_Null_Time) {
					App_Cap_BufferFlush();				/*!> Flush ring buffer   		*/
					CapPushFlag = false;
					LCDDispStat = LCD_CAP_NULL;			/*!> No capacitor presented 	*/
				}
				else if ((Cap_ChargeTime >  Cap_Cmp_Null_Time) &&
						 (Cap_ChargeTime <= CAP_ICP_NORMAL_MIN)) {
					App_Cap_BufferFlush();
					CapPushFlag = false;
					if (CurrUnit == MEAS_UNIT_NF) {		/*!> If current unit is NF		*/
						LCDDispStat = LCD_CAP_TOOSMALL;	/*!> Less than smallest value	*/
					}
					else {								/*!> Current unit is UF			*/
						CurrUnit = MEAS_UNIT_NF;		/*!> Switch measurement range   */
					}
				}
				else if ((Cap_ChargeTime >  CAP_ICP_NORMAL_MIN) &&
						 (Cap_ChargeTime <= CAP_ICP_NORMAL_MAX)) {
					CapPushFlag = true;					/*!> Normal measurement   		*/
					LCDDispStat = LCD_CAP_NORMAL;
				}
				else if ((Cap_ChargeTime >  CAP_ICP_NORMAL_MAX)) {
					App_Cap_BufferFlush();
					CapPushFlag = false;
					if (CurrUnit == MEAS_UNIT_UF) {		/*!> If current unit is UF		*/
						LCDDispStat = LCD_CAP_TOOLARGE;	/*!> Large than largest value	*/
					}
					else {
						CurrUnit = MEAS_UNIT_UF;
					}
				}

				if (CapPushFlag)
					App_Cap_PushBuffer(Cap_ChargeTime);	/*!> Store the result if needed	*/

				Cap_Measure_Stage = STATE_IDLE;			/*!> Reset status to IDLE		*/

				SysLCDDisplay.SysTaskRun = true;		/*!> Run LCD Display Task		*/
			}			
			break;
	}
}

/**
  * @brief  TMR1 Input-Capture ISR call-back function.
  * @param  None
  * @retval None
  */

void Bsp_TMR1_CAPT_cbISR(void) {
	Cap_ICP_OK = true;
	Cap_ChargeTime = ICR1;
	TMR1_STOP_COUNT();
}

/**
  * @brief  TMR1 OverFlow ISR call-back function
  * @param  None
  * @retval None
  */

void Bsp_TMR1_OVF_cbISR(void) {
	Cap_OVF_Cntr ++;
	Cap_MeasOVF = true;
}

/************************ (C) COPYRIGHT Cornell ECE4760 ********END OF FILE****/
