#include <stdint.h>
#include "hw_bare.h"
#include "sys.h"


volatile static int64_t _sys_ticks = 0;
volatile static uint32_t _sys_ticks_1us = 0;
volatile static uint32_t _sys_ticks_load = 0;


int64_t hw_uptime(void) {
    /* lock */
    uint32_t temp=__get_PRIMASK();
    __disable_irq();
    uint32_t VAL = SysTick->VAL;
    int64_t t0 = _sys_ticks;
    __set_PRIMASK(temp);
    /* lock */
    return t0+_sys_ticks_load-VAL;
}

void hw_delay_us(uint32_t us) {
    /* lock */
    uint32_t temp=__get_PRIMASK();
    __disable_irq();
    uint32_t VAL = SysTick->VAL;
    int64_t t0 = _sys_ticks;
    __set_PRIMASK(temp);
    /* lock */
    uint32_t LOAD = _sys_ticks_load;
    int64_t wait = us*_sys_ticks_1us;
    if(wait > VAL) { /* 等待时间>VAL，则先等待_sys_ticks就位 */
        uint32_t last = (wait-VAL)%LOAD;
        int64_t big_wait = t0+LOAD+wait-VAL-last;
        VAL = LOAD; wait = last; t0 = big_wait;
        while(_sys_ticks<big_wait);
    }
    if(1) { /* 等待时间<VAL，则直接等待 */
        uint32_t tmp = VAL-wait;
        while(SysTick->VAL > tmp) {
            /* 如果重载_sys_ticks，则肯定已经超时了 */
            if(_sys_ticks > t0) break;
        }
    }
}

void hw_delay_init(void) {
    _sys_ticks_1us = (CORE_CLOCK_HZ/1000000ul);
    _sys_ticks_load = SysTick->LOAD+1;
}

void SysTick_IRQn_handler(void) {
    _sys_ticks += _sys_ticks_load;
    sys_event_trigger(EVENT_SYSTICKS);
}
