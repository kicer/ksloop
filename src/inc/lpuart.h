#ifndef _LPUART_H_
#define _LPUART_H_


#include <stdint.h>

extern int lpuart_init(uint32_t baud);
extern int lpuart_send(uint8_t *buffer, uint8_t size);
extern int lpuart_deinit(void);

/* interrupt callback weak function */
//void __attribute__((weak)) isrcb_lpuart_rx(uint8_t ch);


#endif /* _LPUART_H_ */
