#include "hw_bare.h"
#include "board.h"
#include "sys.h"
#include "appcfg.h"
#include "log.h"
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

static void tube_debug_msg(int code) {
    static uint8_t step = 0;
    switch(step) {
        case 0: /* 显示版本 */
        case 1: /* 显示版本A1 */
            tube_show_str((uint8_t *)"V a1", 4);
            break;
        case 2: /* 显示启动次数 */
        case 3: /* 显示启动次数 */
            tube_show_label((uint8_t *)"B", 1, gDevCfg.powerCnt);
            break;
        case 4: /* 显示标定次数 */
        case 5: /* 显示标定次数 */
            tube_show_label((uint8_t *)"C", 1, gDevCfg.caliCnt);
            break;
        case 6:
        case 7:
            tube_show_digi(gDevCfg.ad0, 0x0F);
            break;
        case 10:
            tube_show_digi(gDevData.ad, 0x01);
            break;
        case 11:
            tube_show_digi(gDevData.ad0, 0x03);
            break;
        case 12:
            tube_show_digi(gDevData.ad_min, 0x07);
            break;
        case 13:
            tube_show_label((uint8_t *)"T",1,adc_ts());
            break;
        case 14:
            tube_show_label((uint8_t *)"L",1,gDevData.level);
            break;
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            tube_show_digi(gDevData.co2, 0);
            if(step>=19) step = 9; /* 循环显示 */
            break;
        default: /* 显示预热时间 */
            if(gDevData.state == ST_NORMAL) {
                tube_show_digi(gDevData.ad, 0x01);
                step = 10;
            } else {
                tube_show_label((uint8_t *)"ht", 2, code);
                step = 7;
            }
            break;
    }
    step += 1;
}

#define SILENT_MEAS_TS     10
static void sens_raw_cb(uint16_t raw_ad) {
#define _CONTINUE(t)       ((t) >= 5)
#define _READY(t)          ((t) >= 10)
    static uint8_t _stable_time = 0;
    static uint8_t _ready_time = 0;
    static uint8_t _zero_idx = 0; /* 标计归0的次数 */
    static uint8_t _factor_high = 0;
    static uint8_t _factor_low = 0;
    static uint8_t _factor_fix = 0;
    static uint16_t sens_last = 0;
    static uint16_t raw_last = 0;

    uint16_t adv, sens;
    uint16_t zero_ad = gDevData.ad0;
    uint16_t preheat = gDevData.preheat;
    uint8_t factor = gDevCfg.factor;
    /* 原始数值从开始就记录上报，算法不作修改 */
    gDevData.ad = raw_ad;
    if(preheat == 0) { /* 正常测量 */
        /* 加入动态修正系数 */
        factor += _factor_fix;
        if(factor > 100) factor = 100;
        if(_stable_time > 0) { /* 稳定时间 */
            _stable_time -= 1;
            if(_factor_fix > 0) { /* 数值在连续变化 */
                _ready_time = 0;
            } else { /* 数值在波动 */
                _ready_time += 1;
                if(_READY(_ready_time)) {
                    if(_stable_time > 10) {
                        _stable_time -= 10;
                        _ready_time = 0;
                    }
                }
            }
            preheat = _stable_time;
        } else { /* 稳定后才输出有效数值 */
            gDevData.state = ST_NORMAL;
            /* 设置本周期内最小ad值 */
            if(gDevData.ad_min > raw_ad) {
                if(raw_ad >= zero_ad) { /* 有效的最低raw值 */
                    gDevData.ad_min = raw_ad;
                }
            }
        }
    } else if(preheat < SILENT_MEAS_TS){
        _stable_time = gDevCfg.preheat;
        _ready_time = 0;
    } else {
        gDevData.state = ST_PREHEART;
    }
    adv = (raw_ad>zero_ad)?raw_ad-zero_ad:0;
    adv = (uint32_t)adv*gDevCfg.k3/100;
    sens = ((uint32_t)adv*factor+(uint32_t)(100-factor)*sens_last)/100;
    /* 动态调整滤波系数 */
    if(raw_last < raw_ad) { /* 数值在增高 */
        _factor_high += 1;
        if(_CONTINUE(_factor_high)) { /* 且连续5次升高 */
            _factor_fix = (_factor_high<<1);
        } else if(_CONTINUE(_factor_low)) {
            _factor_fix = 0;
        }
        _factor_low = 0;
    } else { /* 数值在降低 */
        if(raw_last > raw_ad) _factor_low += 1;
        if(_CONTINUE(_factor_low)) { /* 且连续5次降低 */
            _factor_fix = (_factor_low<<1);
        } else if(_CONTINUE(_factor_high)) {
            _factor_fix = 0;
        }
        _factor_high = 0;
    }
    /* 判断sens是否长期为0 */
    if(raw_ad < zero_ad) {
        if(_zero_idx > gDevCfg.zeroRstCnt) {
            if(_factor_fix == 0) { /* 数值已经稳定 */
                /* 近x分钟内数值一直为0, 重新执行零点校准 */
                zero_ad = raw_ad; /* 简单重置下零点AD值 */
                gDevData.ad0 = zero_ad;
                _zero_idx = 0;
            }
        } else {
            _zero_idx += 1;
        }
    } else {
        if(_zero_idx > 0) _zero_idx-=1;
    }
    /* 赋值 */
    gDevData.co2 = 400+sens;
    if(1) { /* 计算空气质量等级 */
        uint16_t val = gDevData.co2;
        uint16_t level = 0;
        /* 400:100, 1000:80, 1800:60, 5000:0 */
        if(val<400) {
            level = 100;
        } else if(val<1000) {
            level = 100-(val-400)/30;
        } else if(val<1800) {
            level = 80-(val-1000)/40;
        } else if(val<5000) {
            level = 60-3*(val-1800)/160;
        }
        gDevData.level = level;
    }
    sens_last = sens;
    raw_last = raw_ad;
    /* report */
    if(gDevCfg.report > 0) {
        if(gDevData.report <= 1) {
            gDevData.report = gDevCfg.report;
            tube_debug_msg(preheat);
        } else {
            gDevData.report -= 1;
        }
    }
}

static void loop_main(void) {
    uint8_t idx = gDevData.idx;
    gDevData.ad_sens[idx] = adc_convert(0, 16);
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
        gDevData.idx = 0;
        if(gDevData.preheat > 0) gDevData.preheat -= 1;
        sens_raw_cb(ad);
    } else {
        gDevData.idx = idx+1;
    }
}

static void sync_cfg(void) {
    /* 本周期内有效的最低值作为零点值
     * 遍历并拉取缓存数据，以最低值作为下周期零点值
     */
    uint8_t idx = gDevCfg.cali_idx;
    uint16_t min = gDevData.ad_min;
    gDevCfg.cali_buf[idx%ZERO_DAYS_CNT] = min;
    for(int i=0; i<ZERO_DAYS_CNT; i++) {
        uint16_t n = gDevCfg.cali_buf[i];
        if((n!=0)&&(n<min)) min = n;
    }
    gDevCfg.cali_idx = idx+1;
    /* set ad-zero */
    gDevData.ad0 = min;
    gDevCfg.ad0 = min;
    gDevData.ad_min = 0xFFFF;
    /* save data */
    gDevCfg.caliCnt += 1;
    appcfg_write(&gDevCfg, sizeof(DevCfg));
    dmesg_hex(LOG_INFO, "Save CFG:", (uint8_t *)&gDevCfg, sizeof(gDevCfg));
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
        gDevCfg.preheat = 30;   /* 预热30s+动态30s */
        gDevCfg.report = 1;     /* 1s报告一次数据 */
        gDevCfg.syncTime = 8;   /* 8小时同步一次标定数据 */
        gDevCfg.ad0 = 0xFFFF;   /* 默认零点AD值 */
        gDevCfg.k1 = 100;       /* k.tvoc */
        gDevCfg.k2 = 100;       /* k.hcho */
        gDevCfg.k3 = 100;       /* k.co2 */
        gDevCfg.factor = 20;    /* 数值一阶滤波系数 */
        gDevCfg.zeroRstCnt = 120; /* 归0两分钟后计算零点 */
        gDevCfg.magic = MAGIC_CODE;
    }
    gDevData.ad0 = gDevCfg.ad0;
    gDevData.ad_min = 0xFFFF;
    gDevData.state = ST_LOAD_CFG;
    gDevData.preheat = gDevCfg.preheat;
    gDevData.report = gDevCfg.report;
}

static void delay_exec(void) {
    /* powerCnt */
    gDevCfg.powerCnt += 1;
    appcfg_write(&gDevCfg, sizeof(DevCfg));
    /* logger */
    log_init(LOG_DEBUG);
    dmesg_hex(LOG_INFO, "eCO2 hwV2.0:", (uint8_t *)&gDevCfg, sizeof(gDevCfg));
    /* show msg */
    tube_set_bright(6);
    tube_debug_msg(0);
}

/* ################# main functions #################   */
int board_init(void) {
    hw_delay_init();
    hw_io_init();
    appcfg_init(MAGIC_CODE);
    load_config();
    /* peripheral init */
    adc_init();
    tube_init();
    /* task init */
    sys_task_reg_alarm(67, delay_exec);
    sys_task_reg_timer(ADC_LOOP_TICKS, loop_main);
    sys_task_reg_timer((clock_t)3600000*gDevCfg.syncTime, sync_cfg);
    return 0;
}
