/**
 * chip: STC8H8K64U
 * ram: 256B idata, 8KB xdata
 * flash: 64KB
 *   - 4KB for bootloader
 *   - 60KB for application
 */

#ifndef __COMMON_H__
#define __COMMON_H__

//////////////////////////// change this by specific chip ////////////////////////////

#define STC_RAM_SIZE 0x2000   // STC8H8K64U has 8KB xdata
#define STC_ROM_SIZE 0x10000  // STC8H8K64U has 64KB flash

//////////////////////////// change this by user ////////////////////////////

// #define MAIN_Fosc 11520000UL
// #define MAIN_Fosc 12000000UL
// #define MAIN_Fosc 22118400UL
#define MAIN_Fosc 24000000UL

#define DFU_TAG 0x12ABCD34UL  // force DFU mode
#define DFU_ADDR (STC_RAM_SIZE - sizeof(DFU_TAG))

#define LDR_VERSION 0x0100  // bootloader version 1.0

//////////////////////////// FLASH partition ////////////////////////////

#define LDR_SIZE 0x1000                     // bootloader flash space = 4KB
#define APP_ADDR LDR_SIZE                   // application address
#define APP_SIZE (STC_ROM_SIZE - LDR_SIZE)  // application size

#define IAP_ADDR_MAX (STC_ROM_SIZE - LDR_SIZE)  // when iap erasing page(by bootloader or application), the `addr` must less than this value

#endif /* __COMMON_H__ */
