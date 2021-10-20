#ifndef _HW_BARE_H_
#define _HW_BARE_H_


#include <stdint.h>

/* porting with ur mcu */
#include "hc32lx.h"

#define CORE_CLOCK_HZ           4000000

#define BSP_enableInterrupt     __enable_irq
#define BSP_SWReset             NVIC_SystemReset
#define BSP_wfi()               __asm__("WFI")

#define BIT(x)                  (1UL << (x))
#define try_param(x)            ((void)x)


#endif /* _HW_BARE_H_ */
