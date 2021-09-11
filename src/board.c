#include "ch55x.h"
#include "log.h"
#include "sys.h"
#include "uart.h"
#include "eeprom.h"
#include "board.h"


static void delay_exec(void) {
    /* logger */
    //log_init(LOG_DEBUG);
    //dmesg_hex(LOG_INFO, "4G-WSM v4:", (uint8_t *)gDevCfg, sizeof(gDevCfg));
}


/* ################# main function #################   */
int board_init(void) {
    sys_task_reg_alarm(1100, delay_exec);
    return 0;
}
