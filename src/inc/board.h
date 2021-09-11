#ifndef _BOARD_H_
#define _BOARD_H_


#include <stdint.h>


#define EVENT_UART1_RECV_PKG     1
#define EVENT_UART1_SEND_PKG     2
#define EVENT_UART1_DATA_RDY     3

#define MAGIC_CODE        (0x20210911) /* 2021-09-11 */

#define REPORT_SIZE   19    

typedef struct {
    uint8_t orderSt;
    /* private */
    uint8_t devSt;
} DevData;

typedef struct {
    uint8_t rst;
    uint32_t magic;
} DevCfg;

extern int board_init(void);


#endif /* _BOARD_H_ */
