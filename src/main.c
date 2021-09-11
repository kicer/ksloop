#include "ch55x.h"
#include "sys.h"
#include "board.h"


static void app_clk_init(void) {
    /* todo */
}

void main(void)
{
    /* Initialization of the clock */
    app_clk_init();
    /* system init */
    sys_init();
    board_init();
    /* system run */
    BSP_enableInterrupt();
    sys_run();
    /* hard fault! */
    while(1) BSP_SWReset(); /* just reboot */
}
