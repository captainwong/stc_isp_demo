#include "uart.h"

void uart_init(void) {
    // uart1_use_p30_p31();
    // pin_mode_io_pup(3, 0);
    // pin_mode_io_pup(3, 1);
    // uart1_brt_use_timer1();
    // uart1_mode1_8bit_brtx();
    // uart1_disable_irq();
    // uart1_enable_recv();
    // RI = 0;
    // TI = 1;

    // t1_1t();
    // t1_as_timer();
    // t1_load(uart_calc_load_1t(MAIN_Fosc, UART1_BAUD));
    // t1_disable_irq();
    // t1_run();

    AUXR &= ~0x01;
    SCON = 0x52;

    AUXR |= 0x40;
    TMOD &= ~0xf0;
    t1_load(uart_calc_load_1t(MAIN_Fosc, UART1_BAUD));
    TR1 = 1;
}

uint8_t uart_send(uint8_t dat) {
    while (!TI);
    TI = 0;
    SBUF = dat;

    return dat;
}
