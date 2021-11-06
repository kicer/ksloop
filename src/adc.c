#include <stdint.h>
#include "adc.h"
#include "hw_bare.h"


int adc_init(void) {
    hal_adc_init();
    M0P_CLOCK->PERI_CLKEN_f.ADC = 1; /* adc clock */
    M0P_BGR->CR_f.BGR_EN = 1; /* bgr enable */
    M0P_ADC->CR0 = 0x01; /* adc enable */
    ADC_DELAY();
    M0P_ADC->CR1_f.CT = 1; /* continue mode */
    M0P_ADC->CR0_f.SREF = 2; /* 2.5V */
    M0P_ADC->CR0_f.SAM = 3; /* 12cycle */
    M0P_ADC->CR0_f.CLKSEL = 3; /* PCLK/8 */
    M0P_ADC->CR0_f.BUFEN = 1; /* adc op.buf */
    M0P_ADC->CR1_f.RACC_EN = 1;
    return 0;
}

int adc_deinit(void) {
    M0P_ADC->CR0_f.ADCEN = 0;
    M0P_BGR->CR_f.BGR_EN = 0;
    M0P_CLOCK->PERI_CLKEN_f.ADC = 0;
    hal_adc_deinit();
    return 0;
}

uint16_t adc_convert(uint8_t mux, uint8_t cnt) {
    static uint16_t v = 1024;
    v += 1;
    if(v > 1100) v= 1023;
    return v;
    uint32_t result;
    M0P_ADC->CR2 = (1UL<<mux);
    M0P_ADC->CR2_f.ADCCNT = cnt-1;
    M0P_ADC->CR1_f.RACC_CLR = 0; /* clear ACC */
    M0P_ADC->ICLR_f.CONT_INTC = 0; /* clear CONT_INTF */
    M0P_ADC->CR0_f.STATERST = 1;
    M0P_ADC->CR0_f.START = 1; /* start */
    while(M0P_ADC->IFR_f.CONT_INTF == 0){};
    M0P_ADC->ICLR_f.CONT_INTC = 0; /* clear CONT_INTF */
    result = M0P_ADC->RESULT_ACC;
    return (uint16_t)(result/cnt);
}
