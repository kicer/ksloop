#ifndef _HW_BARE_H_
#define _HW_BARE_H_


#include <stdint.h>

/* porting with ur mcu */
#include "hc32lx.h"

#define CORE_CLOCK_HZ           4000000

#define BIT(x)                  (1UL << (x))
#define try_param(x)            ((void)x)

#define BSP_enableInterrupt     __enable_irq
#define BSP_SWReset             NVIC_SystemReset
#define BSP_wfi()               __asm__("WFI")

/* ################### GPIO Action ######################## */
#define DIR_IN                  1
#define DIR_OUT                 0

#define PORT_AD                 1
#define PORT_IO                 0

#define PORT_IT_HIGH            HIE
#define PORT_IT_LOW             LIE
#define PORT_IT_RISE            RIE
#define PORT_IT_FALL            FIE

/* General out io init */
#define gpio_init_out(port, pin, val) do {     \
    M0P_GPIO->port##ADS_f.port##pin = PORT_IO; \
    M0P_GPIO->port##pin##_SEL = 0;             \
    M0P_GPIO->port##DIR_f.port##pin = DIR_OUT; \
    M0P_GPIO->port##OUT_f.port##pin = val;     \
} while(0)

/* General in io init */
#define gpio_init_in(port, pin) do {           \
    M0P_GPIO->port##ADS_f.port##pin = PORT_IO; \
    M0P_GPIO->port##pin##_SEL = 0;             \
    M0P_GPIO->port##DIR_f.port##pin = DIR_IN;  \
} while(0)

/* General AF-out io init */
#define gpio_init_afout(port, pin, af) do {    \
    M0P_GPIO->port##ADS_f.port##pin = PORT_IO; \
    M0P_GPIO->port##pin##_SEL = af;            \
    M0P_GPIO->port##DIR_f.port##pin = DIR_OUT; \
} while(0)

/* General AF-in io init */
#define gpio_init_afin(port, pin, af) do {     \
    M0P_GPIO->port##ADS_f.port##pin = PORT_IO; \
    M0P_GPIO->port##pin##_SEL = af;            \
    M0P_GPIO->port##DIR_f.port##pin = DIR_IN;  \
} while(0)

/* Interrupt enable */
#define gpio_it_enable(port, pin, it) do {     \
    M0P_GPIO->port##it_f.port##pin = 1;        \
} while(0)

/* Interrupt disable */
#define gpio_it_disable(port, pin, it) do {    \
    M0P_GPIO->port##it_f.port##pin = 0;        \
} while(0)

/* Pull-up res */
#define gpio_pull_up(port, pin) do {    \
    M0P_GPIO->port##PU_f.port##pin = 1; \
} while(0)

/* Pull-down res */
#define gpio_pull_down(port, pin) do {  \
    M0P_GPIO->port##PU_f.port##pin = 0; \
    M0P_GPIO->port##PD_f.port##pin = 1; \
} while(0)

/* Pull-down res */
#define gpio_floating(port, pin) do {   \
    M0P_GPIO->port##PU_f.port##pin = 0; \
    M0P_GPIO->port##PD_f.port##pin = 0; \
    M0P_GPIO->port##OD_f.port##pin = 0; \
} while(0)

/* Open-drain */
#define gpio_open_drain(port, pin) do { \
    M0P_GPIO->port##OD_f.port##pin = 1; \
} while(0)

/* pin out 1 */
#define gpio_out_1(port, pin) do {        \
    M0P_GPIO->port##BSET_f.port##pin = 1; \
} while(0)

/* pin out 0 */
#define gpio_out_0(port, pin) do {        \
    M0P_GPIO->port##BCLR_f.port##pin = 1; \
} while(0)

/* pin out x */
#define gpio_out(port, pin, val) do {     \
    if(val) M0P_GPIO->port##BSET_f.port##pin = 1; \
    else M0P_GPIO->port##BCLR_f.port##pin = 1; \
} while(0)

/* pin read */
#define gpio_read(port, pin)  (M0P_GPIO->port##IN_f.port##pin != 0)


#endif /* _HW_BARE_H_ */
