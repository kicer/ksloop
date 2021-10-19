#include <stdint.h>
#include "hc32lx.h"
#include "hc32lx_bare.h"


volatile uint32_t systick = 0;
/* 系统时钟
 * 采用内部RCH时钟作为主时钟，上电后默认为4MHz
 * 进入DeepSleep模式后会自动关闭
 */
volatile uint32_t core_clock_hz = 4000000; /* 4MHz */
/* RTC时钟
 * 超低功耗外设使用外部XTL晶振作为其时钟
 */


#define BUZZER_PIN    7
int main (void) {
    SysTick_Config( core_clock_hz / 1000 );

    M0P_SYSCTRL->PERI_CLKEN_f.GPIO = 1;
    M0P_GPIO->PA07_SEL = 0;
    M0P_GPIO->PADIR_f.PA07 = 0;
    M0P_GPIO->PAOUT_f.PA07 = 1;

    while(1) {
        uint32_t next = systick + 1000;
        while(systick<next) {
            __asm__("WFI");
        }
        M0P_GPIO->PAOUT ^= BIT(BUZZER_PIN);
    }

    return 0;
}


void SysTick_IRQn_handler(void) {
    ++systick;
}
