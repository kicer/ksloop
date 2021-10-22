#ifndef _W25QXX_H_
#define _W25QXX_H_


#include <stdint.h>

typedef enum {
    W25Q80  = 0XEF13,        /**< w25q80 */
    W25Q16  = 0XEF14,        /**< w25q16 */
    W25Q32  = 0XEF15,        /**< w25q32 */
    W25Q64  = 0XEF16,        /**< w25q64 */
    W25Q128 = 0XEF17,        /**< w25q128 */
    W25Q256 = 0XEF18,        /**< w25q256 */
} w25qxx_type_t;

extern int w25qxx_init(w25qxx_type_t type);
extern int w25qxx_deinit(void);
extern int w25qxx_read(uint32_t addr, uint8_t *data, int size);
extern int w25qxx_write(uint32_t addr, uint8_t *data, int size);
extern int w25qxx_block_erase(uint32_t addr);
extern int w25qxx_sector_erase(uint32_t addr);

/* private */
#define W25QXX_PAGE_SIZE        256
#define W25QXX_SECTOR_SIZE      4096
#define W25QXX_BLOCK_SIZE       65536
#define W25QXX_PAGE_PER_SECTOR  16      /* 4096/256 */


#endif /* _W25QXX_H_ */
