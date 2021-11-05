#ifndef _ADC_H_
#define _ADC_H_


#include <stdint.h>

extern int adc_init(void);
extern int adc_deinit(void);
extern uint16_t adc_convert(uint8_t mux, uint8_t cnt);

/* adc.port */
/* VBAT:PA04,mux=4, VCHG:PA05,mux=5 */
#include "hw_bare.h"
#define hal_adc_init()  do {   \
    gpio_analog_port(P2,4);    \
} while(0)

#define hal_adc_deinit()  do { \
    gpio_init_in(P2,4);        \
    gpio_floating(P2,4);       \
} while(0)

#define ADC_DELAY()    hw_delay_us(50)


#endif /* _ADC_H_ */
