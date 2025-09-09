#ifndef __NORFLASH_H__
#define __NORFLASH_H__

#include <bsp/spi.h>

#define NORFLASH_TYPES_MAP(XX) \
    XX(0xEF13, W25Q80)         \
    XX(0xEF14, W25Q16)         \
    XX(0xEF15, W25Q32)         \
    XX(0xEF16, W25Q64)         \
    XX(0xEF17, W25Q128)        \
    XX(0xEF18, W25Q256)

/* FLASH芯片列表 */
typedef enum norflash_type_t {
#define XX(id, name) NFT_##name = id,
    NORFLASH_TYPES_MAP(XX)
#undef XX
} norflash_type_t;

#define NORFLASH_COMMANDS_MAP(XX)    \
    XX(0x06, WRITE_ENABLE)           \
    XX(0x04, WRITE_DISABLE)          \
    XX(0x05, READ_STATUS_REG1)       \
    XX(0x35, READ_STATUS_REG2)       \
    XX(0x15, READ_STATUS_REG3)       \
    XX(0x01, WRITE_STATUS_REG1)      \
    XX(0x31, WRITE_STATUS_REG2)      \
    XX(0x11, WRITE_STATUS_REG3)      \
    XX(0x03, READ_DATA)              \
    XX(0x0B, FAST_READ_DATA)         \
    XX(0x3B, FAST_READ_DUAL)         \
    XX(0xEB, FAST_READ_QUAD)         \
    XX(0x02, PAGE_PROGRAM)           \
    XX(0x32, PAGE_PROGRAM_QUAD)      \
    XX(0xD8, BLOCK_ERASE)            \
    XX(0x20, SECTOR_ERASE)           \
    XX(0xC7, CHIP_ERASE)             \
    XX(0xB9, POWER_DOWN)             \
    XX(0xAB, RELEASE_POWER_DOWN)     \
    XX(0xAB, DEVICE_ID)              \
    XX(0x90, MANUFACTURER_DEVICE_ID) \
    XX(0x9F, JEDEC_DEVICE_ID)        \
    XX(0xB7, ENABLE_4BYTE_ADDR)      \
    XX(0xE9, EXIT_4BYTE_ADDR)        \
    XX(0xC0, SET_READ_PARAM)         \
    XX(0x38, ENTER_QPI_MODE)         \
    XX(0xFF, DUMMY)

/* 指令表 */
typedef enum norflash_command_t {
#define XX(cmd, name) NFCMD_##name = cmd,
    NORFLASH_COMMANDS_MAP(XX)
#undef XX
} norflash_command_t;

// see `norflash_type_t`
extern uint16_t norflash_type;

bool norflash_init(void);
void norflash_powerup(void);
void norflash_powerdown(void);
// The device ID is read from the flash chip and stored in `norflash_type` if the read is successful.
bool norflash_read_id(void);
const char *norflash_get_type_string(void);
void norflash_enable_write(void);
/**
 * @brief Read the status register of the flash chip.
 *
 * @param reg The status register to read, 1, 2, or 3.
 * @return uint8_t The value of the status register.
 */
uint8_t norflash_read_sr(uint8_t reg);
/**
 * @brief Write to the status register of the flash chip.
 *
 * @param reg The status register to write to, 1, 2, or 3.
 * @param value The value to write to the status register.
 */
void norflash_write_sr(uint8_t reg, uint8_t value);

void norflash_erase_chip(void);
// The address is the starting address of the sector to erase.
void norflash_erase_sector(uint32_t addr);
void norflash_read(uint32_t addr, uint8_t *buf, uint16_t len);
// This function will automatically check if the data in the address is all 0xFF and write the data directly.
// If the data in the address is not all 0xFF, the function will erase the sector before writing the data.
// void norflash_write(uint32_t addr, uint8_t *buf, uint16_t len);

/**
 * @brief 不检查所写入的地址内数据是否全部为0xFF，直接写入数据
 *
 * @param addr The address to write
 * @param buf The buffer to write
 * @param len The length of the buffer
 * @note 该函数不检查所写入的地址内数据是否全部为0xFF，直接写入数据, 请确保所写入的地址内数据全部为0xFF
 * @note 该函数具有自动换页功能，可以写入超过一页的数据
 * @note 该函数不会检查地址是否超出范围
 */
void norflash_write_no_check(uint32_t addr, uint8_t *buf, uint16_t len);

#endif /* __NORFLASH_H__ */
