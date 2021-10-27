#include <stdint.h>
#include "hw_bare.h"


void hw_delay_us(uint32_t us) {
    for(uint32_t i=0; i<us; i++) {
        for(int k=0; k<100; k++) {
            __NOP();
        }
    }
}
