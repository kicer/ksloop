#ifndef _UART_H_
#define _UART_H_


#include <stdint.h>

#define UART_TX_MAXSIZE    64

extern int uart1_init(unsigned long baud);
extern int uart1_send(uint8_t *buffer, int size);
extern int uart1_flush_output(void);
extern int uart1_is_tx_empty(void);

/* for multi send, last with [wait=1] */
extern int uart1_cache_send(uint8_t *buffer, int size, uint8_t wait);

/* for interrupt
 * int uart1_tx_data(void);
 * void uart1_rx_cb(uint8_t ch)
 */


#endif /* _UART_H_ */
