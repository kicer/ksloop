#ifndef _LPUART0_H_
#define _LPUART0_H_


#include <stdint.h>

extern int lpuart0_init(uint32_t baud);
extern int lpuart0_send(uint8_t *buffer, uint8_t size);
extern int lpuart0_deinit(void);

/* interrupt callback weak function */
//void __attribute__((weak)) isrcb_lpuart0_rx(uint8_t ch);


#endif /* _LPUART0_H_ */
