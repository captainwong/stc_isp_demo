#include "protocol.h"
#include "sys/gpio.h"
#include "uart.h"

uint32_t xdata dfutag __at(DFU_ADDR);
uint16_t cycles;

void isp_handle(void);

void delay() {
    volatile uint16_t data i = 0;
#ifdef DEBUG
    volatile uint8_t data j = 0;
    while (++i) {
        while (++j) {
            nop1();
        }
    }
#else
    while (++i) {
        nop1();
    }
#endif
}

void debug_led(void) {
    LED_DEBUG1 = ctx.state != ISP_PARSE_STATE_IDLE;
    LED_DEBUG2 = ctx.state != ISP_PARSE_STATE_LENGTH;
    LED_DEBUG3 = ctx.state != ISP_PARSE_STATE_BODY;
    LED_DEBUG4 = ctx.state != ISP_PARSE_STATE_END;
    LED_DEBUG5 = ctx.state != ISP_PARSE_STATE_CHECKSUM;
}

void main() {
    delay();
    disable_irq();
    isp_parse_init(ctx);
    gpio_init();
    pin_pu(3, 5);
    uart_init();

    if ((dfutag != DFU_TAG) &&
        (*(uint8_t code *)(LDR_SIZE) == 0x02) &&               // check if first op code is `LJMP addr16`
        (*(uint16_t code *)(LDR_SIZE + 1) >= LDR_SIZE + 3)) {  // check if `addr16 >= LDR_SIZE + 3`
        dfutag = 0;                                            // clear force DFU mode flag
        ((void(code *)())(LDR_SIZE))();                        // LJMP #LDR_SIZE, from here the CPU is running application code
    }

    // now CPU is running bootloader code

    dfutag = 0;     // clear force DFU mode flag
    enable_xsfr();  // for xsfr `IAP_TPS`
    iap_tps(iap_calc_tps(MAIN_Fosc));

    uart_send(0xAA);
    uart_send(0xBB);
    uart_send(0xCC);
    cycles = 0;

    while (1) {
        debug_led();
        if (RI) {
            uint8_t dat;
            RI = 0;
            dat = SBUF;
            uart_parse(dat);
            if (isp_parse_ok) {
                isp_parse_ok = false;
                isp_handle();
            }
        }

        if (++cycles == 0) {
            led_run_toggle();
        }

        if (key_reboot_pressed()) {
            sys_reset();
            while (1);
        }
    }
}

void isp_handle(void) {
    uint16_t addr = rx.pkt.addr;
    tx.pkt.status = LDR_STATUS_OK;
    tx.pkt.size = 0;

    // uart_send(0xAA);
    switch (rx.pkt.cmd) {
        case ISP_CMD_CONNECT:
            tx.pkt.size = 2;
            tx.pkt.dat[0] = LDR_VERSION >> 8;
            tx.pkt.dat[1] = LDR_VERSION;
            break;
        case ISP_CMD_READ:
            tx.pkt.size = 128;
            iap_read_bytes(addr, tx.pkt.dat, 128);
            break;
        case ISP_CMD_PROGRAM:
            if (!iap_write_bytes_check(addr, rx.pkt.dat, rx.pkt.size)) {
                tx.pkt.status = LDR_STATUS_PROGRAM_FAILED;
            }
            break;
        case ISP_CMD_ERASE:
            for (addr = 0; addr < 0xF000; addr += IAP_PAGE_SIZE) {
                if (!iap_erase_page_check(addr)) {
                    tx.pkt.status = LDR_STATUS_PROGRAM_FAILED;
                    break;
                }
            }
            break;
        case ISP_CMD_REBOOT:
            sys_reset();
            break;
        default:
            tx.pkt.status = LDR_STATUS_UNKNOWN_CMD;
            break;
    }
    uart_send_tx();
}
