#ifndef _BOARD_H_
#define _BOARD_H_


#include <stdint.h>

#define EVENT_UART_RECV_PKG     1
#define EVENT_UART_SEND_PKG     2

#define MAGIC_CODE        (0x20211025) /* 2021-10-25 */
#define SENS_ID0          0x6B
#define SENS_ID1          0x30

#define SENS_I2C_ADDR     (SENS_ID0+SENS_ID1-0x30)

/* for 1s */
#define ADC_BUF_SIZE      6  /* 3-66 */
#define ADC_LOOP_TICKS    166
#define REPORT_DATA_SIZE  9

/* cali ad0 of N-days */
#define ZERO_DAYS_CNT     6


#define ST_INIT         0
#define ST_LOAD_CFG     1
#define ST_1V8_READ     2
#define ST_PREHEART     3
#define ST_NORMAL       0x0A
typedef struct { /* base_addr = 0x1000 */
/*
 * 0: init
 * 1: load config ok
 * 2: v1.8 read ok 
 */
    uint8_t state; /* normal? */
    uint16_t ad;
    uint16_t tvoc;
    uint16_t hcho;  /* eHCHO */
    uint16_t co2;  /* eCO2  */
    /* private */
    uint8_t idx;
    uint8_t report;
    uint16_t ad_min;
    uint16_t ad_1v8;
    uint16_t ad_sens[ADC_BUF_SIZE];
    /* long-time ad0 logic */
    uint8_t cali_idx;
    uint16_t cali_buf[ZERO_DAYS_CNT];
} DevData;

typedef struct {
    uint8_t addr;
    uint8_t report;
    uint8_t syncTime; /* unit:1hour */
    uint8_t factor; /* 一阶滤波系数 */
    uint16_t k1;
    uint16_t k2;
    uint16_t k3;
    uint16_t ad0;
    uint16_t adv;
    uint16_t caliCnt;
    uint16_t powerCnt;
    uint32_t magic;
} DevCfg;

extern int board_init(void);


#endif /* _BOARD_H_ */
