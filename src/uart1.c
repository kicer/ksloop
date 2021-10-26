#include "hw_bare.h"
#include "uart1.h"


#define DEV_UARTx     M0P_UART1
#define TX_MAXSIZE    64

volatile static uint8_t TxBuffer[TX_MAXSIZE];
volatile static int TxCounter = 0;
volatile static int TxPointer = -1;

static void uart1_tx_start(void) {
    DEV_UARTx->SCON_f.TXEIE = 1; /* tx empty interrupt */
}

static void uart1_tx_stop(void) {
    DEV_UARTx->SCON_f.TXEIE = 0; /* tx empty interrupt */
}

int uart1_init(uint32_t baud) {
    /* TX PA02,AF1 */
    gpio_init_afout(PA,02,1);
    gpio_pull_up(PA,02);
    /* RX PA03,AF1 */
    gpio_init_afin(PA,03,1);
    gpio_pull_up(PA,03);
    /* clock enable */
    M0P_SYSCTRL->PERI_CLKEN_f.UART1 = 1;
#if (CORE_CLOCK_HZ == 4000000ul)
    /* uart init, HCLK=4M, 19200bps 8N1, mode=1,div=8,cnt=26 */
    DEV_UARTx->SCON = 0;
    DEV_UARTx->SCON = 0x240u;
    DEV_UARTx->SCNT = ((2500000/baud)*2+5)/10;
#elif (CORE_CLOCK_HZ == 24000000ul)
    /* uart init, HCLK=24M, 115200bps 8N1, mode=1,div=8,cnt=26 */
    DEV_UARTx->SCON = 0;
    DEV_UARTx->SCON = 0x240u;
    DEV_UARTx->SCNT = (12500/(baud/2400)+5)/10;
#else
#error "Not support this core.clock with uartx"
#endif
    DEV_UARTx->ICR_f.RCCF = 0; /* clear rx flag */
    DEV_UARTx->ICR_f.TCCF = 0; /* clear tx flag */
    DEV_UARTx->SCON_f.REN = 1;  /* rx/tx enable */
    DEV_UARTx->SCON_f.RCIE = 1; /* rx interrupt */
    //DEV_UARTx->SCON_f.TCIE = 1; /* tx interrupt */
    /* interrupt setting */
    nvic_irq_enable(UART1_IRQn, 3);
    return 0;
}

int uart1_deinit(void) {
    nvic_irq_disable(UART1_IRQn);
    M0P_SYSCTRL->PERI_CLKEN_f.UART1 = 0;
    gpio_init_in(PA,02);
    gpio_init_in(PA,03);
    gpio_floating(PA,02);
    gpio_floating(PA,03);
    return 0;
}

int uart1_send(uint8_t *buffer, uint8_t size) {
    uint8_t count = TxCounter; 
    if(count + size <= TX_MAXSIZE) {
        for(int i=0; i<size; i++) {
            TxBuffer[count] = buffer[i];
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

static int isrcb_uart1_tx_ch(void) {
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
