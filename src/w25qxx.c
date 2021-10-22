#include "hw_bare.h"
#include "w25qxx.h"


#define DEV_SPIx                    M0P_SPI1
#define CMD_READ_MANUFACTURER       0x90
#define CMD_READ_DATA               0x03
#define CMD_WRITE_ENABLE            0x06
#define CMD_PAGE_PROGRAM            0x02
#define CMD_READ_STATUS_REG         0x05
#define CMD_BLOCK_ERASE_64K         0xD8
#define CMD_SECTOR_ERASE            0x20

#define w25qxx_rbyte()     w25qxx_rw_byte(0)
#define w25qxx_wbyte(ch)   w25qxx_rw_byte(ch)

static uint8_t w25qxx_rw_byte(uint8_t ch) {
    while(DEV_SPIx->STAT_f.TXE == 0);
    DEV_SPIx->DATA = ch;
    while(DEV_SPIx->STAT_f.RXNE == 0);
    return (DEV_SPIx->DATA);
}

static int w25qxx_write_enable(void) {
    DEV_SPIx->SSN = 0;
    w25qxx_wbyte(CMD_WRITE_ENABLE);
    DEV_SPIx->SSN = 1;
    return 0;
}

static uint16_t w25qxx_get_id(void) {
    uint16_t id = 0;
    DEV_SPIx->SSN = 0;
    w25qxx_wbyte(CMD_READ_MANUFACTURER);
    w25qxx_wbyte(0x00);
    w25qxx_wbyte(0x00);
    w25qxx_wbyte(0x00);
    id |= w25qxx_rbyte()<<8;
    id |= w25qxx_rbyte();
    DEV_SPIx->SSN = 1;
    return id;
}

static uint8_t w25qxx_get_streg(void) {
    uint8_t st;
    DEV_SPIx->SSN = 0;
    w25qxx_wbyte(CMD_READ_STATUS_REG);
    st = w25qxx_rbyte();
    DEV_SPIx->SSN = 1;
    return st;
}

#define w25qxx_wait_busy() do { \
    } while((w25qxx_get_streg()&0x01)==0x01)

int w25qxx_page_program(uint32_t addr, uint8_t *pdat, int size) {
    w25qxx_write_enable();
    DEV_SPIx->SSN = 0;
    w25qxx_wbyte(CMD_PAGE_PROGRAM);
    w25qxx_wbyte((uint8_t)(addr>>16));
    w25qxx_wbyte((uint8_t)(addr>>8));
    w25qxx_wbyte((uint8_t)(addr));
    for(int i=0; i<size; i++) {
        w25qxx_wbyte(pdat[i]);
    }
    DEV_SPIx->SSN = 1;
    w25qxx_wait_busy();
    return 0;
}


/* ####################### extern function #######################*/
int w25qxx_init(w25qxx_type_t type) {
    gpio_init_afout(PB,12,1); /* SPI.CS  */
    gpio_init_afout(PB,13,1); /* SPI.SCK */
    gpio_init_afin(PB,14,1);  /* SPI.MISO */
    gpio_init_afout(PB,15,1); /* SPI.MOSI */
    /* clock enable */
    M0P_SYSCTRL->PERI_CLKEN_f.SPI1 = 1;
    DEV_SPIx->CR = 0;
    /* master, div2, cpha.1st, cpol.low, enable */
    DEV_SPIx->CR = (uint32_t)0x50;
    if(w25qxx_get_id() == type) {
        return 0;
    }
    w25qxx_deinit();
    return -1;
}

int w25qxx_deinit(void) {
    DEV_SPIx->CR = 0;
    M0P_SYSCTRL->PERI_CLKEN_f.SPI1 = 0;
    gpio_init_in(PB,12); /* SPI.CS  */
    gpio_init_in(PB,13); /* SPI.SCK */
    gpio_init_in(PB,14); /* SPI.MISO */
    gpio_init_in(PB,15); /* SPI.MOSI */
    gpio_floating(PB,12);
    gpio_floating(PB,13);
    gpio_floating(PB,14);
    gpio_floating(PB,15);
    return 0;
}

int w25qxx_read(uint32_t addr, uint8_t *data, int size) {
    DEV_SPIx->SSN = 0;
    w25qxx_wbyte(CMD_READ_DATA);
    w25qxx_wbyte((uint8_t)(addr>>16));
    w25qxx_wbyte((uint8_t)(addr>>8));
    w25qxx_wbyte((uint8_t)(addr));
    for(int i=0; i<size; i++) {
        data[i] = w25qxx_rbyte();
    }
    DEV_SPIx->SSN = 1;
    return 0;
}

int w25qxx_write(uint32_t addr, uint8_t *data, int size) {
    if(addr%W25QXX_PAGE_SIZE== 0) { /* address page align? */
        uint8_t page_cnt = (size+W25QXX_PAGE_SIZE-1)>>8;
        uint8_t idx = 0;
        for(int i=0; i<page_cnt; i++) {
            int _last = size-idx;
            int page_size = (_last<W25QXX_PAGE_SIZE)?_last:W25QXX_PAGE_SIZE;
            w25qxx_page_program(addr, data+idx, page_size);
            addr += page_size;
            idx += page_size;
        }
        return 0;
    }
    return -1;
}

int w25qxx_block_erase(uint32_t addr) {
    w25qxx_write_enable();
    DEV_SPIx->SSN = 0;
    w25qxx_wbyte(CMD_BLOCK_ERASE_64K);
    w25qxx_wbyte((uint8_t)(addr>>16));
    w25qxx_wbyte((uint8_t)(addr>>8));
    w25qxx_wbyte((uint8_t)(addr));
    DEV_SPIx->SSN = 1;
    w25qxx_wait_busy();
    return 0;
}

int w25qxx_sector_erase(uint32_t addr) {
    w25qxx_write_enable();
    DEV_SPIx->SSN = 0;
    w25qxx_wbyte(CMD_SECTOR_ERASE);
    w25qxx_wbyte((uint8_t)(addr>>16));
    w25qxx_wbyte((uint8_t)(addr>>8));
    w25qxx_wbyte((uint8_t)(addr));
    DEV_SPIx->SSN = 1;
    w25qxx_wait_busy();
    return 0;
}
