/* io-uart logger */
#include "ch55x.h"
#include "log.h"


/* bsp helper function */
#if defined(LOG_DEVICE_UART)
    #include "uart.h"    
    #define _log_init()     do {uart_init(LOG_BAUDRATE);} while(0)
    #define _log_send       uart_send
#endif /* LOG_DEVICE_UART */


static uint8_t TxBuffer[LOG_TX_MAXSIZE];
static uint8_t gLevel;

static const uint8_t LOG_CODE_TABLE[] = {'-','E','W','I','D'};

#define LOG_TX_EMPTY()  (TxCounter==0)

int log_init(uint8_t loglevel) {
    if(loglevel != LOG_NONE) {
        _log_init();
    }
    gLevel = loglevel;
    return 0;
}

int dmesg(uint8_t loglevel, const uint8_t *pbuf, uint8_t size) {
    #define H2C(c) ((c)>=10?'A'-10+(c):(c)+'0')
    uint8_t TxCounter = 0;

    /* judge loglevel */
    if((loglevel&0x7F) > gLevel) return 0;
    /* level for hex */
    if(loglevel&LOG_HEX) loglevel = LOG_HEX;

    int _count = TxCounter+size+3;
    if(loglevel == LOG_HEX) {
        _count += size;
        _count += size;
    }

    if(_count <= LOG_TX_MAXSIZE) {
        if(loglevel != LOG_HEX) {
            uint8_t tch = LOG_CODE_TABLE[loglevel];
            TxBuffer[TxCounter] = tch; TxCounter+=1;
            TxBuffer[TxCounter] = '>'; TxCounter+=1;
            TxBuffer[TxCounter] = ' '; TxCounter+=1;
        }
        for(int i=0; i<size; i++) {
            if(loglevel == LOG_HEX) {
                TxBuffer[TxCounter] = H2C((pbuf[i]>>4)); TxCounter+=1;
                TxBuffer[TxCounter] = H2C((pbuf[i]&0x0F));TxCounter+=1;
                TxBuffer[TxCounter] = ' ';
                if(i==size-1) TxBuffer[TxCounter] = '\n';
            } else {
                TxBuffer[TxCounter] = pbuf[i];
            }
            TxCounter += 1;
        }
        return _log_send(TxBuffer, TxCounter);
    }

    return -1;
}
