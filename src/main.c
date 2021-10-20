#include <stdint.h>
#include "board.h"
#include "sys.h"

#include "hw_bare.h"


static void app_clk_init(void) {
    /* 系统时钟
     * 采用内部RCH时钟作为主时钟，上电后默认为4MHz
     * 进入DeepSleep模式后会自动关闭
     */
}


int main(void)
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
    return 0;
}
