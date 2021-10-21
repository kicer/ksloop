#include <stdint.h>
#include "board.h"
#include "sys.h"

#include "hw_bare.h"


static void app_clk_init(void) {
    /* 系统时钟
     * 采用内部RCH时钟作为主时钟，上电后默认为4MHz
     * 进入DeepSleep模式后会自动关闭
     */
#if 0
    /* switch to 24MHz */
     * 切换成24MHz，以便适配串口波特率115200
     */
    M0P_SYSCTRL->SYSCTRL2 = 0x5A5A;
    M0P_SYSCTRL->SYSCTRL2 = 0xA5A5;
    M0P_SYSCTRL->SYSCTRL0_f.HCLK_PRS = 7;
    M0P_SYSCTRL->RCH_CR = *((volatile uint16_t *)(0x00100C08ul)); /* 4M */
    M0P_SYSCTRL->RCH_CR = *((volatile uint16_t *)(0x00100C06ul)); /* 8M */
    M0P_SYSCTRL->RCH_CR = *((volatile uint16_t *)(0x00100C04ul)); /* 16M */
    M0P_SYSCTRL->RCH_CR = *((volatile uint16_t *)(0x00100C00ul)); /* 24M */
    M0P_SYSCTRL->SYSCTRL2 = 0x5A5A;
    M0P_SYSCTRL->SYSCTRL2 = 0xA5A5;
    M0P_SYSCTRL->SYSCTRL0_f.HCLK_PRS = 0;
    /* wait stable */
    while(1 != M0P_SYSCTRL->RCH_CR_f.STABLE){};
#endif
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
