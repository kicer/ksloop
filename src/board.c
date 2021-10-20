#include "hw_bare.h"
#include "log.h"
#include "sys.h"
#include "board.h"

/* global vars */
static DevData gDevData;
static DevCfg gDevCfg;


static void hw_io_init(void) {
    /* io clock enable */
    M0P_SYSCTRL->PERI_CLKEN_f.GPIO = 1;
    /* buzzer, off*/
    gpio_init_out(PA,07,0);
    /* 4g.pwr, off */
    gpio_init_out(PD,06,0);
    /* sensor.pwr, on */
    gpio_init_out(PB,06,1);
    /* ble.pwr, off */
    gpio_init_out(PA,11,0);
    /* ble.rst, pull-up */
    gpio_init_in(PA,12);
    /* eeprom.cs, off */
    gpio_init_out(PB,12,1);
    /* unused pin, set 0?
     * gpio_init_out(PC,13,0);
     * gpio_init_out(PA,06,0);
     * gpio_init_out(PB,07,0);
     * gpio_init_out(PB,05,0);
     * gpio_init_out(PA,15,0);
     * gpio_init_out(PD,07,0);
     */
}

static void detec_loop(void) {
    /* led loop */
    gpio_out(PA,07,!gpio_read(PA,07));
}

static void load_config(void) {
    /* load cfg */
    if(gDevCfg.magic != MAGIC_CODE) {
        gDevCfg.magic = MAGIC_CODE;
    }
    /* init global */
    for(int i=0; i<sizeof(gDevData); i++) {
        *(((uint8_t *)&gDevData)+i) = 0x00;
    }
}

static void delay_exec(void) {
    /* logger */
    log_init(LOG_DEBUG);
    dmesg_hex(LOG_INFO, "HWB.v1:", (uint8_t *)&gDevCfg, sizeof(gDevCfg));
}

static void goto_sleep() {
    log_deinit();
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    SCB->SCR |= 1u<<0;
}


/* ################# main function #################   */
int board_init(void) {
    load_config();
    hw_io_init();
    //sys_task_reg_event(EVENT_UART_RECV_PKG, uart_recv_pkg_cb);
    sys_task_reg_timer(5000, detec_loop);
    sys_task_reg_alarm(1024, delay_exec);
    sys_task_reg_alarm(20240, goto_sleep);
    return 0;
}
