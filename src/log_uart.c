#include "hw_bare.h"
#include "log_uart.h"


static uint8_t TxBuffer[LOG_UART_TX_MAXSIZE];
static int TxCounter = 0;
static int TxPointer = -1;

static void log_uart_enable(void) {
    /* todo */
}

static void log_uart_disable(void) {
    /* todo */
}

int log_uart_init(uint32_t baud) {
    /* todo */ try_param(baud);
    log_uart_disable();
    return 0;
}

int log_uart_send(uint8_t *buffer, uint8_t size) {
    uint8_t count = TxCounter; 
    if(count + size <= LOG_UART_TX_MAXSIZE) {
        for(int i=0; i<size; i++) {
            TxBuffer[TxCounter] = buffer[i];
            count += 1;
        }
        if(TxPointer<0) {
            TxPointer = 0;
            TxCounter = count;
            log_uart_enable();
        } else {
            TxCounter = count;
        }
        return size;
    }
    return -1;
}

int isrcb_log_uart_tx_ch(void) {
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
