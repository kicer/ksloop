/*
** $Id: hwb_gpio.h $
** hwb_gpio - hw_bare gpio module
**
** Arduino-style Functions:
**      pinMode(pin,mode)
**      pinFunc(pin,func)
**      digitalWrite(pin,value)
**      digitalRead(pin)
**      attachInterrupt(pin, mode)
**      detachInterrupt(pin)
**
** Arduino-style IOs:
**      IO_Pxy
*/

#ifndef _HWB_GPIO_H_
#define _HWB_GPIO_H_


#include <stdint.h>

#include "hc32lx.h"


/* pinMode(pin,mode)
**  : pinMode(IO_P35, INPUT)
**  : pinMode(IO_P35, OUTPUT)
*/
#define pinMode(P,m)                pinMode_##m(P)
#define pinMode_INPUT(o,p)          gpio_init_afin(o,p,0)
#define pinMode_OUTPUT(o,p)         gpio_init_afout(o,p,0)

/* pinFunc(pin,func)
**  : pinFunc(IO_P35, PULLUP)       # INPUT
**  : pinFunc(IO_P35, PULLDOWN)     # INPUT
**  : pinFunc(IO_P35, FLOATING)     # INPUT
**  : pinFunc(IO_P35, PUSH_PULL)    # OUTPUT
**  : pinFunc(IO_P35, OPEN_DRAIN)   # OUTPUT
**  : pinFunc(IO_P35, AFx)          # Alternate Function
**  : pinFunc(IO_P35, DIR_IN)       # DIR IN
**  : pinFunc(IO_P35, DIR_OUT)      # DIR OUT
**  : pinFunc(IO_P35, ANALOG)       # ANALOG Read
*/
#define pinFunc(P,f)                pinFunc_##f(P)
#define pinFunc_PULLUP(o,p)         gpio_pull_up(o,p,1)
#define pinFunc_PULLDOWN(o,p)       gpio_pull_down(o,p,1)
#define pinFunc_FLOATING(o,p)       gpio_floating(o,p)
#define pinFunc_PUSH_PULL(o,p)      gpio_open_drain(o,p,0)
#define pinFunc_OPEN_DRAIN(o,p)     gpio_open_drain(o,p,1)
#define pinFunc_ANALOG(o,p)         gpio_analog(o,p)
#define pinFunc_DIR_IN(o,p)         gpio_dir(o,p,1)
#define pinFunc_DIR_OUT(o,p)        gpio_dir(o,p,0)
#define pinFunc_AF0(o,p)            gpio_afsel(o,p,0)
#define pinFunc_AF1(o,p)            gpio_afsel(o,p,1)
#define pinFunc_AF2(o,p)            gpio_afsel(o,p,2)
#define pinFunc_AF3(o,p)            gpio_afsel(o,p,3)
#define pinFunc_AF4(o,p)            gpio_afsel(o,p,4)
#define pinFunc_AF5(o,p)            gpio_afsel(o,p,5)
#define pinFunc_AF6(o,p)            gpio_afsel(o,p,6)
#define pinFunc_AF7(o,p)            gpio_afsel(o,p,7)

/* digitalWrite(pin,value)
**  : digitalWrite(IO_P35, LOW)     # or 0
**  : digitalWrite(IO_P35, HIGH)    # or 1
*/
#define digitalWrite(P,m)           digitalWrite_##m(P)
#define digitalWrite_0(o,p)         gpio_out(o,p,0)
#define digitalWrite_1(o,p)         gpio_out(o,p,1)
#define digitalWrite_LOW(o,p)       gpio_out(o,p,0)
#define digitalWrite_HIGH(o,p)      gpio_out(o,p,1)

/* digitalRead(pin)
**  : digitalWrite(IO_P35)
*/
#define digitalRead(P)              gpio_read(P)

/* attachInterrupt(pin, mode)
**  : attachInterrupt(IO_P35, RISING)
**  : attachInterrupt(IO_P35, FALLING)
**  : attachInterrupt(IO_P35, CHANGE)
*/
#define attachInterrupt(P,m)         attachInterrupt_##m(P)
#define attachInterrupt_RISING(o,p)  gpio_ie_enable(o,p,RIE)
#define attachInterrupt_FALLING(o,p) gpio_ie_enable(o,p,FIE)
#define attachInterrupt_CHANGE(o,p)  gpio_it_enable(o,p)

/* attachInterrupt(pin)
**  : attachInterrupt(IO_P35)
*/
#define detachInterrupt(P)          gpio_it_disable(P)

/* pin.x */
#define IO_P35          P3,5
#define IO_P36          P3,6
#define IO_P00          P0,0
#define IO_P01          P0,1
#define IO_P02          P0,2
#define IO_P15          P1,5
#define IO_P14          P1,4
#define IO_P23          P2,3
#define IO_P24          P2,4
#define IO_P25          P2,5
#define IO_P26          P2,6
#define IO_P27          P2,7
#define IO_P31          P3,1

/* pin dir */
#define DIR_IN          1
#define DIR_OUT         0
/* port mode */
#define PORT_AD         1
#define PORT_IO         0

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

/* Analog, for ADC */
#define gpio_analog(port, pin) do {             \
    M0P_GPIO->port##ADS_f.port##pin = PORT_AD;  \
} while(0)

/* pin direction */
#define gpio_dir(port, pin, dir) do {       \
    M0P_GPIO->port##DIR_f.port##pin = dir;  \
    } while(0)

/* pin function select */
#define gpio_afsel(port, pin, af) do {      \
    M0P_GPIO->port##pin##_SEL = af;         \
    } while(0)

/* pin out x */
#define gpio_out(port, pin, val) do {       \
    M0P_GPIO->port##OUT_f.port##pin = val;  \
    } while(0)

/* Pull-up res */
#define gpio_pull_up(port, pin, val) do {   \
    M0P_GPIO->port##PU_f.port##pin = val;   \
    } while(0)

/* Pull-down res */
#define gpio_pull_down(port, pin, val) do { \
    M0P_GPIO->port##PD_f.port##pin = val;   \
    } while(0)

/* No pull-up/down res */
#define gpio_floating(port, pin) do {       \
    M0P_GPIO->port##PU_f.port##pin = 0;     \
    M0P_GPIO->port##PD_f.port##pin = 0;     \
    } while(0)

/* Open-drain */
#define gpio_open_drain(port, pin, val) do { \
    M0P_GPIO->port##OD_f.port##pin = val;    \
    } while(0)

/* pin read */
#define gpio_read(port, pin)  ((M0P_GPIO->port##IN_f.port##pin) != 0)

/* Interrupt enable */
/* HIE/LIE/RIE/FIE */
#define gpio_ie_enable(port, pin, ie) do {      \
    M0P_GPIO->port##ie##_f.port##pin = 1;       \
} while(0)
#define gpio_it_enable(port, pin) do {          \
    M0P_GPIO->port##RIE_f.port##pin = 1;        \
    M0P_GPIO->port##FIE_f.port##pin = 1;        \
} while(0)

/* Interrupt disable */
/* HIE/LIE/RIE/FIE */
#define gpio_ie_disable(port, pin, ie) do {     \
    M0P_GPIO->port##ie##_f.port##pin = 0;       \
} while(0)
#define gpio_it_disable(port, pin) do {         \
    M0P_GPIO->port##RIE_f.port##pin = 0;        \
    M0P_GPIO->port##FIE_f.port##pin = 0;        \
} while(0)


#endif /* _HWB_GPIO_H_ */
