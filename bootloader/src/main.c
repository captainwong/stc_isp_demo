#include "protocol.h"

uint32_t xdata dfutag __at(DFU_ADDR);
isp_pkt_parse_context_t xdata ctx;
ldr_packet_t xdata tx;

void uart_init(void);
void isp_handle(void);

void main() {
    if ((dfutag != DFU_TAG) &&
        (*(uint8_t code *)(LDR_SIZE) == 0x02) &&               // check if first op code is `LJMP addr16`
        (*(uint16_t code *)(LDR_SIZE + 1) >= LDR_SIZE + 3)) {  // check if `addr16 >= LDR_SIZE + 3`
        ((void(code *)())(LDR_SIZE))();                        // LJMP #LDR_SIZE, from here the CPU is running application code
    }

    // now CPU is running bootloader code

    dfutag = 0;     // clear force DFU mode flag
    enable_xsfr();  // for xsfr `IAP_TPS`
    iap_tps(iap_calc_tps(MAIN_Fosc));
    isp_parse_init(ctx);
    uart_init();

    while (1) {
        uint8_t dat;

        if (RI) {
            RI = 0;
            dat = SBUF;
            if (isp_parse(&ctx, dat)) {
                isp_handle();
            }
        }
    }
}

void uart_init(void) {
    t1_1t();
    t1_as_timer();
    t1_load(uart_calc_load_1t(MAIN_Fosc, UART1_BAUD));
    t1_disable_irq();
    t1_run();

    uart1_use_p30_p31();
    pin_mode_io_pup(3, 0);
    pin_mode_io_pup(3, 1);
    uart1_mode1_8bit_brtx();
    uart1_brt_use_timer1();
    uart1_enable_recv();
    TI = 1;
}

static uint8_t send(uint8_t dat) {
    while (!TI);
    TI = 0;
    SBUF = dat;

    return dat;
}

void send_tx(void) {
    uint8_t sum = send(LDR_PKT_HEAD), i;
    sum += send(tx.pkt.status);
    sum += send(tx.pkt.size);
    for (i = 0; i < tx.pkt.size; i++) {
        sum += send(tx.pkt.dat[i]);
    }
    sum += send(LDR_PKT_END);
    send(-sum);
}

void isp_handle(void) {
    uint16_t addr = ctx.rx.pkt.addr;
    tx.pkt.status = LDR_STATUS_OK;
    tx.pkt.size = 0;
    switch (ctx.rx.pkt.cmd) {
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
