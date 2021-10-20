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
    /* led test */
    M0P_GPIO->PA07_SEL = 0;
    M0P_GPIO->PADIR_f.PA07 = 0;
    M0P_GPIO->PAOUT_f.PA07 = 0;
}

static void detec_loop(void) {
    /* led loop */
    M0P_GPIO->PAOUT ^= BIT(7);
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


/* ################# main function #################   */
int board_init(void) {
    load_config();
    hw_io_init();
    //sys_task_reg_event(EVENT_UART_RECV_PKG, uart_recv_pkg_cb);
    sys_task_reg_timer(1000, detec_loop);
    sys_task_reg_alarm(1024, delay_exec);
    return 0;
}
