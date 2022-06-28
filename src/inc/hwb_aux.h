/*
** $Id: hwb_aux.h $
** hwb_aux - hw_bare auxiliary module
**
** Arduino-style Functions:
**      enInterrupts()
**      noInterrupts()
**      swReset()
*/

#ifndef _HWB_AUX_H_
#define _HWB_AUX_H_


#include <stdint.h>

#include "hc32lx.h"

/* enInterrupts()
**  : enable global interrupt
*/
#define enInterrupts()          __enable_irq()

/* noInterrupts()
**  : disable global interrupt
*/
#define noInterrupts()          __disable_irq()

/* swReset()
**  : software reset
*/
#define swReset()               NVIC_SystemReset()

#define BIT(x)                  (1ul << (x))
#define try_param(x)            ((void)x)
#define hex2ch(c)               ((c)>=10?'A'+(c)-10:'0'+(c))

#define BSP_enableInterrupt     __enable_irq
#define BSP_SWReset             NVIC_SystemReset
#define BSP_wfi()               __asm__("WFI")

/*
 ** some useful bit tricks
 */
#define bitmask(b)      (1<<(b))
#define testbits(x,m)   ((x) & (m))

#ifndef cast
#define cast(t,exp)     ((t)(exp))
#endif

#define not_used(v)     ((void)(v))

/* internal assertions for in-house debugging */
#ifdef hwb_assert
#define check_exp(c,e)      (hwb_assert(c),(e))
#else
#define hwb_assert(c)       ((void)0)
#define check_exp(c,e)      (e)
#endif

#ifndef hwb_abort
#define hwb_abort()     while(1)
#endif

/* ################### Helper Action ######################## */
#define nvic_irq_enable(irq,level) do { \
    NVIC_ClearPendingIRQ(irq);          \
    NVIC_SetPriority(irq, level);       \
    NVIC_EnableIRQ(irq);                \
} while(0)

#define nvic_irq_disable      NVIC_DisableIRQ

extern void hw_delay_init(void);
extern void hw_delay_us(uint32_t us);
extern int64_t hw_uptime(void);

#endif /* _HWB_AUX_H_ */
