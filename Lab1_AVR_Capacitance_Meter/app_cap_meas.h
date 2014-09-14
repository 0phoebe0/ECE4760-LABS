#ifndef __APP_CAP_MEAS_H__
#define __APP_CAP_MEAS_H__

#include "bsp_tmr1.h"
#include <stdbool.h>

typedef enum {
	STATE_IDLE				= (0),
	STATE_DISCHARGE 		= (1),
	STATE_DISCHARGE_CHECK 	= (2),
	STATE_CHARGE			= (3),
	STATE_CALC				= (4)
} Cap_Measure_t;

void App_Cap_DisChargeDone(void);
void App_Cap_Measure_Task(bool MeasCtrl);

#endif
