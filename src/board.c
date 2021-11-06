#include "hw_bare.h"
#include "board.h"
#include "sys.h"
#include "appcfg.h"
#include "adc.h"
#include "tube.h"


/* global vars */
static DevData gDevData;
static DevCfg gDevCfg;

/* k2/k3有值则记录有标定数值 */
#define IS_CALI_FINISH() (gDevCfg.k2&&gDevCfg.k3)

static void hw_io_init(void) {
    /* io clock enable */
    M0P_CLOCK->PERI_CLKEN_f.GPIO = 1;
}

static void load_config(void) {
    /* init value */
    for(int i=0; i<sizeof(gDevData); i++) {
        *(((uint8_t *)&gDevData)+i) = 0x00;
    }
    int size = appcfg_read(&gDevCfg, sizeof(gDevCfg));
    /* load config */
    if((size != sizeof(DevCfg))||(gDevCfg.magic != MAGIC_CODE)) {
        for(int i=0; i<sizeof(DevCfg); i++) {
            *(((uint8_t *)&gDevCfg)+i) = 0x00;
        }
        gDevCfg.addr = SENS_I2C_ADDR;
        gDevCfg.report = 0x01;
        gDevCfg.syncTime = 8;
        /* 
         * gDevCfg.ad0 = 0x01A1;
         * gDevCfg.adv = 0x08AD;
         */
        gDevCfg.adv = 0; /* for start from zero */
        gDevCfg.k1 = 0x0123;
        gDevCfg.k2 = 13;
        gDevCfg.k3 = 213;
        gDevCfg.factor = 20;
        gDevCfg.magic = MAGIC_CODE;
    }
    gDevData.ad_min = 0xFFFF;
    gDevData.state = ST_LOAD_CFG;
    gDevData.report = gDevCfg.report;
}

static uint16_t _ad_last = 0xFFFF;
static void loop_main(void) {
    static uint16_t _pht_idx = 0;
    uint8_t idx = gDevData.idx;
    /* sens */
    gDevData.ad_sens[idx] = adc_convert(4, 16);
    if(idx >= ADC_BUF_SIZE-1) {
        uint16_t ad;
        /* ad.sens */
        uint16_t _sum = 0;
        uint16_t _ad_min = gDevData.ad_sens[0];
        uint16_t _ad_max = gDevData.ad_sens[0];
        for(int i=0; i<ADC_BUF_SIZE; i++) {
            ad = gDevData.ad_sens[i];
            _sum += ad;
            if(ad < _ad_min) _ad_min = ad;
            if(ad > _ad_max) _ad_max = ad;
        }
        ad = _sum-_ad_min-_ad_max;
        gDevData.ad = ad;
        /* ad.1v8 */
        if(gDevData.state < ST_1V8_READ) {
            /* 完成1v8电压读取，进入预热模式 */
            gDevData.state = ST_1V8_READ;
        } else if(gDevData.state < ST_PREHEART) {
            if(ad >= _ad_last) {
                if(ad-_ad_last>6) {
                    /* 有升高较多则退出预热 */
                    gDevData.state = ST_PREHEART;
                } else {
                    /* 有连续升高一些则退出预热 */
                    _pht_idx += 1;
                    if(_pht_idx > 5) gDevData.state = ST_PREHEART;
                }
            } else { /* 预热时候AD值一直下降 */
                _pht_idx = 0;
            }
            _ad_last = ad;
        } else {
            if(gDevData.state != ST_NORMAL){
                /* 进入正常模式 */
                if(IS_CALI_FINISH()) {
                    gDevData.state = ST_NORMAL;
                }
            } else {
                ad = ((uint32_t)ad*gDevCfg.factor+(uint32_t)(100-gDevCfg.factor)*_ad_last)/100;
            }
            _ad_last = ad;
        }
        /* calc */
        uint16_t tvoc,hcho,co2;
        uint16_t adv = gDevCfg.adv;
        uint16_t ad0 = gDevCfg.ad0;
        /* 零点AD值转换
         * 供电电压变化，对应的零点AD值也会有改变
         */
        //ad0 = (adv==0)?0xFFFF:(uint16_t)((float)ad0*adk/adv);
        if(adv == 0) {
            ad0 = 0xFFFF;
        }
        if(ad<ad0) { /* 有更小值则更新AD0值 */
            gDevCfg.ad0 = ad;
            gDevCfg.adv = 0;
            ad0 = ad;
        }
        /* 设置本周期内最小ad值 */
        if(gDevData.ad_min > ad) gDevData.ad_min = ad;
        /* 计算tvoc数值 */
        tvoc = (uint16_t)((ad - ad0)*(float)gDevCfg.k1/100);
        if(IS_CALI_FINISH()) {
            hcho = (uint16_t)((ad - ad0)*(float)gDevCfg.k2/100);
            co2 = 400+(uint16_t)((ad - ad0)*(float)gDevCfg.k3/100);
        } else {
            /* 标定模式下上传raw值 */
            hcho = ad;
            co2 = ad0;
        }
        gDevData.tvoc = tvoc;
        gDevData.hcho = hcho;
        gDevData.co2 = co2;
        gDevData.idx = 0;
        /* report */
        if(gDevCfg.report > 0) {
            if(gDevData.report <= 1) {
                gDevData.report = gDevCfg.report;
                tube_show_digi(co2);
            } else {
                gDevData.report -= 1;
            }
        }
    } else {
        if((gDevData.state<ST_1V8_READ)&&(idx!=0)) {
            /* read 1v8 */
            gDevData.ad_1v8 += adc_convert(4, 16);
        }
        gDevData.idx = idx+1;
    }
}

static void sync_cfg(void) {
    /* save data */
    gDevCfg.caliCnt += 1;
    appcfg_write(&gDevCfg, sizeof(DevCfg));
    //dmesg_hex(LOG_INFO, "Save CFG:", (uint8_t *)gDevCfg, sizeof(gDevCfg));
    /* reset to last N-day's ad0 */
    uint8_t idx = gDevData.cali_idx;
    uint16_t min = gDevData.ad_min;
    gDevData.cali_buf[idx%ZERO_DAYS_CNT] = min;
    for(int i=0; i<ZERO_DAYS_CNT; i++) {
        uint16_t n = gDevData.cali_buf[i];
        if((n!=0)&&(n<min)) min = n;
    }
    gDevData.cali_idx = idx+1;
    /* set ad-zero */
    gDevCfg.ad0 = min;
    gDevCfg.adv = gDevData.ad_1v8;
    /* reset ad_min */
    gDevData.ad_min = 0xFFFF;
}

static void user_test(void) {
    static int v = 0;
    v += 1;
    tube_show_digi(v);
}

static void delay_exec(void) {
    tube_show_wait();
    /* powerCnt */
    gDevCfg.powerCnt += 1;
    appcfg_write(&gDevCfg, sizeof(DevCfg));
    /* logger */
    //log_init(LOG_DEBUG);
    //dmesg_hex(LOG_INFO, "MVQ6600 hwV2.0:", (uint8_t *)gDevCfg, sizeof(gDevCfg));
}

/* ################# main function #################   */
int board_init(void) {
    hw_delay_init();
    hw_io_init();
    appcfg_init(MAGIC_CODE);
    load_config();
    /* peripheral init */
    adc_init();
    tube_init(8);
    /* task init */
    sys_task_reg_alarm(67, delay_exec);
    //sys_task_reg_timer(ADC_LOOP_TICKS, loop_main);
    sys_task_reg_timer(1000, user_test);
    sys_task_reg_timer((clock_t)3600000*gDevCfg.syncTime, sync_cfg);
    return 0;
}
