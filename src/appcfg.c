#include "hw_bare.h"
#include "appcfg.h"


/*
 * block: [head] [idx] [size] <data...> [chksum]
 */
typedef struct {
    uint32_t head;
    uint32_t idx;
    uint8_t size;
    uint8_t data[APPCFG_BLOCK_SIZE-4-4-1-1];
    uint8_t chksum;
} EECFG;

volatile int eBlockId = -1;
volatile EECFG eBlock;

#define EECFG_ICODE_SIZE    8

int appcfg_init(uint32_t magic) {
    uint8_t chkCnt = 0;
    uint32_t maxIdx = 0;
    uint32_t addr = 0;
    uint8_t *pBlock = (uint8_t *)&eBlock;
    /* select block clear */
    eBlockId = -1;
    hal_appcfg_init();
    /* load config */
    for(int i=0; i<APPCFG_BLOCK_NUMBER; i++) {
        addr = APPCFG_START_ADDRESS + i*APPCFG_BLOCK_SIZE;
        hal_appcfg_read(addr, pBlock, EECFG_ICODE_SIZE);
        if(eBlock.head == magic) {
            if(eBlock.idx >= maxIdx) {
                maxIdx = eBlock.idx;
                eBlockId = i;
            }
        }
        eBlock.head = 0; /* make block to invalid */
    }
    while(eBlockId >= 0) {
        /* read&chkeck eBlock */
        uint8_t chksum = 0x00;
        uint8_t btmp = 0;
        uint8_t size = EECFG_ICODE_SIZE+1;
        addr = APPCFG_START_ADDRESS + eBlockId*APPCFG_BLOCK_SIZE;
        hal_appcfg_read(addr+EECFG_ICODE_SIZE, &btmp, 1);
        size += btmp;
        hal_appcfg_read(addr, pBlock, size);
        for(int i=0; i<size; i++) {
            chksum += pBlock[i];
        }
        hal_appcfg_read(addr+APPCFG_BLOCK_SIZE, &btmp, 1);
        chksum += btmp;
        if(chksum == 0xFF) {
            /* chksum pass: break  */
            break;
        }
        /* chksum error: find next */
        eBlockId = (eBlockId-1+APPCFG_BLOCK_NUMBER)%APPCFG_BLOCK_NUMBER;
        chkCnt += 1;
        if(chkCnt >= APPCFG_BLOCK_NUMBER) {
            eBlockId=-1;
            break;
        }
    }
    if(eBlockId < 0) {
        /* init eBlock */
        eBlock.head = magic;
    }
    return 0;
}

int appcfg_write(void *cfg, int size) {
    uint8_t *pcfg = (uint8_t *)cfg;
    if((size>0) && (size<=sizeof(eBlock.data))) {
        uint8_t chksum = 0;
        int blk_id = (eBlockId+1)%APPCFG_BLOCK_NUMBER;
        uint8_t *pBlock = (uint8_t *)&eBlock;
        eBlock.idx += 1;
        eBlock.size = size;
        for(int i=0; i<size; i++) {
            eBlock.data[i] = pcfg[i];
            chksum += pcfg[i];
        }
        for(int i=0; i<9; i++) {
            chksum += *(pBlock+i);
        }
        eBlock.chksum = 0xFF-chksum;
        /* write config to device */
        uint32_t addr = APPCFG_START_ADDRESS + blk_id*APPCFG_BLOCK_SIZE;
        hal_appcfg_write(addr, (uint8_t *)(&eBlockId), APPCFG_BLOCK_SIZE);
        eBlockId = blk_id;
        return 0;
    }
    return -1;
}

int appcfg_read(void *cfg) {
    uint8_t *pcfg = (uint8_t *)cfg;
    if(eBlockId >= 0) {
        for(int i=0; i<eBlock.size; i++) {
            pcfg[i] = eBlock.data[i];
        }
        return eBlock.size;
    }
    return -1;
}
