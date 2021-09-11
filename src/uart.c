#include "ch55x.h"
#include "uart.h"

static uint8_t TxBuffer[UART_TX_MAXSIZE];
static int TxCounter = 0;
static int TxPointer = -1;

int uart_init(uint32_t baud) {
    /* todo */ try_param(baud);
    return 0;
}

void uart_enable(void) {
    /* todo */
}

void uart_disable(void) {
    /* todo */
}

void _uart_putch(uint8_t ch) {
    /* todo */ try_param(ch);
}

int uart_flush_output(void) {
    TxCounter = 0;
    TxPointer = -1;
    return 0;
}

int uart_send(uint8_t *buffer, uint8_t size) {
    uint8_t count = TxCounter; 
    if(count + size <= UART_TX_MAXSIZE) {
        for(int i=0; i<size; i++) {
            TxBuffer[TxCounter] = buffer[i];
            count += 1;
        }
        if(TxPointer<0) {
            TxPointer = 0;
            TxCounter = count;
            uart_enable();
        } else {
            TxCounter = count;
        }
        return size;
    }
    return -1;
}

int _uart_tx_ch(void) {
    int ch = -1;
    int count = TxCounter;
    if(count > 0) {
        ch = TxBuffer[TxPointer];
        TxPointer += 1;
        if(TxPointer >= count) {
            TxCounter = 0;
            TxPointer = -1;
        }
    }
    return ch;
}
