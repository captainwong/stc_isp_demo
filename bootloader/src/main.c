#include "protocol.h"
#include "sys/gpio.h"
#include "uart.h"

uint32_t xdata dfutag __at(DFU_ADDR);
isp_pkt_parse_context_t ctx;
isp_packet_t xdata rx;
ldr_packet_t xdata tx;
uint16_t cycles;

void isp_parse(uint8_t b);
void isp_handle(void);

void delay() {
    volatile uint16_t data i = 0;
    while (++i) {
        nop1();
    }
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
    isp_parse_init(ctx);
    gpio_init();
    pin_pu(3, 5);
    uart_init();
    cycles = 0;

    uart_send(0xAA);
    uart_send(0xBB);
    uart_send(0xCC);

    while (1) {
        debug_led();
        if (RI) {
            uint8_t dat;
            RI = 0;
            dat = SBUF;
            isp_parse(dat);
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

void send_tx(void) {
    uint8_t sum = uart_send(LDR_PKT_HEAD), i;
    sum += uart_send(tx.pkt.status);
    sum += uart_send(tx.pkt.size);
    for (i = 0; i < tx.pkt.size; i++) {
        sum += uart_send(tx.pkt.dat[i]);
    }
    sum += uart_send(LDR_PKT_END);
    uart_send(-sum);
    while (!TI);
}

void isp_parse(uint8_t b) {
    switch (ctx.state) {
        case ISP_PARSE_STATE_IDLE:
        check_isp_pkt_head:
            if (b == ISP_PKT_HEAD) {
                ctx.len = 0;
                ctx.sum = b;
                ctx.state = ISP_PARSE_STATE_LENGTH;
            }
            break;
        case ISP_PARSE_STATE_LENGTH:
            rx.buf[ctx.len++] = b;
            ctx.sum += b;
            ctx.state = ISP_PARSE_STATE_BODY;
            break;
        case ISP_PARSE_STATE_BODY:
            rx.buf[ctx.len++] = b;
            ctx.sum += b;
            if (rx.pkt.len + 1 == ctx.len) { // 协议里len不包括len本身，但我的packet里包括了，所以这里+1
                ctx.state = ISP_PARSE_STATE_END;
            }
            break;
        case ISP_PARSE_STATE_END:
            if (b == ISP_PKT_END) {
                ctx.sum += b;
                ctx.state = ISP_PARSE_STATE_CHECKSUM;
            } else {
                uart_send(0xCC);
                uart_send(b);
                uart_send(ctx.len);
                uart_send(rx.pkt.len);
                ctx.state = ISP_PARSE_STATE_IDLE;
                goto check_isp_pkt_head;
            }
            break;
        case ISP_PARSE_STATE_CHECKSUM:
            if (b == -ctx.sum) {
                ctx.state = ISP_PARSE_STATE_IDLE;
                isp_parse_ok = true;
            } else {
                uart_send(0xDD);
                uart_send(b);
                uart_send(ctx.sum);
                uart_send(-ctx.sum);
                ctx.state = ISP_PARSE_STATE_IDLE;
                goto check_isp_pkt_head;
            }
            break;
        default:
            ctx.state = ISP_PARSE_STATE_IDLE;
            break;
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
    send_tx();
}
