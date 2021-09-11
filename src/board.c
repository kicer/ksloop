#include "ch55x.h"
#include "log.h"
#include "sys.h"
#include "board.h"


static DevData gDevData;
static DevCfg gDevCfg;


static void uart_recv_pkg_cb(void) {
    /* todo */
}

static void detec_loop(void) {
    /* todo */
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
    dmesg_hex(LOG_INFO, "CH55x v1:", (uint8_t *)gDevCfg, sizeof(gDevCfg));
}


/* ################# main function #################   */
int board_init(void) {
    load_config();
    sys_task_reg_event(EVENT_UART_RECV_PKG, uart_recv_pkg_cb);
    //sys_task_reg_event(EVENT_UART_SEND_PKG, uart_send_pkg_cb);
    sys_task_reg_timer(1000, detec_loop);
    sys_task_reg_alarm(1024, delay_exec);
    return 0;
}
