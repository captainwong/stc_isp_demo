#include "protocol.h"

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
