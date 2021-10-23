#ifndef _LPUART1_H_
#define _LPUART1_H_


#include <stdint.h>

extern int lpuart1_init(uint32_t baud);
extern int lpuart1_send(uint8_t *buffer, uint8_t size);
extern int lpuart1_deinit(void);

/* interrupt callback weak function */
//void __attribute__((weak)) isrcb_lpuart1_rx(uint8_t ch);


#endif /* _LPUART1_H_ */
