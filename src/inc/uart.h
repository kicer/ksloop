#ifndef _UART_H_
#define _UART_H_


#include <stdint.h>

#define UART_TX_MAXSIZE    64

extern int uart_init(uint32_t baud);
extern int uart_send(uint8_t *buffer, uint8_t size);

extern void uart_enable(void);
extern void uart_disable(void);
extern int uart_flush_output(void);

/* private for interrupt */
extern int _uart_tx_ch(void);
extern void _uart_putch(uint8_t ch);


#endif /* _UART_H_ */
