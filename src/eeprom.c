#include "hw_bare.h"
#include "eeprom.h"


#define FLASH_UNLOCK()    do {          \
    M0P_FLASH->BYPASS_f.BYSEQ = 0x5A5A; \
    M0P_FLASH->BYPASS_f.BYSEQ = 0xA5A5; \
} while(0)


int eeprom_init(void) {
#if (CORE_CLOCK_HZ == 4000000ul)
    while(M0P_FLASH->CR_f.WAIT != 0) {
        FLASH_UNLOCK();
        M0P_FLASH->CR = 0x00;
    }
#else
    /* need setting TNVS/TPGS/TPROG... */
#error "Not support this core.clock with eeprom"
#endif
    return 0;
}

int eeprom_deinit(void) {
    return 0;
}

int eeprom_read(uint32_t addr, uint8_t *data, int size) {
    for(int i=0; i<size; i++) {
        data[i] = *((volatile uint8_t*)(addr+i));
    }
    return 0;
}

int eeprom_write(uint32_t addr, uint8_t *data, int size) {
    while(M0P_FLASH->SLOCK_f.SLOCK != 0xFFFF) {
        FLASH_UNLOCK();
        M0P_FLASH->SLOCK = 0xFFFF;
    }
    while(M0P_FLASH->CR_f.BUSY==1){};
    while(M0P_FLASH->CR_f.OP != 1) {
        FLASH_UNLOCK();
        M0P_FLASH->CR = 0x01; /* program */
    }
    for(int i=0; i<size; i++) {
        *((volatile uint8_t*)(addr+i)) = data[i];
        while(M0P_FLASH->CR_f.BUSY==1){};
    }
    FLASH_UNLOCK();
    M0P_FLASH->SLOCK = 0x0000;
    for(int i=0; i<size; i++) {
        if(data[i] != *((volatile uint8_t*)(addr+i))) {
            return -1;
        }
    }
    return 0;
}

int eeprom_sector_erase(uint32_t addr) {
    while(M0P_FLASH->SLOCK_f.SLOCK != 0xFFFF) {
        FLASH_UNLOCK();
        M0P_FLASH->SLOCK = 0xFFFF;
    }
    while(M0P_FLASH->CR_f.BUSY==1){};
    while(M0P_FLASH->CR_f.OP != 2) {
        FLASH_UNLOCK();
        M0P_FLASH->CR = 0x02; /* sector erase */
    }
    *((volatile uint8_t*)addr) = 0x00;
    while(M0P_FLASH->CR_f.BUSY==1){};
    FLASH_UNLOCK();
    M0P_FLASH->SLOCK = 0x0000;
    return 0;
}
