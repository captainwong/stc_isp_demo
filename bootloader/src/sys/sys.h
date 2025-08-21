#ifndef __SYS_H__
#define __SYS_H__

#include <libstc/stc8h.h>
#include <libstc/stc_helpers.h>
#include <string.h>

#include "../../../common.h"

#define UART1_BAUD 9600
// #define UART1_BAUD 115200UL
// #define UART1_BAUD 230400UL
#define UART2_BAUD 115200UL

//////////////////// GPIO ////////////////////

#define PORT_MODE_IO_PUP 0
#define PORT_MODE_OUT_PP 1
#define PORT_MODE_IN_HIZ 2
#define PORT_MODE_OUT_OD 3

#define PORT_COUNT 4
#define DEFAULT_PORT_MODE PORT_MODE_IN_HIZ

#define GPIO_TABLES_MAP(XX)                       \
    XX(2, 0, io_pup, LED_RUN)    /* 运行指示灯 */ \
    XX(2, 1, io_pup, LED_DEBUG1) /* 调试指示灯 */ \
    XX(2, 2, io_pup, LED_DEBUG2) /* 调试指示灯 */ \
    XX(2, 3, io_pup, LED_DEBUG3) /* 调试指示灯 */ \
    XX(2, 4, io_pup, LED_DEBUG4) /* 调试指示灯 */ \
    XX(2, 5, io_pup, LED_DEBUG5) /* 调试指示灯 */ \
    XX(3, 0, io_pup, RXD)                         \
    XX(3, 1, io_pup, TXD)                         \
    XX(3, 5, io_pup, KEY_REBOOT) /* 重启按键 */

#if !defined(__C51__) || defined(VSCODE)
#define XX(port, pin, mode, name) sbit name;
#else
#define XX(port, pin, mode, name) sbit name = P##port ^ pin;
#endif
GPIO_TABLES_MAP(XX)
#undef XX

#define led_run_on() LED_RUN = 0
#define led_run_off() LED_RUN = 1
#define led_run_toggle() LED_RUN = !LED_RUN

#define key_reboot_pressed() KEY_REBOOT == 0
#define key_reboot_released() KEY_REBOOT == 1

void debug_led(void);

#endif /* __SYS_H__ */
