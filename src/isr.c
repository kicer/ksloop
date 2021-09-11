#include "ch55x.h"
#include "sys.h"
#include "uart.h"
#include "board.h"


void uart_rx_isr(void) {
    /* uart's recv callback function */
    static uint8_t _ticks = 0;
    _ticks += 1;
    if(_ticks > 8) {
        _ticks = 0;
        sys_event_trigger(EVENT_UART_RECV_PKG);
    }
}

void uart_tx_isr(void) {
    /* uart's send isr function */
    int ch = _uart_tx_ch();
    if(ch != -1) {
        _uart_putch((uint8_t)ch);
    } else {
        uart_disable();
        sys_event_trigger(EVENT_UART_SEND_PKG);
    }
}
