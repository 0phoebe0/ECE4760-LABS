#ifndef __DRV_LED_H__
#define __DRV_LED_H__

#include <stdint.h>

#define DRV_LED_PORT   		PORTD
#define DRV_LED_PORTDDR		DDRD
#define DRV_LED_PIN_NBR		2		

void Drv_LED_Init	(void);
void Drv_LED_Toggle	(void);
void Drv_LED_On		(void);

#endif
