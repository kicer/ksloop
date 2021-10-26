#include "rtc.h"
#include "hw_bare.h"


volatile uint32_t g_time = 0;
volatile uint8_t g_time_sync = 0;


uint32_t time_ts(void) {
    return g_time;
}

int is_time_sync(void) {
    return g_time_sync;
}

int time_sync(uint32_t ts) {
    g_time = ts;
    g_time_sync = 1;
    return 0;
}

int rtc_init(void) {
    gpio_analog_port(PC,14);
    gpio_analog_port(PC,15);
    M0P_SYSCTRL->SYSCTRL2 = 0x5A5A;
    M0P_SYSCTRL->SYSCTRL2 = 0xA5A5;
    M0P_SYSCTRL->SYSCTRL0_f.XTL_EN = 1;
    while(1 != M0P_SYSCTRL->XTL_CR_f.STABLE){};
    /* config... */
    M0P_SYSCTRL->PERI_CLKEN_f.RTC = 1;
    M0P_RTC->CR0_f.START = 0;
    M0P_RTC->CR0_f.AMPM = 1;
    M0P_RTC->CR0_f.PRDSEL = 1; /* clock:xtl */
    M0P_RTC->CR0_f.PRDX = 1;   /* period:1s */
    M0P_RTC->CR1_f.CKSEL = 0;
    /* init time */
    M0P_RTC->SEC   = 0x00;
    M0P_RTC->MIN   = 0x00;
    M0P_RTC->HOUR  = 0x00;
    M0P_RTC->DAY   = 0x01;
    M0P_RTC->MON   = 0x01;
    M0P_RTC->YEAR  = 0x00;
    M0P_RTC->WEEK  = 0x06;
    /* rtc interrupt enable */
    M0P_RTC->CR1_f.PRDF = 0;
    M0P_RTC->CR1_f.ALMF = 0;
    M0P_RTC->CR1_f.ALMIE = 1;
    /* irq setting */
    nvic_irq_enable(RTC_IRQn, 0);
    /* start */
    M0P_RTC->CR0_f.START = 1;
    /* wait... */
    M0P_RTC->CR1_f.WAIT = 1;
    while(M0P_RTC->CR1_f.WAITF != 1) {};
    M0P_RTC->CR1_f.WAIT = 0;
    while(M0P_RTC->CR1_f.WAITF != 0) {};
    return 0;
}

int rtc_deinit(void) {
    nvic_irq_disable(RTC_IRQn);
    M0P_SYSCTRL->PERI_CLKEN_f.RTC = 0;
    M0P_SYSCTRL->SYSCTRL0_f.XTL_EN = 0;
    gpio_init_in(PC,14);
    gpio_init_in(PC,15);
    gpio_floating(PC,14);
    gpio_floating(PC,15);
    return 0;
}

void RTC_IRQn_handler(void) {
    if(M0P_RTC->CR1_f.PRDF) {
        M0P_RTC->CR1_f.PRDF = 0;
        uint32_t _ts = g_time;
        _ts += 1;
        g_time = _ts;
    }
}
