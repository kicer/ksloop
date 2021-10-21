#include "hw_bare.h"
#include "w25qxx.h"


#define DEV_SPIx      M0P_SPI1
#define CMD_READ_MANUFACTURER      0x90

#define w25qxx_rbyte()     w25qxx_rw_byte(0)
#define w25qxx_wbyte(ch)   w25qxx_rw_byte(ch)

static uint8_t w25qxx_rw_byte(uint8_t ch) {
    while(DEV_SPIx->STAT_f.TXE == 0);
    DEV_SPIx->DATA = ch;
    while(DEV_SPIx->STAT_f.RXNE == 0);
    return (DEV_SPIx->DATA);
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
    while(DEV_SPIx->STAT_f.BUSY == 1);
    DEV_SPIx->SSN = 1;
    return id;
}

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
    return 0;
}

int w25qxx_write(uint32_t addr, uint8_t *data, int size) {
    return 0;
}
