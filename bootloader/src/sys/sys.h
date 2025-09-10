#ifndef __SYS_H__
#define __SYS_H__

#include <libstc/stc8h.h>
#include <libstc/stc_helpers.h>
#include <string.h>

#include "../../../common/common.h"

// #define UART1_BAUD 9600
#define UART1_BAUD 115200UL
// #define UART1_BAUD 230400UL
#define UART2_BAUD 115200UL

//////////////////// GPIO ////////////////////

#define GPIO_TABLES_MAP(XX)    \
    SHARED_GPIO_TABLES_MAP(XX) \
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

extern uint32_t xdata dfutag;
extern uint32_t xdata modetag;

#endif /* __SYS_H__ */
