#ifndef _BOARD_H_
#define _BOARD_H_


#include <stdint.h>

#define EVENT_UART_RECV_PKG     1
#define EVENT_UART_SEND_PKG     2

#define MAGIC_CODE        (0x20210911) /* 2021-09-11 */

typedef struct {
    uint8_t devSt;
} DevData;

typedef struct {
    uint32_t magic;
} DevCfg;

extern int board_init(void);


#endif /* _BOARD_H_ */
