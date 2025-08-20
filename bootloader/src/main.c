#include "dfu.h"
#include "uart.h"

void main() {
    enable_xsfr();
    iap_tps(iap_calc_tps(MAIN_Fosc));

    dfu_check();
    uart_init();

    while (1) {
        uart_isr();
        dfu_events();
    }
}
