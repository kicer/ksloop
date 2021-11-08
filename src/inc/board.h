#ifndef _BOARD_H_
#define _BOARD_H_


#include <stdint.h>

#define EVENT_UART_RECV_PKG     1
#define EVENT_UART_SEND_PKG     2

#define MAGIC_CODE        (0x20211108) /* 2021-11-08 */

/* for 1s=6*166ms */
#define ADC_BUF_SIZE      6
#define ADC_LOOP_TICKS    166

/* cali ad0 of N-days */
#define ZERO_DAYS_CNT     6

#define ST_INIT         0
#define ST_LOAD_CFG     1
#define ST_PREHEART     2
#define ST_NORMAL       0x0A
typedef struct {
    uint8_t state; /* normal? */
    uint16_t ad;
    uint16_t ad0;
    uint16_t tvoc;
    uint16_t hcho;   /* eHCHO */
    uint16_t co2;    /* eCO2  */
    uint16_t level;  /* quality level:0~100 */
    /* private */
    uint8_t idx;
    uint8_t report;
    uint8_t preheat;
    uint16_t ad_min;
    uint16_t ad_sens[ADC_BUF_SIZE];
} DevData;

typedef struct {
    /* !!!size<56bytes */
    uint8_t preheat;
    uint8_t report;
    uint8_t syncTime; /* unit:1hour */
    uint8_t factor; /* 一阶滤波系数 */
    uint16_t k1;
    uint16_t k2;
    uint16_t k3;
    uint16_t ad0;
    uint16_t zeroRstCnt;
    uint16_t caliCnt;
    /* long-time ad0 logic */
    uint16_t cali_idx;
    uint16_t cali_buf[ZERO_DAYS_CNT];
    uint16_t powerCnt;
    uint32_t magic;
} DevCfg;

extern int board_init(void);


#endif /* _BOARD_H_ */
