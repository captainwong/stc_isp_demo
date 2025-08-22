#include "uart.h"

#ifdef DEBUG
#include <stdarg.h>
#include <stdio.h>
#endif

isp_pkt_parse_context_t ctx;
isp_packet_t xdata rx;
ldr_packet_t xdata tx;

void uart_init(void) {
    uart1_use_p30_p31();
    pin_mode_io_pup(3, 0);
    pin_mode_io_pup(3, 1);
    uart1_brt_use_timer1();
    uart1_mode1_8bit_brtx();
    uart1_disable_irq();
    uart1_enable_recv();
    RI = 0;
    TI = 1;

    t1_1t();
    t1_as_timer();
    t1_load(uart_calc_load_1t(MAIN_Fosc, UART1_BAUD));
    t1_disable_irq();
    t1_run();
}

uint8_t uart_send(uint8_t dat) {
    while (!TI);
    TI = 0;
    SBUF = dat;

    return dat;
}

void uart_send_tx(void) {
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

void uart_parse(uint8_t b) {
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
            if (rx.pkt.len + 1 == ctx.len) {  // 协议里len不包括len本身，但我的packet里包括了，所以这里+1
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
