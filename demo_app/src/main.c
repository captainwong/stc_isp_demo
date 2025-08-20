#include "sys/sys.h"

uint32_t xdata dfuflag __at(0x1FFC);  // DFU标志, 定义在xdata的最后4字节
uint16_t counter_1ms = 0;

void tm0_isr() INTERRUPT(TMR0_VECTOR) {
    if (++counter_1ms == 500) {
        counter_1ms = 0;
        P2 = ~P2;
    }
}

void main() {
    enable_xsfr();

    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    pin_pu(3, 2);

    t0_mode0_16bit_auto_reload();
    t0_1t();
    t0_as_timer();
    t0_load(T1MS);
    t0_enable_irq();
    t0_run();

    enable_irq();

    dfuflag = 0;

    while (1) {
        if (P32 == 0) {
            dfuflag = DFU_TAG;  // 当需要执行用户ISP代码时,将强制执行标志赋值到DFU标志变量中
            sys_reset();
        }
    }
}
