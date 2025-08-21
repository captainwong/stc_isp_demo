#ifndef __UART_H__
#define __UART_H__

#include "sys/sys.h"

void uart_init(void);
uint8_t uart_send(uint8_t dat);

#endif /* __UART_H__ */
