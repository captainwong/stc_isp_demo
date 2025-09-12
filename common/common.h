#ifndef __COMMON_H__
#define __COMMON_H__

#include <libemb/emb_bitrev.h>
#include <libemb/emb_config.h>

//////////////////////////// change this by specific chip ////////////////////////////

/**
 * board: 开天斧三.1 2022/8/27 开发板
 * chip: STC8H8K64U
 * ram: 256B idata, 8KB xdata
 * rom: 64KB
 *   - 512B page erase
 *   - 512B page program
 *   - partition: aiapp-isp: set user eeprom size to 64KB
 *      - 0x0000 - 0x1FFF : 8KB for bootloader
 *      - 0x2000 - 0xFDFF : 55.5KB for application
 *      - 0xFE00 - 0xFFFF : 512B for factory metadata
 * norflash: W25Q32JVSIQ (32Mbit, 4MB)
 *   - 4KB sector erase
 *   - 256B page program
 *   - partition:
 *      - 0x000000 - 0x000FFF : 4KB for ota info
 *      - 0x001000 - 0x00FFFF : 60KB for factory application
 *      - 0x010000 - 0x10FFFF : 60KB for ota application 1
 *      - 0x110000 - 0x20FFFF : 60KB for ota application 2
 *      - 0x210000 - 0x3FFFFF : application use
 */

#define STC_RAM_SIZE 0x2000   // STC8H8K64U has 8KB xdata
#define STC_ROM_SIZE 0x10000  // STC8H8K64U has 64KB flash

//////////////////////////// change this by you want ////////////////////////////

// #define MAIN_Fosc 11520000UL
// #define MAIN_Fosc 12000000UL
// #define MAIN_Fosc 22118400UL
#define MAIN_Fosc 24000000UL
#define T1MS (65536 - MAIN_Fosc / 1000)  // 1T

typedef union {
    uint8_t b;
    struct {
        uint8_t dfu : 1;                // whether to enter DFU mode
        uint8_t ldr : 1;                // whether running in bootloader mode
        uint8_t onchip_meta_valid : 1;  // whether on-chip factory metadata is valid
        uint8_t resv : 5;
    } st;
} system_context_t;

#define SYSTEM_CONTEXT_ADDR (STC_RAM_SIZE - sizeof(system_context_t))  // at end of RAM space

//////////////////////////// shared pin configuration ////////////////////////////

// 开天斧板子P1.3被NTC测温电路占用了

#define PORT_COUNT 4
#define DEFAULT_PORT_MODE PORT_MODE_IN_HIZ

#define SHARED_GPIO_TABLES_MAP(XX)              \
    XX(2, 0, io_pup, LED_RUN)  /* 运行指示灯 */ \
    XX(2, 2, io_pup, SPI_NSS)  /* SPI NSS */    \
    XX(2, 3, io_pup, SPI_MOSI) /* SPI MOSI */   \
    XX(2, 4, io_pup, SPI_MISO) /* SPI MISO */   \
    XX(2, 5, io_pup, SPI_SCK)  /* SPI SCK */    \
    XX(3, 0, io_pup, RXD)                       \
    XX(3, 1, io_pup, TXD)

//////////////////////////// on-chip flash partition ////////////////////////////

#define LDR_SIZE 0x2000          // bootloader flash space = 8KB, at the beginning of on-chip flash
#define FACTORY_META_SIZE 0x200  // factory metadata space = 512B, at the end of on-chip flash
#define APP_MAX_SIZE (STC_ROM_SIZE - LDR_SIZE - FACTORY_META_SIZE)
#define IAP_ADDR_APP_START LDR_SIZE  // application start address for IAP functions
#define IAP_ADDR_APP_END (APP_MAX_SIZE + IAP_ADDR_APP_START)
#define IAP_ADDR_FACTORY_META (STC_ROM_SIZE - FACTORY_META_SIZE)

//////////////////////////// app info ////////////////////////////

#if !defined(__C51__) || defined(VSCODE)
#pragma pack(1)
#endif

// application info structure
// if retrieved from ota server, little endian
// if stored in norflash, big endian
typedef struct {
    uint32_t size;       // size of the whole application binary
    uint32_t crc;        // crc32 of the whole application binary
    uint32_t timestamp;  // UTC timestamp
    uint32_t version;    // major(8).minor(8).patch(16)
} app_info_t;

#define app_info_to_little_endian(info)             \
    do {                                            \
        (info).size = rev32((info).size);           \
        (info).crc = rev32((info).crc);             \
        (info).timestamp = rev32((info).timestamp); \
        (info).version = rev32((info).version);     \
    } while (0)

#define app_info_to_big_endian(info) app_info_to_little_endian(info)

// application info structure, stored in norflash, big endian
typedef struct {
    uint32_t addr;
    app_info_t info;
} flash_app_info_t;

// ota info structure, stored in norflash, big endian
typedef struct {
    flash_app_info_t factory;  // factory application info
    flash_app_info_t ota1;     // ota application 1 info
    flash_app_info_t ota2;     // ota application 2 info
} ota_info_t;

#if !defined(__C51__) || defined(VSCODE)
#pragma pack()
#endif

//////////////////////////// common functions ////////////////////////////

#define is_valid_on_chip_app_program()                                                                 \
    ((*(uint8_t code *)(LDR_SIZE) == 0x02) &&            /* check if first op code is `LJMP addr16` */ \
     (*(uint16_t code *)(LDR_SIZE + 1) >= LDR_SIZE + 3)) /* check if `addr16 >= LDR_SIZE + 3` */

#define jump_to_on_chip_app_program(offset) ((void(code *)())(LDR_SIZE + (offset)))()

void delay_ms(uint16_t ms);

#endif /* __COMMON_H__ */
