#ifndef __SYS_H__
#define __SYS_H__

#include <libstc/stc8h.h>
#include <libstc/stc_helpers.h>
#include <string.h>

// #define MAIN_Fosc 22118400UL
// #define MAIN_Fosc 12000000UL
#define MAIN_Fosc 24000000UL

#define T1MS (65536 - MAIN_Fosc / 1000)

#define UART1_BAUD 115200UL
// #define UART1_BAUD 230400UL
#define UART2_BAUD 115200UL

#define LDR_SIZE 0x1000
#define LDR_VERSION 0x0100

#define DFU_TAG 0x12ABCD34  // DFU强制执行标志

//////////////////// GPIO ////////////////////

#define PORT_MODE_IO_PUP 0
#define PORT_MODE_OUT_PP 1
#define PORT_MODE_IN_HIZ 2
#define PORT_MODE_OUT_OD 3

#define PORT_COUNT 4
#define DEFAULT_PORT_MODE PORT_MODE_IN_HIZ

#define GPIO_TABLES_MAP(XX)                 \
    XX(2, 0, io_pup, LED)  /* 运行指示灯 */ \
    XX(3, 2, io_pup, BOOT) /* 烧录按键 */

#if !defined(__C51__) || defined(VSCODE)
#define XX(port, pin, mode, name) sbit name;
#else
#define XX(port, pin, mode, name) sbit name = P##port ^ pin;
#endif
GPIO_TABLES_MAP(XX)
#undef XX

#endif /* __SYS_H__ */
