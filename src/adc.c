#include <stdint.h>
#include "adc.h"
#include "hw_bare.h"


int adc_init(void) {
    hal_adc_init();
    M0P_SYSCTRL->PERI_CLKEN_f.ADC = 1; /* adc clock */
    M0P_BGR->CR_f.BGR_EN = 1; /* bgr enable */
    M0P_ADC->CR0 = 0x01; /* adc enable */
    ADC_DELAY();
    M0P_ADC->CR1_f.MODE = 1; /* continue mode */
    M0P_ADC->CR0_f.REF = 3; /* AVCC */
    M0P_ADC->CR0_f.SAM = 3; /* 12cycle */
    M0P_ADC->CR0_f.CLKDIV = 3; /* PCLK/8 */
    M0P_ADC->CR0_f.BUF = 1; /* adc op.buf */
    return 0;
}

int adc_deinit(void) {
    M0P_ADC->CR0_f.EN = 0;
    M0P_BGR->CR_f.BGR_EN = 0;
    M0P_SYSCTRL->PERI_CLKEN_f.ADC = 0;
    hal_adc_deinit();
    return 0;
}

uint16_t adc_convert(uint8_t mux, uint8_t cnt) {
    uint32_t result;
    if(cnt > 15) M0P_ADC->SQR2_f.CH15MUX = mux;
    if(cnt > 14) M0P_ADC->SQR2_f.CH14MUX = mux;
    if(cnt > 13) M0P_ADC->SQR2_f.CH13MUX = mux;
    if(cnt > 12) M0P_ADC->SQR2_f.CH12MUX = mux;
    if(cnt > 11) M0P_ADC->SQR1_f.CH11MUX = mux;
    if(cnt > 10) M0P_ADC->SQR1_f.CH10MUX = mux;
    if(cnt >  9) M0P_ADC->SQR1_f.CH9MUX = mux;
    if(cnt >  8) M0P_ADC->SQR1_f.CH8MUX = mux;
    if(cnt >  7) M0P_ADC->SQR1_f.CH7MUX = mux;
    if(cnt >  6) M0P_ADC->SQR1_f.CH6MUX = mux;
    if(cnt >  5) M0P_ADC->SQR0_f.CH5MUX = mux;
    if(cnt >  4) M0P_ADC->SQR0_f.CH4MUX = mux;
    if(cnt >  3) M0P_ADC->SQR0_f.CH3MUX = mux;
    if(cnt >  2) M0P_ADC->SQR0_f.CH2MUX = mux;
    if(cnt >  1) M0P_ADC->SQR0_f.CH1MUX = mux;
    if(cnt >  0) M0P_ADC->SQR0_f.CH0MUX = mux;
    M0P_ADC->SQR2_f.CNT = cnt-1;
    M0P_ADC->CR1_f.RACCEN = 1;
    M0P_ADC->CR1_f.RACCCLR = 0; /* clear ACC */
    M0P_ADC->ICR_f.SQRIC = 0; /* clear SQRIF */
    M0P_ADC->SQRSTART = 1; /* start */
    while(M0P_ADC->IFR_f.SQRIF==0){};
    M0P_ADC->ICR_f.SQRIC = 0; /* clear SQRIF */
    result = M0P_ADC->RESULTACC;
    return (uint16_t)(result/cnt);
}
