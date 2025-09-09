#ifndef __SYS_H__
#define __SYS_H__

#include <libstc/stc8h.h>
#include <libstc/stc_helpers.h>
#include <string.h>

#include "../../../common/common.h"

#define UART1_BAUD 115200UL
// #define UART1_BAUD 230400UL
#define UART2_BAUD 115200UL

//////////////////// GPIO ////////////////////

#define PORT_COUNT 4
#define DEFAULT_PORT_MODE PORT_MODE_IN_HIZ

#define GPIO_TABLES_MAP(XX)                     \
    XX(3, 2, io_pup, KEY_BOOT) /* 烧录按键 */

#if !defined(__C51__) || defined(VSCODE)
#define XX(port, pin, mode, name) sbit name;
#else
#define XX(port, pin, mode, name) sbit name = P##port ^ pin;
#endif
SHARED_GPIO_TABLES_MAP(XX)
GPIO_TABLES_MAP(XX)
#undef XX

#define led_run_on() LED_RUN = 0
#define led_run_off() LED_RUN = 1
#define led_run_toggle() LED_RUN = !LED_RUN

#define key_boot_pressed() KEY_BOOT == 0
#define key_boot_released() KEY_BOOT == 1

#endif /* __SYS_H__ */
