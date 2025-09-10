#include "bsp/norflash.h"
#include "protocol.h"
#include "sys/build_time.h"
#include "sys/gpio.h"
#include "sys/version.h"
#include "uart.h"

uint32_t xdata dfutag __at(DFU_ADDR);
uint32_t xdata modetag __at(MODE_ADDR);
uint16_t cycles;
uint16_t rx_time, rx_timeout;

void isp_handle(void);

void delay() {
    volatile uint16_t data i = 0;
#ifdef DEBUG
    volatile uint8_t data j;
    while (++i) {
        j = 200;
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

void main() {
    modetag = MODE_TAG;  // indicate current mode is bootloader mode
    delay();
    disable_irq();
    isp_parse_init(ctx);
    gpio_init();
    pin_pu(3, 5);  // KEY_REBOOT 上拉
    uart_init();
    enable_irq();
    led_run_on();

    while (!norflash_init()) {
        debugf1("Norflash init failed");
        led_run_toggle();
        delay_ms(1000);
    }
    debugf3("Norflash init ok, type=%04X, %s", norflash_type, norflash_get_type_string());
    uart_wait_sent();
    debugf2("dfutag=%08lX", dfutag);
    uart_wait_sent();
    debugf4("first 3 byte: %02bX %02bX %02bX",
            *(uint8_t code *)(LDR_SIZE),
            *(uint8_t code *)(LDR_SIZE + 1),
            *(uint8_t code *)(LDR_SIZE + 2));
    uart_wait_sent();
    if ((dfutag != DFU_TAG) && is_valid_on_chip_app_program()) {
        debugf1("Jump to application");
        uart_wait_sent();
        uart_release();
        dfutag = 0;                      // clear force DFU mode flag
        modetag = 0;                     // indicate current mode is application mode
        jump_to_on_chip_app_program(0);  // LJMP #LDR_SIZE, from here the CPU is running application code
    }

    // now CPU is running bootloader code

    debugf1("bootloader running");
    dfutag = 0;     // clear force DFU mode flag
    enable_xsfr();  // for xsfr `IAP_TPS`
    iap_tps(iap_calc_tps(MAIN_Fosc));
    cycles = 0;

    while (1) {
        uart_run();
        if (isp_parse_ok) {
            isp_parse_ok = false;
            rx_time = rx_timeout = cycles;
            isp_handle();
        }

        if (++rx_timeout == rx_time) {  // 转一圈还没有新的数据，说明这一包超时
            isp_parse_init(ctx);
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

    switch (rx.pkt.cmd) {
        case ISP_CMD_CONNECT:
            tx.pkt.size = 2;
            tx.pkt.dat[0] = LDR_VERSION_MAJOR;
            tx.pkt.dat[1] = LDR_VERSION_MINOR;
            break;
        case ISP_CMD_READ:
            tx.pkt.status = LDR_STATUS_ROM;
            tx.pkt.size = rx.pkt.size;
            iap_read_bytes(addr, tx.pkt.dat, rx.pkt.size);
            break;
        case ISP_CMD_PROGRAM:
            if (!iap_write_bytes_check(addr, rx.pkt.dat, rx.pkt.size)) {
                tx.pkt.status = LDR_STATUS_PROGRAM_FAILED;
            }
            break;
        case ISP_CMD_ERASE:
            for (addr = 0; addr < IAP_ADDR_MAX; addr += IAP_PAGE_SIZE) {
                if (!iap_erase_page_check(addr)) {
                    tx.pkt.status = LDR_STATUS_PROGRAM_FAILED;
                    break;
                }
            }
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
        case ISP_CMD_READ_LDR_VERSION:
            tx.pkt.status = LDR_STATUS_LDR_VERSION;
            tx.pkt.size = 8;
            *(uint32_t *)&tx.pkt.dat[0] = LDR_VERSION;
            *(uint32_t *)&tx.pkt.dat[4] = LDR_BUILD_TIME;
            break;
        default:
            tx.pkt.status = LDR_STATUS_UNKNOWN_CMD;
            break;
    }
    uart_send_tx();
}
