#ifndef __BSP_AC_H__
#define __BSP_AC_H__

void Bsp_AC_Init(void);

#define ACOMP_ENABLE_ISR()    	ACSR |= (1 << ACIE)
#define ACOMP_DISABLE_ISR()		ACSR &= ~(1 << ACIE)

#endif
