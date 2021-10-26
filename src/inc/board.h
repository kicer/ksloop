#ifndef _BOARD_H_
#define _BOARD_H_


#include <stdint.h>

#define EVENT_UART_RECV_PKG     1
#define EVENT_UART_SEND_PKG     2
#define EVENT_GHGKEY_ACTION     3
#define EVENT_SYKEY_ACTION      4

#define MAGIC_CODE        (0x20211025) /* 2021-10-25 */
#define DEVICE_CODE       (SENS_TRH_IN|SENS_ECS0)

typedef enum {
    SENS_ECS0       = (uint8_t)0x01,
    SENS_ECS1       = (uint8_t)0x02,
    SENS_ECS2       = (uint8_t)0x04,
    SENS_ECS3       = (uint8_t)0x08,
    SENS_TRH_OUT    = (uint8_t)0x40,
    SENS_TRH_IN     = (uint8_t)0x80,
} sens_type_t;

typedef struct {
    uint8_t device;
    uint8_t sens;
    uint16_t t;
    uint16_t rh;
    /* private */
    uint32_t ts;
} DevData;

typedef struct {
    uint8_t logLevel;
    uint32_t powerCnt;
    uint32_t magic;
} DevCfg;

extern int board_init(void);


#endif /* _BOARD_H_ */
