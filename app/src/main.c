#include <sys/build_time.h>
#include <sys/gpio.h>
#include <sys/sys.h>
#include <sys/version.h>

#include "uart.h"

system_context_t xdata sysctx __at(SYSTEM_CONTEXT_ADDR);
uint16_t counter_1ms = 0;

void isp_handle(void);

void t0_isr() INTERRUPT(TMR0_VECTOR) {
    if (++counter_1ms == 1000) {
        counter_1ms = 0;
        led_run_toggle();
    }
}

void main() {
    enable_xsfr();
    gpio_init();
    pin_pu(3, 2);  // KEY_BOOT 上拉

    t0_mode0_16bit_auto_reload();
    t0_1t();
    t0_as_timer();
    t0_load(T1MS);
    t0_enable_irq();
    t0_run();

    uart_init();
    enable_irq();
    led_run_on();

    sysctx.st.dfu = 0;

    debugf1("App start");

    while (1) {
        uart_run();
        if (isp_parse_ok) {
            isp_parse_ok = false;
            isp_handle();
        }

        if (key_boot_pressed()) {
            sysctx.st.dfu = 1;
            sys_reset();
        }
    }
}

void isp_handle(void) {
    uint16_t addr = rx.pkt.addr;
    tx.pkt.status = LDR_STATUS_OK;
    tx.pkt.size = 0;

    switch (rx.pkt.cmd) {
        case ISP_CMD_CONNECT:
            tx.pkt.size = 2;
            tx.pkt.dat[0] = APP_VERSION_MAJOR;
            tx.pkt.dat[1] = APP_VERSION_MINOR;
            break;
        case ISP_CMD_REBOOT:
            sys_reset();
            break;
        case ISP_CMD_READ_CHIP_INFO:
            tx.pkt.status = LDR_STATUS_CHIP_INFO;
            tx.pkt.size = sizeof(stc_chipid_t);
            *(stc_chipid_t *)tx.pkt.dat = STC_CHIPID();
            break;
        case ISP_CMD_READ_CHIP_VERSION:
            tx.pkt.status = LDR_STATUS_CHIP_VERSION;
            tx.pkt.size = 1;
            tx.pkt.dat[0] = stc_chip_version();
            break;
        case ISP_CMD_READ_APP_VERSION:
            tx.pkt.status = LDR_STATUS_APP_VERSION;
            tx.pkt.size = 8;
            *(uint32_t *)&tx.pkt.dat[0] = APP_VERSION;
            *(uint32_t *)&tx.pkt.dat[4] = APP_BUILD_TIME;
            break;
        default:
            tx.pkt.status = LDR_STATUS_UNKNOWN_CMD;
            break;
    }
    uart_send_tx();
}
