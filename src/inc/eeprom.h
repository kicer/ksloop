#ifndef _EEPROM_H_
#define _EEPROM_H_


#include <stdint.h>

extern int eeprom_init(void);
extern int eeprom_deinit(void);
extern int eeprom_read(uint32_t addr, uint8_t *data, int size);
extern int eeprom_write(uint32_t addr, uint8_t *data, int size);
extern int eeprom_block_erase(uint32_t addr);
extern int eeprom_sector_erase(uint32_t addr);

/* private */
#define EEPROM_PAGE_SIZE        256
#define EEPROM_SECTOR_SIZE      4096
#define EEPROM_BLOCK_SIZE       65536
#define EEPROM_PAGE_PER_SECTOR  16      /* 4096/256 */


#endif /* _EEPROM_H_ */
