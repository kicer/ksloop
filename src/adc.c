#include <stdint.h>
#include "adc.h"
#include "hw_bare.h"


int adc_init(void) {
    hal_adc_init();
    M0P_CLOCK->PERI_CLKEN_f.ADC = 1; /* adc clock */
    M0P_BGR->CR_f.BGR_EN = 1; /* bgr enable */
    M0P_BGR->CR_f.TS_EN = 1; /* TS enable */
    M0P_ADC->CR0 = 0x01; /* adc.cr0 enable */
    M0P_ADC->CR1 = 0x00; /* adc.cr1 init */
    M0P_ADC->CR2 = 0x00; /* adc.cr2 init */
    ADC_DELAY();
    M0P_ADC->CR0_f.SREF = 0; /* 1.5V */
    M0P_ADC->CR0_f.SAM = 3; /* 12cycle */
    M0P_ADC->CR0_f.CLKSEL = 3; /* PCLK/8 */
    M0P_ADC->CR0_f.BUFEN = 1; /* adc op.buf */
    return 0;
}

int adc_deinit(void) {
    M0P_ADC->CR0_f.ADCEN = 0;
    M0P_BGR->CR_f.BGR_EN = 0;
    M0P_BGR->CR_f.TS_EN = 0;
    M0P_CLOCK->PERI_CLKEN_f.ADC = 0;
    hal_adc_deinit();
    return 0;
}

uint16_t adc_convert(uint8_t mux, uint8_t cnt) {
    uint32_t result;
    if(cnt == 1) { /* 单次转换模式 */
        M0P_ADC->CR1_f.CT = 0; /* signal mode */
        M0P_ADC->CR0_f.SEL = mux;
        M0P_ADC->CR0_f.START = 1;
        while(M0P_ADC->CR0_f.START == 1){};
        return M0P_ADC->RESULT;
    } else { /* 连续转换累加模式 */
        M0P_ADC->CR1_f.CT = 1; /* continue mode */
        M0P_ADC->CR2 = (1UL<<mux);
        M0P_ADC->CR2_f.ADCCNT = cnt-1;
        /* acc enable */
        M0P_ADC->CR1_f.RACC_EN = 1;
        M0P_ADC->CR1_f.RACC_CLR = 0; /* clear ACC */
        /* start */
        M0P_ADC->ICLR_f.CONT_INTC = 0; /* clear CONT_INTF */
        M0P_ADC->CR0_f.STATERST = 1;
        M0P_ADC->CR0_f.START = 1;
        while(M0P_ADC->IFR_f.CONT_INTF == 0){};
        M0P_ADC->ICLR_f.CONT_INTC = 0; /* clear CONT_INTF */
        /* clean */
        result = M0P_ADC->RESULT_ACC;
        M0P_ADC->CR1_f.RACC_CLR = 0; /* clear ACC */
    }
    return (uint16_t)(result/cnt);
}

uint16_t adc_ts(void) {
    uint16_t ts = adc_convert(10,1);
    uint16_t trim = (*((volatile uint16_t*) (0x00100C34ul)));
    //ts = 25+0.0839*1.5*(ts-trim);
    if(ts>trim) { /* x10 */
        ts = 250+126*(ts-trim)/100;
    } else {
        ts = 250-126*(trim-ts)/100;
    }
    return ts;
}
