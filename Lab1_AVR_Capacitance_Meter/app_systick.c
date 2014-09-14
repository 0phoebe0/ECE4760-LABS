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
	int8_t RdyIndex;
	for(RdyIndex = (SYS_TASK_COUNT - 1); RdyIndex >= 0; RdyIndex--) {
		if (SysTaskMgMt_Arr[RdyIndex]->SysTaskFlag == true) {
			SysTaskMgMt_Arr[RdyIndex]->SysTaskFlag = false;
			if((SysTaskMgMt_Arr[RdyIndex]->SysTaskRun == true) &&
		   	   (SysTaskMgMt_Arr[RdyIndex]->SysFunction != 0))
				SysTaskMgMt_Arr[RdyIndex]->SysFunction();
		}
	}
}

void Bsp_TMR0_CTC_cbISR(void) {

	uint8_t TaskScan;

	for(TaskScan = 0; TaskScan < SYS_TASK_COUNT; TaskScan++) {
		if (SysTaskMgMt_Arr[TaskScan]->SysTaskTimeMs != 0)
		    SysTaskMgMt_Arr[TaskScan]->SysTaskTimeMs --;
		else {
			SysTaskMgMt_Arr[TaskScan]->SysTaskFlag = true;
			SysTaskMgMt_Arr[TaskScan]->SysTaskTimeMs = TaskTimeRec[TaskScan];
		}
	}
}

