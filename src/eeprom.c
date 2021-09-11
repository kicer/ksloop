#include "ch55x.h"
#include "eeprom.h"


/*
 * block: [head] [idx] [size] <data...> [chksum]
 */
typedef struct {
    uint8_t head[4];
    uint8_t idx[4];
    uint8_t size;
    uint8_t data[FLASH_BLOCK_SIZE-4-4-1-1];
    uint8_t chksum;
} EECFG;

int eBlockId = -1;
EECFG eBlock;

#define UID32(p)            (*((uint32_t *)(p)))
//(sizeof(eBlock.head)+sizeof(eBlock.idx))
#define EECFG_ICODE_SIZE    8


static void _eeprom_bsp_init(void) {
}

int eeprom_init(uint32_t magic) {
    _eeprom_bsp_init();
    try_param(magic);
    return 0;
}

int eeprom_write_config(void *cfg, int size) {
    try_param(cfg);
    try_param(size);
    return -1;
}

int eeprom_read_config(void *cfg) {
    try_param(cfg);
    return -1;
}
