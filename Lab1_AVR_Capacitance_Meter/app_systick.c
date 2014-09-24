#include "app_systick.h"
#include "app_cap_meas.h"

#include "drv_debug.h"
#include <stdbool.h>

static uint8_t TaskItemIndex = 0;

SysTaskMgMt_t 	*SysTaskMgMt_Arr[SYS_TASK_COUNT];
static uint16_t 	 TaskTimeRec[SYS_TASK_COUNT];

void App_SysTaskMgMt_Init(SysTaskMgMt_t *TaskMgMtItem) {
	if (TaskItemIndex < SYS_TASK_COUNT) {
		SysTaskMgMt_Arr[TaskItemIndex] = TaskMgMtItem;
		TaskTimeRec[TaskItemIndex] = TaskMgMtItem->SysTaskTimeMs;
		TaskItemIndex ++;
	}
	else {
		TaskItemIndex = 0;
	}
}

void App_Task_Exec(void) {

	uint8_t RdyIndex = 0;

	for(RdyIndex = 0; RdyIndex < SYS_TASK_COUNT; RdyIndex++) {
		if (SysTaskMgMt_Arr[RdyIndex] != 0) {
			if (SysTaskMgMt_Arr[RdyIndex]->SysTaskFlag == true) {
				SysTaskMgMt_Arr[RdyIndex]->SysTaskFlag = false;
				if((SysTaskMgMt_Arr[RdyIndex]->SysTaskRun == true) &&
			   	   (SysTaskMgMt_Arr[RdyIndex]->SysFunction != 0))
					SysTaskMgMt_Arr[RdyIndex]->SysFunction();
			}
		}
	}
}


void Bsp_TMR0_CTC_cbISR(void) {

	uint8_t TaskScan;
	
	for(TaskScan = 0; TaskScan < SYS_TASK_COUNT; TaskScan++) {
		
		/*!> Before we access the pointer, we need to make sure it is not NULL!      */

		if (SysTaskMgMt_Arr[TaskScan] != 0) {					/*!> Very important! */
			if (SysTaskMgMt_Arr[TaskScan]->SysTaskTimeMs != 0)	
			    SysTaskMgMt_Arr[TaskScan]->SysTaskTimeMs --;
			else {
				SysTaskMgMt_Arr[TaskScan]->SysTaskFlag = true;
				SysTaskMgMt_Arr[TaskScan]->SysTaskTimeMs = TaskTimeRec[TaskScan];
			}
		}
	}
}

