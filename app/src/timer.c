#include "timer.h"

bool flag_1s = false;

static uint16_t counter_1ms = 0;

void t0_isr() INTERRUPT(TMR0_VECTOR) {
    if (++counter_1ms == 1000) {
        counter_1ms = 0;
        flag_1s = true;
    }
}

void timer0_init(void) {
    t0_mode0_16bit_auto_reload();
    t0_1t();
    t0_as_timer();
    t0_load(T1MS);
    t0_enable_irq();
    t0_run();
}
