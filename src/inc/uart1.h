#ifndef _UART1_H_
#define _UART1_H_


#include <stdint.h>

extern int uart1_init(uint32_t baud);
extern int uart1_send(uint8_t *buffer, uint8_t size);
extern int uart1_deinit(void);

/* interrupt callback weak function */
//void __attribute__((weak)) isrcb_uart1_rx(uint8_t ch);


#endif /* _UART1_H_ */
