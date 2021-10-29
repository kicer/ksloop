#include "hw_bare.h"
#include "board.h"
#include "sys.h"
#include "log.h"
#include "appcfg.h"
#include "rtc.h"
#include "sht3x.h"
#include "adc.h"


/* global vars */
static DevData gDevData;
static DevCfg gDevCfg;

#define pwr_ble_init()      gpio_init_out(PA,11,0)
#define pwr_ble_on()        gpio_out(PA,11,1)
#define pwr_ble_off()       gpio_out(PA,11,0)

#define pwr_sensor_init()   gpio_init_out(PB,06,0)
#define pwr_sensor_on()     gpio_out(PB,06,1)
#define pwr_sensor_off()    gpio_out(PB,06,0)

#define pwr_4g_init()       gpio_init_out(PD,06,0)
#define pwr_4g_on()         gpio_out(PD,06,1)
#define pwr_4g_off()        gpio_out(PD,06,0)

#define pwr_buzzer_init()   gpio_init_out(PA,07,0)
#define pwr_buzzer_on()     gpio_out(PA,07,1)
#define pwr_buzzer_off()    gpio_out(PA,07,0)


static void hw_io_init(void) {
    /* io clock enable */
    M0P_SYSCTRL->PERI_CLKEN_f.GPIO = 1;
    /* peri power init */
    pwr_buzzer_init(); pwr_buzzer_off();
    pwr_4g_init(); pwr_4g_off();
    pwr_ble_init(); pwr_ble_off();
    pwr_sensor_init(); pwr_sensor_on();
    /* sy_key, raise&fall it */
    gpio_init_in(PB,04);
    gpio_it_enable(PB,04,FIE);
    gpio_it_enable(PB,04,RIE);
    /* ghg_key, raise&fall it */
    gpio_init_in(PB,03);
    gpio_it_enable(PB,03,FIE);
    gpio_it_enable(PB,03,RIE);
    /* enable PORTB it */
    nvic_irq_enable(PORTB_IRQn, 3);
    /* unused pin, set 0? */
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
    //gpio_out(PA,07,!gpio_read(PA,07));
    /* sht3x meas start */
    sht3x_meas();
    sys_task_reg_alarm(15, sensor_read_cb);
}

static void goto_deepsleep() {
    pwr_buzzer_on();
    pwr_ble_off();
    log_deinit();
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}

static void exit_deepsleep() {
    pwr_buzzer_off();
    pwr_ble_on();
    log_init(gDevCfg.logLevel);
    SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
}

/* 干簧管动作 */
static void user_ghgkey_cb(void) {
    if(gpio_read(PB,03)) {
        goto_deepsleep();
    } else {
        exit_deepsleep();
    }
}

/* 水银开关动作 */
static void user_sykey_cb(void) {
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
        gDevCfg.logLevel = LOG_DEBUG;
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

static void user_test1(void) {
    adc_init();
    uint16_t tmp = adc_convert(4,16);
    adc_deinit();
    dmesg_hex(LOG_DEBUG, "user_test1:", (uint8_t *)&tmp, sizeof(tmp));
}

void isrcb_uart1_rx(uint8_t ch) {
    switch(ch) {
        case '0':
            sys_event_trigger(EVENT_TEST1_ACTION);
            break;
        default:
            gDevData.ts = time_ts();
            dmesg_hex(LOG_DEBUG, "@", (uint8_t *)&gDevData, sizeof(gDevData));
            break;
    }
}


/* ################# main function #################   */
int board_init(void) {
    hw_delay_init();
    hw_io_init();
    appcfg_init(MAGIC_CODE);
    load_config();
    log_init(gDevCfg.logLevel);
    /* peripheral init */
    rtc_init();
    sht3x_init();
    sys_task_reg_event(EVENT_TEST1_ACTION,  user_test1); /* todo: test */
    sys_task_reg_timer(2000, detec_loop);
    sys_task_reg_alarm(1024, delay_exec);
    sys_task_reg_event(EVENT_SYKEY_ACTION,  user_sykey_cb);
    sys_task_reg_event(EVENT_GHGKEY_ACTION, user_ghgkey_cb);
    sys_task_reg_alarm(600000, goto_deepsleep);
    return 0;
}

void PORTB_IRQn_handler(void) {
    if(M0P_GPIO->PB_STAT_f.PB04) {
        M0P_GPIO->PB_ICLR_f.PB04 = 0;
        sys_event_trigger(EVENT_SYKEY_ACTION);
    } else if(M0P_GPIO->PB_STAT_f.PB03) {
        M0P_GPIO->PB_ICLR_f.PB03 = 0;
        sys_event_trigger(EVENT_GHGKEY_ACTION);
    }
}
