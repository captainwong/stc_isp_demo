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
 *      - 0x0000 - 0x37FF : 14KB for bootloader
 *      - 0x3800 - 0x39FF : 512B for factory metadata
 *      - 0x3A00 - 0xFFFF : 49.5KB for application
 * norflash: W25Q32JVSIQ (32Mbit, 4MB)
 *   - 4KB sector erase
 *   - 256B page program
 *   - partition:
 *      - 0x000000 - 0x000FFF : 4KB for ota info
 *      - 0x001000 - 0x001FFF : 4KB for ota info backup
 *      - 0x002000 - 0x010FFF : 60KB for factory application
 *      - 0x011000 - 0x01FFFF : 60KB for ota application 1
 *      - 0x020000 - 0x02EFFF : 60KB for ota application 2
 *      - 0x02F000 - 0x3FFFFF : 3908KB for application use
 */

#define STC_RAM_SIZE 0x2000   // STC8H8K64U has 8KB xdata
#define STC_ROM_SIZE 0x10000  // STC8H8K64U has 64KB flash

#define FLASH_OTA_ID_MASTER 0  // master ota info
#define FLASH_OTA_ID_BACKUP 1  // backup ota info

#define FLASH_APP_ID_FACTORY 0
#define FLASH_APP_ID_APP1 1
#define FLASH_APP_ID_APP2 2
#define FLASH_APP_ID_MAX 2

//////////////////////////// change this by you want ////////////////////////////

// #define MAIN_Fosc 11520000UL
// #define MAIN_Fosc 12000000UL
// #define MAIN_Fosc 22118400UL
#define MAIN_Fosc 24000000UL
#define T1MS (65536 - MAIN_Fosc / 1000)  // 1T

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

//////////////////////////// system context in XDATA ////////////////////////////

typedef union {
    uint8_t b;
    struct {
        uint8_t dfu : 1;                // whether to enter DFU mode
        uint8_t ldr : 1;                // whether running in bootloader mode
        uint8_t onchip_app_valid : 1;   // whether on-chip application is valid
        uint8_t onchip_meta_valid : 1;  // whether on-chip factory metadata is valid
        uint8_t appid : 2;              // current running application id, 0: factory, 1: app1, 2: app2
        uint8_t otaid : 1;              // current old ota info id (which is ready to overwrite), 0: master, 1: backup
        uint8_t resv : 1;               // reserved
    } st;
} system_context_t;

#define SYSTEM_CONTEXT_ADDR (STC_RAM_SIZE - sizeof(system_context_t))  // at end of RAM space

//////////////////////////// on-chip flash partition ////////////////////////////

#define LDR_SIZE 0x3800                                             // bootloader flash space = 13.5KB, at the beginning of on-chip flash
#define FACTORY_META_SIZE 0x200                                     // factory metadata space = 512B, after bootloader
#define APP_MAX_SIZE (STC_ROM_SIZE - LDR_SIZE - FACTORY_META_SIZE)  // max application size = 50KB, after metadata
#define IAP_ADDR_FACTORY_META LDR_SIZE                              // factory metadata address for IAP functions
#define IAP_ADDR_APP_START (LDR_SIZE + FACTORY_META_SIZE)           // application start address for IAP functions
#define IAP_ADDR_APP_END (IAP_ADDR_APP_START + APP_MAX_SIZE)        // application end address for IAP functions

//////////////////////////// off-chip flash partition ////////////////////////////

#define NORFLASH_OTA_INFO_SIZE 0x1000UL                                                // ota info size in norflash = 4KB
#define NORFLASH_OTA_MASTER_ADDR 0x0000UL                                              // ota master address in norflash
#define NORFLASH_OTA_BACKUP_ADDR (NORFLASH_OTA_MASTER_ADDR + NORFLASH_OTA_INFO_SIZE)   // ota backup address in norflash
#define NORFLASH_APP_SIZE 0xF0000UL                                                    // application size in norflash = 60KB
#define NORFLASH_FACTORY_APP_ADDR (NORFLASH_OTA_BACKUP_ADDR + NORFLASH_OTA_INFO_SIZE)  // factory application address in norflash
#define NORFLASH_APP1_ADDR (NORFLASH_FACTORY_APP_ADDR + NORFLASH_APP_SIZE)             // ota application 1 address in norflash
#define NORFLASH_APP2_ADDR (NORFLASH_APP1_ADDR + NORFLASH_APP_SIZE)                    // ota application 2 address in norflash

#if NORFLASH_APP_SIZE < APP_MAX_SIZE
#error "NORFLASH_APP_SIZE must be >= APP_MAX_SIZE"
#endif

#define get_norflash_app_addr(appid)                             \
    ((appid) == FLASH_APP_ID_FACTORY ? NORFLASH_FACTORY_APP_ADDR \
     : (appid) == FLASH_APP_ID_APP1  ? NORFLASH_APP1_ADDR        \
                                     : NORFLASH_APP2_ADDR)

//////////////////////////// app info ////////////////////////////

#if !defined(__C51__) || defined(VSCODE)
#pragma pack(1)
#endif

// application info structure
// if retrieved from ota server, little endian
// if stored in norflash, big endian
// 16 bytes
typedef struct {
    uint32_t size;       // size of the whole application binary
    uint32_t crc;        // crc32 of the whole application binary
    uint32_t timestamp;  // UTC timestamp
    uint32_t version;    // major(8).minor(8).patch(16)
} app_info_t;

#define init_app_info(info) (info).size = (info).crc = (info).timestamp = (info).version = 0

#define app_info_to_little_endian(info)             \
    do {                                            \
        (info).size = rev32((info).size);           \
        (info).crc = rev32((info).crc);             \
        (info).timestamp = rev32((info).timestamp); \
        (info).version = rev32((info).version);     \
    } while (0)

#define app_info_to_big_endian(info) app_info_to_little_endian(info)

typedef enum {
    FLASH_APP_DL_STATE_INVALID = 0xFF,
    FLASH_APP_DL_STATE_IDLE = 0,
    FLASH_APP_DL_STATE_ERASING,
    FLASH_APP_DL_STATE_DOWNLOADING,
    FLASH_APP_DL_STATE_VERIFYING,
    FLASH_APP_DL_STATE_APPLYING,
} flash_app_dl_state_t;

// current downloading application context, stored in norflash, big endian
// 12 bytes
typedef struct {
    uint8_t state;      // flash_app_dl_state_t
    uint8_t appid;      // application id being downloaded, 1: app1, 2: app2, will never be 0(factory)
    uint8_t resv[2];    // reserved
    uint32_t received;  // bytes received so far
    uint32_t crc;       // crc32 of bytes received so far
} flash_app_download_ctx_t;

#define init_flash_app_download_ctx(ctx)       \
    do {                                       \
        (ctx).state = FLASH_APP_DL_STATE_IDLE; \
        (ctx).appid = 0; /* 0 for idle */      \
        (ctx).received = 0;                    \
        (ctx).crc = 0;                         \
    } while (0)

#define flash_app_download_ctx_to_little_endian(ctx) \
    do {                                             \
        (ctx).received = rev32((ctx).received);      \
        (ctx).crc = rev32((ctx).crc);                \
    } while (0)

#define flash_app_download_ctx_to_big_endian(ctx) flash_app_download_ctx_to_little_endian(ctx)

// ota info structure, stored in norflash, big endian
// 68 bytes
typedef struct {
    uint32_t seq;                    /* sequence number, incremented by 1 each time ota info is updated
                                      * which seq is larger, which ota info is newer
                                      * which seq is smaller, which ota info is safe to write
                                      * i don't believe seq will overflow.
                                      */
    uint8_t current_app;             // current running application id, 0: factory, 1: app1, 2: app2
    uint8_t resv[3];                 // reserved
    flash_app_download_ctx_t dlctx;  // downloading application context, for app use, bootloader will ignore it
    app_info_t factory;              // factory application info
    app_info_t app1;                 // ota application 1 info
    app_info_t app2;                 // ota application 2 info
} ota_info_t;

#define ota_info_to_little_endian(info)                        \
    do {                                                       \
        (info).seq = rev32((info).seq);                        \
        flash_app_download_ctx_to_little_endian((info).dlctx); \
        app_info_to_little_endian((info).factory);             \
        app_info_to_little_endian((info).app1);                \
        app_info_to_little_endian((info).app2);                \
    } while (0)

#define ota_info_to_big_endian(info) ota_info_to_little_endian(info)

extern ota_info_t EMB_XDATA_MODIFIER ota_info;

#if !defined(__C51__) || defined(VSCODE)
#pragma pack()
#endif

//////////////////////////// common functions ////////////////////////////

#define is_valid_on_chip_app_program()                                                                                     \
    ((*(uint8_t code *)(IAP_ADDR_APP_START) == 0x02) &&                      /* check if first op code is `LJMP addr16` */ \
     (*(uint16_t code *)(IAP_ADDR_APP_START + 1) >= IAP_ADDR_APP_START + 3)) /* check if `addr16 >= IAP_ADDR_APP_START + 3` */

#define jump_to_on_chip_app_program(offset) ((void(code *)())(IAP_ADDR_APP_START + (offset)))()

#define version_major(v) (uint8_t)(((v) >> 24) & 0xFF)
#define version_minor(v) (uint8_t)(((v) >> 16) & 0xFF)
#define version_patch(v) (uint16_t)((v) & 0xFFFF)

void delay_ms(uint16_t ms);

// read ota master/backup to find out which ota info is older(smaller seq)
// then set sysctx.st.otaid accordingly
void findout_which_ota_info_is_older(void);

#endif /* __COMMON_H__ */
