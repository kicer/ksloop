#include "hw_bare.h"
#include "board.h"


#define LED_RED     IO_P14
#define LED_GREEN   IO_P15
static void hw_io_init(void) {
    M0P_CLOCK->PERI_CLKEN_f.GPIO = 1;
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinFunc(LED_RED, PUSH_PULL);
    pinFunc(LED_GREEN, PUSH_PULL);
    digitalWrite(LED_RED, 1);
    digitalWrite(LED_GREEN, 1);
}

static void loop_main(void) {
    if(digitalRead(LED_RED)) {
        digitalWrite(LED_RED, 0);
        digitalWrite(LED_GREEN, 1);
    } else {
        digitalWrite(LED_RED, 1);
        digitalWrite(LED_GREEN, 0);
    }
}

/* ################# main functions #################   */
int board_init(void) {
    hw_io_init();
    sys_task_reg_timer(1000, loop_main);
    return 0;
}
