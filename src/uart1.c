#include "hw_bare.h"
#include "uart1.h"


#define DEV_UARTx           M0P_UART1
#define TX_MAXSIZE    64

static uint8_t TxBuffer[TX_MAXSIZE];
static int TxCounter = 0;
static int TxPointer = -1;

static void uart1_tx_start(void) {
    DEV_UARTx->SCON_f.TXEIE = 1; /* tx empty interrupt */
}

static void uart1_tx_stop(void) {
    DEV_UARTx->SCON_f.TXEIE = 0; /* tx empty interrupt */
}

int uart1_init(uint32_t baud) {
    /* TX.BLE PA02 */
    gpio_init_afout(PA,02,1);
    gpio_pull_up(PA,02);
    /* RX.BLE PA03 */
    gpio_init_afin(PA,03,1);
    gpio_pull_up(PA,03);
    /* clock enable */
    M0P_SYSCTRL->PERI_CLKEN_f.UART1 = 1;
    /* uart init, 19200bps 8N1, mode=1,div=8,cnt=26 */
    DEV_UARTx->SCON = 0x240u;
    DEV_UARTx->SCNT = 26;
    DEV_UARTx->ICR_f.RCCF = 0; /* clear rx flag */
    DEV_UARTx->ICR_f.TCCF = 0; /* clear tx flag */
    DEV_UARTx->SCON_f.REN = 1;  /* rx/tx enable */
    DEV_UARTx->SCON_f.RCIE = 1; /* rx interrupt */
    //DEV_UARTx->SCON_f.TCIE = 1; /* tx interrupt */
    /* interrupt setting */
    NVIC_ClearPendingIRQ(UART1_IRQn); /* clear pending irq */
    NVIC_SetPriority(UART1_IRQn, 3); /* low level */
    NVIC_EnableIRQ(UART1_IRQn); /* enable irq */
    return 0;
}

int uart1_deinit(void) {
    gpio_init_in(PA,02);
    gpio_init_in(PA,03);
    gpio_pull_up(PA,02);
    gpio_pull_up(PA,03);
    NVIC_DisableIRQ(UART1_IRQn); /* enable irq */
    return 0;
}

int uart1_send(uint8_t *buffer, uint8_t size) {
    uint8_t count = TxCounter; 
    if(count + size <= TX_MAXSIZE) {
        for(int i=0; i<size; i++) {
            TxBuffer[TxCounter] = buffer[i];
            count += 1;
        }
        if(TxPointer<0) {
            TxPointer = 0;
            TxCounter = count;
            uart1_tx_start();
        } else {
            TxCounter = count;
        }
        return size;
    }
    return -1;
}

int isrcb_uart1_tx_ch(void) {
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

void __attribute__((weak)) isrcb_uart1_rx(uint8_t ch) {
    try_param(ch);
}

/*
 * Configure Tick IRQ callback function
 */
void UART1_IRQn_handler(void) {
    /* check frame error? */
    if(DEV_UARTx->ISR_f.FE) {
        DEV_UARTx->ICR_f.FECF = 0; /* clear frame-error flag */
    }
    /* data wait send? */
    if(DEV_UARTx->ISR_f.TXE) {
        int ch = isrcb_uart1_tx_ch();
        if(ch >= 0) {
            DEV_UARTx->SBUF_f.DATA = ch;
        } else {
            uart1_tx_stop();
        }
    }
    /* data receive? */
    if(DEV_UARTx->ISR_f.RC) {
        DEV_UARTx->ICR_f.RCCF = 0; /* clear rx flag */
        isrcb_uart1_rx(DEV_UARTx->SBUF_f.DATA);
    }
}
