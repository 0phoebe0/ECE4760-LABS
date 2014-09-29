#include "drv.h"

void Drv_Init(void) {
	Drv_LCD_Init();
	Drv_LED_Init();
	Drv_KeyPad_Init();
}
