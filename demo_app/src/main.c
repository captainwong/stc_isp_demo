#include "sys/gpio.h"
#include "sys/sys.h"

uint32_t xdata dfuflag __at(0x1FFC);  // DFU标志, 定义在xdata的最后4字节
uint16_t counter_1ms = 0;

void t0_isr() INTERRUPT(TMR0_VECTOR) {
    if (++counter_1ms == 500) {
        counter_1ms = 0;
        led_run_toggle();
    }
}

void main() {
    enable_xsfr();
    gpio_init();
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
        if (key_boot_pressed()) {
            dfuflag = DFU_TAG;  // 当需要执行用户ISP代码时,将强制执行标志赋值到DFU标志变量中
            sys_reset();
        }
    }
}
