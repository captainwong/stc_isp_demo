#include "protocol.h"

bit isp_parse_ok = false;

// void isp_parse(isp_pkt_parse_context_t* ctx, uint8_t b) {
//     switch (ctx->state) {
//         case ISP_PARSE_STATE_IDLE:
// check_isp_pkt_head:
//             if (b == ISP_PKT_HEAD) {
//                 ctx->len = 0;
//                 ctx->sum = b;
//                 ctx->state = ISP_PARSE_STATE_LENGTH;
//             }
//             break;
//         case ISP_PARSE_STATE_LENGTH:
//             ctx->rx.buf[ctx->len++] = b;
//             ctx->state = ISP_PARSE_STATE_BODY;
//             break;
//         case ISP_PARSE_STATE_BODY:
//             ctx->rx.buf[ctx->len++] = b;
//             ctx->sum += b;
//             if (ctx->rx.pkt.len == ctx->len) {
//                 ctx->state = ISP_PARSE_STATE_END;
//             }
//             break;
//         case ISP_PARSE_STATE_END:
//             if (b == ISP_PKT_END) {
//                 ctx->sum += b;
//                 ctx->state = ISP_PARSE_STATE_CHECKSUM;
//             } else {
//                 ctx->state = ISP_PARSE_STATE_IDLE;
//                 goto check_isp_pkt_head;
//             }
//             break;
//         case ISP_PARSE_STATE_CHECKSUM:
//             if (b + ctx->sum == 0) {
//                 ctx->state = ISP_PARSE_STATE_IDLE;
//                 isp_parse_ok = true;
//             } else {
//                 ctx->state = ISP_PARSE_STATE_IDLE;
//                 goto check_isp_pkt_head;
//             }
//             break;
//     }
// }
