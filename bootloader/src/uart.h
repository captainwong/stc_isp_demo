#ifndef __UART_H__
#define __UART_H__

#include "sys/sys.h"

void uart_init();
void uart_isr();

extern bit bUartRxReady;

extern uint8_t xdata UartTxBuffer[256];
extern uint8_t xdata UartRxBuffer[256];

void uart_send(uint8_t status, uint8_t size);
void uart_recv_done();

#endif
