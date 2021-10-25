#include "hw_bare.h"
#include "board.h"
#include "sys.h"
#include "log.h"
#include "appcfg.h"
#include "rtc.h"
#include "sht3x.h"

/* global vars */
static DevData gDevData;
static DevCfg gDevCfg;


static void hw_io_init(void) {
    /* io clock enable */
    M0P_SYSCTRL->PERI_CLKEN_f.GPIO = 1;
    /* buzzer, off*/
    //gpio_init_out(PA,07,0);
    /* 4g.pwr, off */
    gpio_init_out(PD,06,0);
    /* sensor.pwr, on */
    gpio_init_out(PB,06,1);
    /* ble.pwr, off */
    gpio_init_out(PA,11,1); /* todo, for debug */
    /* unused pin, set 0?
     * gpio_init_out(PC,13,0);
     * gpio_init_out(PA,06,0);
     * gpio_init_out(PB,07,0);
     * gpio_init_out(PB,05,0);
     * gpio_init_out(PA,15,0);
     * gpio_init_out(PD,07,0);
     */
}

static void sensor_read_cb(void) {
    uint16_t t, rh;
    if(sht3x_read(&t, &rh) == 0) {
        gDevData.t = t;
        gDevData.rh = rh;
        gDevData.sens |= SENS_TRH_IN;
    }
}

static void detec_loop(void) {
    /* led loop */
    gpio_out(PA,07,!gpio_read(PA,07));
    /* sht3x meas start */
    sht3x_meas();
    sys_task_reg_alarm(15, sensor_read_cb);
}

static void load_config(void) {
    /* init data */
    for(int i=0; i<sizeof(gDevData); i++) {
        *(((uint8_t *)&gDevData)+i) = 0x00;
    }
    /* load cfg */
    int size = appcfg_read(&gDevCfg, sizeof(DevCfg));
    if((size != sizeof(DevCfg))||(gDevCfg.magic != MAGIC_CODE)) {
        for(int i=0; i<sizeof(gDevCfg); i++) {
            *(((uint8_t *)&gDevCfg)+i) = 0x00;
        }
        gDevCfg.magic = MAGIC_CODE;
    }
    /* init vars */
    gDevData.device = DEVICE_CODE;
}

static void delay_exec(void) {
    /* powerCnt */
    gDevCfg.powerCnt += 1;
    appcfg_write(&gDevCfg, sizeof(DevCfg));
    /* logger */
    dmesg_hex(LOG_INFO, "HWB.v1:", (uint8_t *)&gDevCfg, sizeof(gDevCfg));
}

static void goto_sleep() {
    log_deinit();
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    SCB->SCR |= 1u<<0;
}

/* todo: for user.test */
void isrcb_uart1_rx(uint8_t ch) {
    switch(ch) {
        case '#':
            gDevData.ts = time_ts();
            break;
        case '$':
            rtc_init();
            break;
        default:
            break;
    }
    dmesg_hex(LOG_DEBUG, "gDevData:", (uint8_t *)&gDevData, sizeof(gDevData));
}


/* ################# main function #################   */
int board_init(void) {
    hw_io_init();
    appcfg_init(MAGIC_CODE);
    load_config();
    log_init(LOG_DEBUG);
    /* peripheral init */
    rtc_init();
    sht3x_init();
    //sys_task_reg_event(EVENT_UART_RECV_PKG, uart_recv_pkg_cb);
    sys_task_reg_timer(2000, detec_loop);
    sys_task_reg_alarm(1024, delay_exec);
    sys_task_reg_alarm(6000000, goto_sleep);
    return 0;
}
