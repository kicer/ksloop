#include "hw_bare.h"
#include "lpuart0.h"


#define DEV_UARTx     M0P_LPUART0
#define TX_MAXSIZE    64

volatile static uint8_t TxBuffer[TX_MAXSIZE];
volatile static int TxCounter = 0;
volatile static int TxPointer = -1;

static void lpuart0_tx_start(void) {
    DEV_UARTx->SCON_f.TXEIE = 1; /* tx empty interrupt */
}

static void lpuart0_tx_stop(void) {
    DEV_UARTx->SCON_f.TXEIE = 0; /* tx empty interrupt */
}

int lpuart0_init(uint32_t baud) {
    /* TX PB10,AF4 */
    gpio_init_afout(PB,10,4);
    gpio_pull_up(PB,10);
    /* RX PB11,AF3 */
    gpio_init_afin(PB,11,3);
    gpio_pull_up(PB,11);
    /* clock enable */
    M0P_SYSCTRL->PERI_CLKEN_f.LPUART0 = 1;
#if (SLEEP_CLOCK_HZ == 38400ul)
    /* uart init, SCLK=38.4KHz, 9600bps 8N1, mode=1,div=4,cnt=1 */
    DEV_UARTx->SCON = 0;
    DEV_UARTx->SCON = 0x1C40u;
    DEV_UARTx->SCNT = 9600/baud;
#else
#error "Not support this core.clock with uartx"
#endif
    DEV_UARTx->ICR_f.RCCF = 0; /* clear rx flag */
    DEV_UARTx->ICR_f.TCCF = 0; /* clear tx flag */
    DEV_UARTx->SCON_f.REN = 1;  /* rx/tx enable */
    DEV_UARTx->SCON_f.RCIE = 1; /* rx interrupt */
    //DEV_UARTx->SCON_f.TCIE = 1; /* tx interrupt */
    /* interrupt setting */
    NVIC_ClearPendingIRQ(LPUART0_IRQn); /* clear pending irq */
    NVIC_SetPriority(LPUART0_IRQn, 1); /* high level */
    NVIC_EnableIRQ(LPUART0_IRQn); /* enable irq */
    return 0;
}

int lpuart0_deinit(void) {
    NVIC_DisableIRQ(LPUART0_IRQn); /* disable irq */
    M0P_SYSCTRL->PERI_CLKEN_f.LPUART0 = 0;
    gpio_init_in(PB,08);
    gpio_init_in(PB,09);
    gpio_floating(PB,08);
    gpio_floating(PB,09);
    return 0;
}

int lpuart0_send(uint8_t *buffer, uint8_t size) {
    uint8_t count = TxCounter; 
    if(count + size <= TX_MAXSIZE) {
        for(int i=0; i<size; i++) {
            TxBuffer[count] = buffer[i];
            count += 1;
        }
        if(TxPointer<0) {
            TxPointer = 0;
            TxCounter = count;
            lpuart0_tx_start();
        } else {
            TxCounter = count;
        }
        return size;
    }
    return -1;
}

static int isrcb_lpuart0_tx_ch(void) {
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

void __attribute__((weak)) isrcb_lpuart0_rx(uint8_t ch) {
    try_param(ch);
}

/*
 * Configure Tick IRQ callback function
 */
void LPUART0_IRQn_handler(void) {
    /* check frame error? */
    if(DEV_UARTx->ISR_f.FE) {
        DEV_UARTx->ICR_f.FECF = 0; /* clear frame-error flag */
    }
    /* data wait send? */
    if(DEV_UARTx->ISR_f.TXE) {
        int ch = isrcb_lpuart0_tx_ch();
        if(ch >= 0) {
            DEV_UARTx->SBUF_f.DATA = ch;
        } else {
            lpuart0_tx_stop();
        }
    }
    /* data receive? */
    if(DEV_UARTx->ISR_f.RC) {
        DEV_UARTx->ICR_f.RCCF = 0; /* clear rx flag */
        isrcb_lpuart0_rx(DEV_UARTx->SBUF_f.DATA);
    }
}
