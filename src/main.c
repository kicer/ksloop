#include <stdint.h>

#include "hw_bare.h"
#include "board.h"


static void app_clk_init(void) {
    /* 系统时钟
     * 上电后默认为内部RCH时钟4MHz，
     * 需要同时使能RCL=32.768kHz
     */
    /* step 1: startup=256, load trim */
    M0P_CLOCK->RCL_CR = (*cast(volatile uint16_t *, 0x00100C22ul)) |
                        (3<<10);
    /* step 2: rcl en */
    M0P_CLOCK->SYSCTRL2 = 0x5A5A;
    M0P_CLOCK->SYSCTRL2 = 0xA5A5;
    M0P_CLOCK->SYSCTRL0_f.RCL_EN = 1;
    /* step 3: wait RCL stable */
    while(1!=M0P_CLOCK->RCL_CR_f.STABLE) {};
    /* step 4: wait RCH stable */
    while(1!=M0P_CLOCK->RCH_CR_f.STABLE) {};
}


int main(void)
{
    /* Initialization of the clock */
    app_clk_init();
    sys_init();
    board_init();
    /* system run */
    enInterrupts();
    sys_run();
    /* hard fault! */
    while(1) swReset(); /* just reboot */
    return 0;
}
