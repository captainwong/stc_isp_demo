/**
 * chip: STC8H8K64U
 * ram: 256B idata, 8KB xdata
 * flash: 64KB
 *   - 4KB for bootloader
 *   - 60KB for application
 */

#ifndef __SYS_H__
#define __SYS_H__

#include <libstc/stc8h.h>
#include <libstc/stc_helpers.h>
#include <string.h>

#define STC_RAM_SIZE 0x2000  // STC8H8K64U has 8KB xdata

// #define MAIN_Fosc 22118400UL
// #define MAIN_Fosc 12000000UL
#define MAIN_Fosc 24000000UL

#define UART1_BAUD 115200UL
// #define UART1_BAUD 230400UL
#define UART2_BAUD 115200UL

#define LDR_SIZE 0x1000  // bootloader flash space
#define LDR_VERSION 0x0100

#define DFU_TAG 0x12ABCD34UL  // force DFU mode
#define DFU_ADDR (STC_RAM_SIZE - sizeof(DFU_TAG))

#endif /* __SYS_H__ */
