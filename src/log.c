/* io-uart logger */
#include "ch55x.h"
#include "log.h"


/* bsp helper function */
#define bsp_log_io_init()    do {} while(0)
#define bsp_log_io_0()       do {} while(0)
#define bsp_log_io_1()       do {} while(0)

static void bsp_log_timer_init(void) {
}

static void bsp_log_timer_enable(void) {
}

static void bsp_log_timer_disable(void) {
}


static uint8_t TxBuffer[LOG_TX_MAXSIZE];
static int TxCounter = 0;
static int TxPointer = 0;
static uint8_t gLevel;

static const uint8_t LOG_CODE_TABLE[] = {'-','E','W','I','D'};

#define LOG_TX_EMPTY()  (TxCounter==0)

int log_init(uint8_t loglevel) {
    if(loglevel != LOG_NONE) {
        bsp_log_io_init();
        bsp_log_timer_init();
    }
    gLevel = loglevel;
    return 0;
}

int dmesg(uint8_t loglevel, const uint8_t *pbuf, uint8_t size) {
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
        #define H2C(c) ((c)>=10?'A'-10+(c):(c)+'0')
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
        if(TxPointer == 0) {
            bsp_log_timer_enable();
        }
        return size;
    }
    return -1;
}

int log_tx_data(void) {
    /* todo: if it(dmesg) then exec dmesg & return end */
    int ch = -1;
    int count = TxCounter;
    if(count > 0) {
        ch = TxBuffer[TxPointer];
        TxPointer += 1;
        if(TxPointer >= count) {
            /* flush output */
            TxCounter = 0;
            TxPointer = 0;
        }
    }
    return ch;
}

void log_ticks_cb(void) {
    static uint8_t bit = 0;
    static uint8_t dat = 0;
    if(bit == 0) {
        int ch = log_tx_data();
        if(ch != -1) {
          dat = (uint8_t)ch;
          bsp_log_io_1(); bit += 1;
        } else {
          bsp_log_timer_disable();
        }
    } else if(bit == 1) {
        bsp_log_io_0();
        bit += 1;
    } else if((bit>1)&&(bit<10)) {
        if(dat & (1<<(bit-2))) {
            bsp_log_io_1();
        } else {
            bsp_log_io_0();
        }
        bit += 1;
    } else {
        bsp_log_io_1();
        bit = 0;
    }
}
