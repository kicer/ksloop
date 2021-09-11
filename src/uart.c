#include "ch55x.h"
#include "uart.h"

static uint8_t TxBuffer[UART_TX_MAXSIZE];
static int TxCounter = 0;
static int TxPointer = 0;

#define UART1_TX_EMPTY()  (TxCounter==0)

static void bsp_uart1_init(void) {
}

static void bsp_uart1_enable(void) {
    //UART1_ITConfig(UART1_IT_TXE, ENABLE);
}

int uart1_init(uint32_t baud) {
    try_param(baud);
    bsp_uart1_init();
    return 0;
}

int uart1_is_tx_empty(void) {
    return UART1_TX_EMPTY();
}

int uart1_flush_output(void) {
    TxCounter = 0;
    TxPointer = 0;
    return 0;
}

int uart1_cache_send(uint8_t *buffer, int size, uint8_t wait) {
    if(TxCounter + size <= UART_TX_MAXSIZE) {
        for(int i=0; i<size; i++) {
            TxBuffer[TxCounter] = buffer[i];
            TxCounter += 1;
        }
        if(!wait) {
            TxPointer = 0;
            bsp_uart1_enable();
        }
        return size;
    }
    return -1;
}

int uart1_send(uint8_t *buffer, int size) {
    if(UART1_TX_EMPTY() && (size > 0)) {
        return uart1_cache_send(buffer, size, 0);
    }
    return -1;
}

int uart1_tx_data(void) {
    int ch = -1;
    int count = TxCounter;
    if(count > 0) {
        ch = TxBuffer[TxPointer];
        TxPointer += 1;
        if(TxPointer >= count) {
            TxCounter = 0;
            TxPointer = 0;
        }
    }
    return ch;
}
