#include "hw_bare.h"
#include "board.h"
#include "sys.h"
#include "appcfg.h"
#include "adc.h"


/* global vars */
static DevData gDevData;
static DevCfg gDevCfg;


static void hw_io_init(void) {
    /* io clock enable */
    M0P_CLOCK->PERI_CLKEN_f.GPIO = 1;
}

static void detec_loop(void) {
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
}


/* ################# main function #################   */
int board_init(void) {
    hw_delay_init();
    hw_io_init();
    appcfg_init(MAGIC_CODE);
    load_config();
    /* peripheral init */
    sys_task_reg_timer(2000, detec_loop);
    sys_task_reg_alarm(1024, delay_exec);
    return 0;
}
