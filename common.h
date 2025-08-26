/**
 * chip: STC8H8K64U
 * ram: 256B idata, 8KB xdata
 * flash: 64KB
 *   - 4KB for bootloader
 *   - 60KB for application
 */

#ifndef __COMMON_H__
#define __COMMON_H__

// #define MAIN_Fosc 11520000UL
// #define MAIN_Fosc 12000000UL
// #define MAIN_Fosc 22118400UL
#define MAIN_Fosc 24000000UL

#define STC_RAM_SIZE 0x2000  // STC8H8K64U has 8KB xdata
#define STC_ROM_SIZE 0x10000 // STC8H8K64U has 64KB flash

#define LDR_SIZE 0x1000     // bootloader flash space = 4KB
#define LDR_VERSION 0x0100  // bootloader version 1.0
#define IAP_ADDR_MAX (STC_ROM_SIZE - LDR_SIZE)

#define DFU_TAG 0x12ABCD34UL  // force DFU mode
#define DFU_ADDR (STC_RAM_SIZE - sizeof(DFU_TAG))

#endif /* __COMMON_H__ */
