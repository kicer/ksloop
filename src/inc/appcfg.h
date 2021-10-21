#ifndef _APPCFG_H_
#define _APPCFG_H_


#include <stdint.h>

extern int appcfg_init(uint32_t magic);
extern int appcfg_read(void *cfg);
extern int appcfg_write(void *cfg, int size);

#define APPCFG_DEVICE_W25X      1
#if defined(APPCFG_DEVICE_W25X)
    #include "w25qxx.h"
    #define hal_appcfg_init()       w25qxx_init(W25Q128)
    #define hal_appcfg_deinit       w25qxx_deinit
    #define hal_appcfg_read         w25qxx_read
    #define hal_appcfg_write        w25qxx_write
    /* w25q128  block 0 for appcfg */
    #define APPCFG_START_ADDRESS    0
    #define APPCFG_BLOCK_NUMBER     256
    #define APPCFG_BLOCK_SIZE       256
#else
    #error "Please defined the appcfg.device function"
#endif


#endif /* _APPCFG_H_ */