#ifndef __UART_H__
#define __UART_H__

#include "sys/sys.h"
#include "protocol.h"

extern isp_pkt_parse_context_t ctx;
extern isp_packet_t xdata rx;
extern ldr_packet_t xdata tx;

void uart_init(void);
uint8_t uart_send(uint8_t dat);
void uart_send_tx(void);
void uart_parse(uint8_t b);

#ifdef DEBUG
void uart_debug(const char* fmt, ...);
#define debugf1(fmt) uart_debug(fmt)
#define debugf2(fmt, _1) uart_debug(fmt, _1)
#define debugf3(fmt, _1, _2) uart_debug(fmt, _1, _2)
#define debugf4(fmt, _1, _2, _3) uart_debug(fmt, _1, _2, _3)
#else
#define debugf1(fmt)
#define debugf2(fmt, _1)
#define debugf3(fmt, _1, _2)
#define debugf4(fmt, _1, _2, _3)
#endif /* DEBUG */

#endif /* __UART_H__ */
