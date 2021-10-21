#ifndef _UART0_H_
#define _UART0_H_


#include <stdint.h>

extern int uart0_init(uint32_t baud);
extern int uart0_send(uint8_t *buffer, uint8_t size);
extern int uart0_deinit(void);

/* interrupt callback weak function */
//void __attribute__((weak)) isrcb_uart0_rx(uint8_t ch);


#endif /* _UART0_H_ */
