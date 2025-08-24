#include "protocol.h"

/////////////////////////////// isp packet ///////////////////////////////

bit isp_parse_ok = false;

#ifdef _MSC_VER
void isp_parse(isp_pkt_parse_context_t* ctx, isp_packet_t* rx, uint8_t b) {
    isp_parse_ok = false;
    switch (ctx->state) {
        case ISP_PARSE_STATE_IDLE:
        check_isp_pkt_head:
            if (b == ISP_PKT_HEAD) {
                ctx->len = 0;
                rx->buf[ctx->len++] = b;
                ctx->sum = b;
                ctx->state = ISP_PARSE_STATE_LENGTH;
            }
            break;
        case ISP_PARSE_STATE_LENGTH:
            rx->buf[ctx->len++] = b;
            ctx->sum += b;
            ctx->state = ISP_PARSE_STATE_BODY;
            break;
        case ISP_PARSE_STATE_BODY:
            rx->buf[ctx->len++] = b;
            ctx->sum += b;
            if (rx->pkt.len + 2 == ctx->len) {  // 协议里len不包括head, len，但ctx->len里包括了，所以这里+2
                ctx->state = ISP_PARSE_STATE_END;
            }
            break;
        case ISP_PARSE_STATE_END:
            if (b == ISP_PKT_END) {
                ctx->sum += b;
                rx->buf[ctx->len++] = b;
                ctx->state = ISP_PARSE_STATE_CHECKSUM;
            } else {
                ctx->state = ISP_PARSE_STATE_IDLE;
                goto check_isp_pkt_head;
            }
            break;
        case ISP_PARSE_STATE_CHECKSUM:
            if (b == (uint8_t)-ctx->sum) {
                rx->buf[ctx->len++] = b;
                ctx->state = ISP_PARSE_STATE_IDLE;
                isp_parse_ok = true;
            } else {
                ctx->state = ISP_PARSE_STATE_IDLE;
                goto check_isp_pkt_head;
            }
            break;
        default:
            ctx->state = ISP_PARSE_STATE_IDLE;
            goto check_isp_pkt_head;
            break;
    }
}
#endif

uint8_t isp_pkt_calc_sum(isp_packet_t* pkt) {
    uint8_t sum = 0, i;
    for (i = 0; i < isp_pkt_len(pkt) - 1; i++) {
        sum += pkt->buf[i];
    }
    return -sum;
}

/////////////////////////////// ldr packet ///////////////////////////////

bit ldr_parse_ok = false;

#ifdef _MSC_VER  // for programmer
void ldr_parse(ldr_pkt_parse_context_t* ctx, ldr_packet_t* rx, uint8_t b) {
    ldr_parse_ok = false;
    switch (ctx->state) {
        case LDR_PARSE_STATE_IDLE:
        check_ldr_pkt_head:
            if (b == LDR_PKT_HEAD) {
                ctx->len = 0;
                rx->buf[ctx->len++] = b;
                ctx->sum = b;
                ctx->state = LDR_PARSE_STATE_STATUS;
            }
            break;
        case LDR_PARSE_STATE_STATUS:
            rx->buf[ctx->len++] = b;
            ctx->sum += b;
            ctx->state = LDR_PARSE_STATE_SIZE;
            break;
        case LDR_PARSE_STATE_SIZE:
            rx->buf[ctx->len++] = b;
            ctx->sum += b;
            ctx->state = b == 0 ? LDR_PARSE_STATE_END : LDR_PARSE_STATE_DATA;
            break;
        case LDR_PARSE_STATE_DATA:
            rx->buf[ctx->len++] = b;
            ctx->sum += b;
            if (rx->pkt.size + 3 == ctx->len) {  // 协议里len不包括head, status, size，但ctx->len里包括了，所以这里+3
                ctx->state = LDR_PARSE_STATE_END;
            }
            break;
        case LDR_PARSE_STATE_END:
            if (b == LDR_PKT_END) {
                ctx->sum += b;
                rx->buf[ctx->len++] = b;
                ctx->state = LDR_PARSE_STATE_CHECKSUM;
            } else {
                ctx->state = LDR_PARSE_STATE_IDLE;
                goto check_ldr_pkt_head;
            }
            break;
        case LDR_PARSE_STATE_CHECKSUM:
            if (b == (uint8_t)-ctx->sum) {
                rx->buf[ctx->len++] = b;
                ctx->state = LDR_PARSE_STATE_IDLE;
                ldr_parse_ok = true;
            } else {
                ctx->state = LDR_PARSE_STATE_IDLE;
                goto check_ldr_pkt_head;
            }
            break;
        default:
            ctx->state = LDR_PARSE_STATE_IDLE;
            goto check_ldr_pkt_head;
            break;
    }
}
#endif

uint8_t ldr_pkt_calc_sum(ldr_packet_t* pkt) {
    uint8_t sum = 0, i;
    for (i = 0; i < ldr_pkt_len(pkt) - 1; i++) {
        sum += pkt->buf[i];
    }
    return -sum;
}
