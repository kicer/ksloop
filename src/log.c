#include "hw_bare.h"
#include "log.h"


/* global log level */
static uint8_t gLevel;
const uint8_t LOG_CODE_TABLE[5] = {'-','E','W','I','D'};

int log_init(uint8_t loglevel) {
    if(loglevel != LOG_NONE) {
        _log_init();
    }
    gLevel = loglevel;
    return 0;
}

int dmesg(uint8_t loglevel, const uint8_t *pbuf, uint8_t size) {
    #define H2C(c) ((c)>=10?'A'-10+(c):(c)+'0')
    uint8_t TxBuffer[LOG_TX_MAXSIZE];
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
