#ifndef __COMMON_H__
#define __COMMON_H__

//////////////////////////// change this by specific chip ////////////////////////////

/**
 * chip: STC8H8K64U
 * ram: 256B idata, 8KB xdata
 * flash: 64KB
 *   - 4KB for bootloader
 *   - 60KB for application
 */

#define STC_RAM_SIZE 0x2000   // STC8H8K64U has 8KB xdata
#define STC_ROM_SIZE 0x10000  // STC8H8K64U has 64KB flash

//////////////////////////// change this by you want ////////////////////////////

// #define MAIN_Fosc 11520000UL
// #define MAIN_Fosc 12000000UL
// #define MAIN_Fosc 22118400UL
#define MAIN_Fosc 24000000UL

#define DFU_TAG 0x12ABCD34UL                       // force DFU mode
#define DFU_ADDR (STC_RAM_SIZE - sizeof(DFU_TAG))  // at the last 4 bytes of RAM space

#define LDR_VERSION 0x0100  // bootloader version 1.0

//////////////////////////// app info ////////////////////////////

typedef struct {
    uint16_t addr;
    uint16_t size;
    uint32_t crc;
} app_info_t;

//////////////////////////// FLASH partition ////////////////////////////

// 0x0000 - 0x1000 : bootloader
// 0x1000 - 0x7000 : application
// 0x7000 - 0xD000 : backup application
// 0xD000 - 0x10000 : application eeprom

#define LDR_SIZE 0x1000  // bootloader flash space = 4KB
#define APP_SIZE 0x6000  // application flash size = 24KB

#define IAP_ADDR_MAX (STC_ROM_SIZE - LDR_SIZE)  // when iap erasing page(by bootloader or application), the `addr` must less than this value


#endif /* __COMMON_H__ */
