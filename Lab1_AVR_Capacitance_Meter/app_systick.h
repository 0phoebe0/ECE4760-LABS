#ifndef __APP_SYSTICK_H__
#define __APP_SYSTICK_H__

#include "drv_led.h"
#include "bsp_tmr0.h"
#include <stdbool.h>

#define SYS_HEARTBEAT_INTERVAL_MS		1000

#define SYS_TASK_COUNT					5

typedef void (*SysFNCT)(void);

typedef struct SysTaskMgMt{
	uint16_t SysTaskTimeMs;
	uint16_t SysTaskCntr;
	bool     SysTaskFlag;
	bool     SysTaskRun;
	SysFNCT  SysFunction;
} SysTaskMgMt_t;

void App_SysTaskMgMt_Init	(SysTaskMgMt_t *TaskMgMtItem);
void App_Task_Exec			(void);

#endif
