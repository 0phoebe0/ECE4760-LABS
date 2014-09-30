#ifndef __AVR_CRICKETCALL_H__
#define __AVR_CRICKETCALL_H__

#include "bsp.h"
#include "drv.h"

#include "app_dds.h"
#include "app_ui.h"

typedef enum {
	SYS_DDS_CONFIG  = (0),
	SYS_DDS_RUN	    = (1),
	SYS_DDS_STOP	= (2),	
} sys_mode_t;

#endif