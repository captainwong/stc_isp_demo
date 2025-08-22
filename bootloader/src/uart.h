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

#endif /* __UART_H__ */
