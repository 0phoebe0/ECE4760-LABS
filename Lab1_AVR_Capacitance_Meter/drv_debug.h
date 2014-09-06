#ifndef __DRV_DEBUG_H__
#define __DRV_DEBUG_H__

#define DRV_DEBUG_TX_BUFF_SIZE   	64
#define DRV_DEBUG_TX_BUFF_SIZE_Msk	(DRV_DEBUG_TX_BUFF_SIZE - 1)

void Drv_Debug_Printf(const char *fmt, ...);

#endif
