#ifndef __SYS_H__
#define __SYS_H__

#include <libstc/stc8h.h>
#include <libstc/stc_helpers.h>
#include <string.h>

#include "../../../common.h"

#define T1MS (65536 - MAIN_Fosc / 1000)

#define UART1_BAUD 115200UL
// #define UART1_BAUD 230400UL
#define UART2_BAUD 115200UL

//////////////////// GPIO ////////////////////

#define PORT_MODE_IO_PUP 0
#define PORT_MODE_OUT_PP 1
#define PORT_MODE_IN_HIZ 2
#define PORT_MODE_OUT_OD 3

#define PORT_COUNT 4
#define DEFAULT_PORT_MODE PORT_MODE_IN_HIZ

#define GPIO_TABLES_MAP(XX)                     \
    XX(2, 1, io_pup, LED_RUN)  /* 运行指示灯 */ \
    XX(3, 2, io_pup, KEY_BOOT) /* 烧录按键 */

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

#define key_boot_pressed() KEY_BOOT == 0
#define key_boot_released() KEY_BOOT == 1

#endif /* __SYS_H__ */
