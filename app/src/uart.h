#ifndef __UART_H__
#define __UART_H__

#include <sys/sys.h>
#include <protocol.h>

extern isp_pkt_parse_context_t ctx;
extern isp_packet_t xdata rx;
extern ldr_packet_t xdata tx;

void uart1_init(void);
void uart1_send_tx(void);
void uart1_wait_sent(void);
void uart1_run(void);
void uart1_send_check_ota(app_info_t* current);

#ifdef DEBUG
void uart1_debug(const char* fmt, ...);
#define debugf1(fmt) uart1_debug(fmt)
#define debugf2(fmt, _1) uart1_debug(fmt, _1)
#define debugf3(fmt, _1, _2) uart1_debug(fmt, _1, _2)
#define debugf4(fmt, _1, _2, _3) uart1_debug(fmt, _1, _2, _3)
#define debugf5(fmt, _1, _2, _3, _4) uart1_debug(fmt, _1, _2, _3, _4)
#define debugf6(fmt, _1, _2, _3, _4, _5) uart1_debug(fmt, _1, _2, _3, _4, _5)
#define debugf7(fmt, _1, _2, _3, _4, _5, _6) uart1_debug(fmt, _1, _2, _3, _4, _5, _6)
#else
#define debugf1(fmt)
#define debugf2(fmt, _1)
#define debugf3(fmt, _1, _2)
#define debugf4(fmt, _1, _2, _3)
#define debugf5(fmt, _1, _2, _3, _4)
#define debugf6(fmt, _1, _2, _3, _4, _5)
#define debugf7(fmt, _1, _2, _3, _4, _5, _6)
#endif /* DEBUG */

#endif /* __UART_H__ */
