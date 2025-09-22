#include "uart.h"

#ifdef DEBUG
#include <stdarg.h>
#include <stdio.h>
#endif

#define RINGBUF_SIZE_TYPE uint16_t
#include <libemb/ringbuf.h>

#define RB_SIZE 256
#define UART_WAIT_TIME 0x7FFF

isp_pkt_parse_context_t ctx;
isp_packet_t xdata rx;
ldr_packet_t xdata tx;

static uint8_t xdata rx_buf[RB_SIZE];
static uint8_t xdata tx_buf[RB_SIZE];
static ringbuf_t xdata rxrb, txrb;
bit tx_busy = false;

void uart_init(void) {
    ringbuf_init(rxrb, rx_buf, sizeof(rx_buf));
    ringbuf_init(txrb, tx_buf, sizeof(tx_buf));
    tx.pkt.head = LDR_PKT_HEAD;

    uart1_use_p30_p31();
    pin_mode_io_pup(3, 0);
    pin_mode_io_pup(3, 1);
    uart1_brt_use_timer1();
    uart1_mode1_8bit_brtx();
    // uart1_disable_irq();
    uart1_enable_irq();
    uart1_enable_recv();
    // RI = 0;
    // TI = 1;

    t1_stop();
    t1_1t();
    t1_as_timer();
    t1_load(uart_calc_load_1t(MAIN_Fosc, UART1_BAUD));
    t1_disable_irq();
    t1_run();
}

void uart1_isr() INTERRUPT(UART1_VECTOR) {
    uint8_t data c;
    if (RI) {
        RI = 0;
        c = SBUF;
        if (!ringbuf_writable(rxrb)) {
            ringbuf_skip(rxrb);
        }
        ringbuf_write(rxrb, c);
    }

    if (TI) {
        TI = 0;
        if (ringbuf_readable(txrb)) {
            c = txrb.buf[txrb.r];
            if (++txrb.r == txrb.size) {
                txrb.r = 0;
            }
            SBUF = c;
        } else {
            tx_busy = false;
        }
    }
}

void uart_wait_sent(void) {
    volatile uint16_t data i = UART_WAIT_TIME;
    while (tx_busy && i--) {
        // wdt_feed();
    }
}

uint8_t uart_block_send(uint8_t dat) {
    uart1_disable_irq();
    TI = 0;
    SBUF = dat;
    while (!TI);
    uart1_enable_irq();
    return dat;
}

static void uart_send_raw(uint8_t* buf, uint8_t n) {
    volatile uint16_t data i = UART_WAIT_TIME;
    if (n > RB_SIZE) {
        return;
    }
    while (ringbuf_writable(txrb) < n && i--) {
        // wdt_feed();
    }
    if (ringbuf_writable(txrb) < n) {
        return;
    }
    ES = 0;
    if (tx_busy || ringbuf_readable(txrb)) {
        ringbuf_write_n(txrb, buf, n);
    } else {
        uint8_t data c = *buf++;
        --n;
        ringbuf_write_n(txrb, buf, n);
        tx_busy = true;
        SBUF = c;
    }
    ES = 1;
}

void uart_send_tx(void) {
    ldr_pkt_end(&tx) = LDR_PKT_END;
    ldr_pkt_sum(&tx) = ldr_pkt_calc_sum(&tx);
    uart_send_raw(tx.buf, ldr_pkt_len(&tx));
}

void uart_parse(uint8_t b) {
    switch (ctx.state) {
        case ISP_PARSE_STATE_IDLE:
        check_isp_pkt_head:
            if (b == ISP_PKT_HEAD) {
                ctx.len = 0;
                ctx.sum = b;
                rx.buf[ctx.len++] = b;
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
            if (rx.pkt.len + 2 == ctx.len) {  // 协议里len不包括head, len，但ctx->len里包括了，所以这里+2
                ctx.state = ISP_PARSE_STATE_END;
            }
            break;
        case ISP_PARSE_STATE_END:
            if (b == ISP_PKT_END) {
                ctx.sum += b;
                ctx.state = ISP_PARSE_STATE_CHECKSUM;
            } else {
#ifdef DEBUG
                uart_block_send(0xCC);
                uart_block_send(b);
                uart_block_send(ctx.len);
                uart_block_send(rx.pkt.len);
#endif
                ctx.state = ISP_PARSE_STATE_IDLE;
                goto check_isp_pkt_head;
            }
            break;
        case ISP_PARSE_STATE_CHECKSUM:
            if (b == (uint8_t)-ctx.sum) {
                ctx.state = ISP_PARSE_STATE_IDLE;
                isp_parse_ok = true;
            } else {
#ifdef DEBUG
                uart_block_send(0xDD);
                uart_block_send(b);
                uart_block_send(ctx.sum);
                uart_block_send(-ctx.sum);
#endif
                ctx.state = ISP_PARSE_STATE_IDLE;
                goto check_isp_pkt_head;
            }
            break;
        default:
            ctx.state = ISP_PARSE_STATE_IDLE;
            break;
    }
}

void uart_run(void) {
    uint8_t data c, i;
    for (i = 0; i < 4 && ringbuf_readable(rxrb); i++) {
        ringbuf_read(rxrb, c);
        uart_parse(c);
        if (isp_parse_ok) {
            return;
        }
    }
}

#ifdef DEBUG
void uart_debug(const char* fmt, ...) {
    va_list args;
    tx.pkt.status = LDR_STATUS_LOG;
    tx.pkt.size = 0;
    va_start(args, fmt);
    tx.pkt.size = vsprintf(tx.pkt.dat, fmt, args);
    va_end(args);
    uart_send_tx();
    uart_wait_sent();
}
#endif /* DEBUG */

void uart_send_check_ota(app_info_t* current) {
    tx.pkt.status = APP2OTA_CMD_GET_LATEST_APP_INFO;
    tx.pkt.size = sizeof(get_latest_app_info_req_t);
    memcpy(tx.pkt.dat, current, tx.pkt.size);
    uart_send_tx();
}
