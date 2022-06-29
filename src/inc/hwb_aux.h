/*
** $Id: hwb_aux.h $
** hwb_aux - hw_bare auxiliary module
**
** Arduino-style Functions:
**      enInterrupts()
**      noInterrupts()
**      swReset()
**      nop()
**      wfi()
**      nvic_irq_enable(irq,level)
**      nvic_irq_disable()
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

/* nop()
**  : nop 
*/
#define nop()                   __NOP()

/* wfi()
**  : Wait For Interrupt
*/
#define wfi()                   __WFI()

/* nvic_irq_enable()
**  : NVIC irq enable
*/
#define nvic_irq_enable(irq,level) do { \
    NVIC_ClearPendingIRQ(irq);          \
    NVIC_SetPriority(irq, level);       \
    NVIC_EnableIRQ(irq);                \
} while(0)

/* nvic_irq_disable()
**  : NVIC irq disable
*/
#define nvic_irq_disable      NVIC_DisableIRQ

/*
 ** some useful bit tricks
 */
#define BIT(x)          (1<<(x))
#define bitmask(b)      BIT(b)
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

#define hex2ch(c)       ((c)>=10?'A'+(c)-10:'0'+(c))


#endif /* _HWB_AUX_H_ */
