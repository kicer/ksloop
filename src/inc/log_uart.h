#ifndef _LOG_UART_H_
#define _LOG_UART_H_


#include <stdint.h>

#define LOG_UART_TX_MAXSIZE    64
extern int log_uart_init(uint32_t baud);
extern int log_uart_send(uint8_t *buffer, uint8_t size);

/* private for interrupt */
extern int isrcb_log_uart_tx_ch(void);


#endif /* _LOG_UART_H_ */
