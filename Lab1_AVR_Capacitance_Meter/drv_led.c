#include "drv_led.h"
#include "drv_debug.h"

#include "bsp_tmr0.h"
#include "bsp_tmr1.h"

void Drv_LED_Init(void) {
	DRV_LED_PORTDDR |= (1 << DRV_LED_PIN_NBR );
	DRV_LED_PORT    |= (1 << DRV_LED_PIN_NBR );
}

void Drv_LED_Toggle(void) {
	DRV_LED_PORT ^= (1 << DRV_LED_PIN_NBR);
	#if (0)
	(DRV_LED_PORT &   (1 << DRV_LED_PIN_NBR)) ? \
	(DRV_LED_PORT &= ~(1 << DRV_LED_PIN_NBR)) : \
	(DRV_LED_PORT |=  (1 << DRV_LED_PIN_NBR));
	#endif
}





