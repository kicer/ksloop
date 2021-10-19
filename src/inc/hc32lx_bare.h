#ifndef _HC32Lx_BARE_H_
#define _HC32Lx_BARE_H_


#include <stdint.h>

#define _MEM_(mem_addr)      (*(volatile uint32_t *)(mem_addr))
#define _SFR_(mem_addr)      (*(volatile uint32_t *)((mem_addr)))
#define _SFR16_(mem_addr)    (*(volatile uint16_t *)((mem_addr)))

#define BIT(x)               (1UL << (x))


#endif /* _HC32Lx_BARE_H_ */
