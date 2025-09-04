#include <bsp/norflash.h>

uint16_t norflash_type = 0;

static void norflash_wait_busy(void) {
    /* B0 in Status Register-1 is Erase/Write In Progress(BUSY) - status only bit
    BUSY is a read only bit in the status register (S0) that is set to a 1 state when the device is executing a
    Page Program, Quad Page Program, Sector Erase, Block Erase, Chip Erase, Write Status Register or
    Erase/Program Security Register instruction. During this time the device will ignore further instructions
    except for the Read Status Register and Erase/Program Suspend instruction (see tW, tPP, tSE, tBE, and
    tCE in AC Characteristics). When the program, erase or write status/security register instruction has
    completed, the BUSY bit will be cleared to a 0 state indicating the device is ready for further instructions. */
    uint8_t sr;
    do {
        sr = norflash_read_sr(1);
    } while (sr & 0x01);
}

static void norflash_send_addr(uint32_t addr) {
    if (norflash_type == NFT_W25Q256) {
        spi_send(addr >> 24);
    }
    spi_send(addr >> 16);
    spi_send(addr >> 8);
    spi_send(addr);
}

/**
 * @brief 在一页内写入最多256字节的数据
 *
 * @param addr The address to write
 * @param buf The buffer to write
 * @param len The length of the buffer
 * @note 写入的长度不能超过该页的剩余大小
 */
static void norflash_write_page(uint32_t addr, uint8_t *buf, uint16_t len) {
    uint16_t i;
    norflash_enable_write();

    spi_select();
    spi_send(NFCMD_PAGE_PROGRAM);
    norflash_send_addr(addr);
    for (i = 0; i < len; i++) {
        spi_send(buf[i]);
    }
    spi_unselect();

    norflash_wait_busy();
}

bool norflash_init(void) {
    norflash_powerup();
    norflash_read_id();

    if (norflash_type == 0) {
        return false;
    }

    // check if we need to enable 4-byte addressing
    if (norflash_type == NFT_W25Q256) {
        uint8_t sr3 = norflash_read_sr(3);
        /* B0 in Status Register-3 is Current Address Mode(ADS) bit
        Its a read only bit that indicates which address mode the device is currently operating in.
        When ADS=0, the device is in 3-byte address mode,
        When ADS=1, the device is in 4-byte address mode.  */
        if ((sr3 & 0x01) == 0) {  // 3-byte addressing, we need to enable 4-byte addressing
            norflash_enable_write();
            /* B1 in Status Register-3 is Power-Up Address Mode(ADP) bit
            The ADP bit is a non-volatile bit that determines the initial address mode when
            the device is powered on or reset. The bit is only used during the power on or
            device reset initialization period, and it is only writable by the non-volatile
            Write Status sequence (06H + 11H).
            When ADP=0(factory default), the device will power up into 3-byte address mode,
            the Extended Address Register must be used to access memory regions beyond 128Mb.
            When ADP=1, the device will power up into 4-byte address mode directly. */

            // 1. set ADP bit to 1 to save our time for the next power on
            sr3 |= 1 << 1;
            norflash_write_sr(3, sr3);
            delay_ms(20);  // wait for the device to be ready

            // 2. enable 4-byte addressing
            spi_select();
            spi_send(NFCMD_ENABLE_4BYTE_ADDR);
            spi_unselect();
        }
    }

    return true;
}

void norflash_powerup(void) {
    spi_select();
    spi_send(NFCMD_RELEASE_POWER_DOWN);
    spi_unselect();
    delay_ms(20);  // wait for the device to be ready
}

void norflash_powerdown(void) {
    spi_select();
    spi_send(NFCMD_POWER_DOWN);
    spi_unselect();
}

bool norflash_read_id(void) {
    norflash_type = 0;

    spi_select();
    spi_send(NFCMD_MANUFACTURER_DEVICE_ID);
    spi_send(0);
    spi_send(0);
    spi_send(0);
    norflash_type |= spi_recv();
    norflash_type <<= 8;
    norflash_type |= spi_recv();
    spi_unselect();

    return norflash_type != 0;
}

const char *norflash_get_type_string(void) {
    switch (norflash_type) {
#define XX(type, name) \
    case type:         \
        return #name;
        NORFLASH_TYPES_MAP(XX)
#undef XX
        default:
            return "Unknown Flash Type";
    }
}

void norflash_enable_write(void) {
    spi_select();
    spi_send(NFCMD_WRITE_ENABLE);
    spi_unselect();
}

uint8_t norflash_read_sr(uint8_t reg) {
    uint8_t cmd = NFCMD_READ_STATUS_REG1, sr;
    switch (reg) {
        case 1:
            cmd = NFCMD_READ_STATUS_REG1;
            break;
        case 2:
            cmd = NFCMD_READ_STATUS_REG2;
            break;
        case 3:
            cmd = NFCMD_READ_STATUS_REG3;
            break;
    }
    spi_select();
    spi_send(cmd);
    sr = spi_recv();
    spi_unselect();
    return sr;
}

void norflash_write_sr(uint8_t reg, uint8_t value) {
    uint8_t cmd = NFCMD_WRITE_STATUS_REG1;
    switch (reg) {
        case 1:
            cmd = NFCMD_WRITE_STATUS_REG1;
            break;
        case 2:
            cmd = NFCMD_WRITE_STATUS_REG2;
            break;
        case 3:
            cmd = NFCMD_WRITE_STATUS_REG3;
            break;
    }
    norflash_enable_write();
    spi_select();
    spi_send(cmd);
    spi_send(value);
    spi_unselect();
}

void norflash_erase_chip(void) {
    norflash_enable_write();
    norflash_wait_busy();
    spi_select();
    spi_send(NFCMD_CHIP_ERASE);
    spi_unselect();
    norflash_wait_busy();
}

void norflash_erase_sector(uint32_t addr) {
    norflash_enable_write();
    norflash_wait_busy();
    spi_select();
    spi_send(NFCMD_SECTOR_ERASE);
    norflash_send_addr(addr);
    spi_unselect();
    norflash_wait_busy();
}

void norflash_read(uint32_t addr, uint8_t *buf, uint16_t len) {
    uint16_t i;
    spi_select();
    spi_send(NFCMD_READ_DATA);
    norflash_send_addr(addr);
    for (i = 0; i < len; i++) {
        buf[i] = spi_recv();
    }
    spi_unselect();
}

// static bool is_all_ff(uint8_t *buf, uint16_t len) {
//     uint16_t i;
//     for (i = 0; i < len / 4; i++) {
//         if (*(uint32_t *)buf != 0xFFFFFFFF) {
//             return false;
//         }
//         buf += 4;
//     }
//     len &= 0x03;
//     for (uint16_t i = 0; i < len; i++) {
//         if (*buf != 0xFF) {
//             return false;
//         }
//         buf++;
//     }
//     return true;
// }

// void norflash_write(uint32_t addr, uint8_t *buf, uint16_t len) {
//     uint32_t sector_addr = addr & 0xFFFFF000;
//     uint16_t sector_offset = addr & 0x00000FFF;
//     uint16_t sector_remain = 4096 - sector_offset;
//     if (len <= sector_remain) {
//         sector_remain = len;
//     }

//     while (len) {
//         norflash_read(sector_addr, sector_buf, 4096);
//         if (!is_all_ff(sector_buf, 4096)) {
//             // erase the sector
//             norflash_erase_sector(sector_addr);
//             memcpy(sector_buf + sector_offset, buf, sector_remain);
//             norflash_write_no_check(sector_addr, sector_buf, 4096);
//         } else {
//             // if the sector is all 0xFF, we don't need to erase it
//             norflash_write_no_check(addr, buf, sector_remain);
//         }

//         addr += sector_remain;
//         buf += sector_remain;
//         len -= sector_remain;

//         sector_addr += 4096;
//         sector_offset = 0;
//         sector_remain = 4096;
//         if (len <= sector_remain) {
//             sector_remain = len;
//         }
//     }
// }

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
void norflash_write_no_check(uint32_t addr, uint8_t *buf, uint16_t len) {
    while (len > 0) {
        uint16_t page_remain = 256 - (addr & 0xFF);
        uint16_t write_len = len < page_remain ? len : page_remain;
        norflash_write_page(addr, buf, write_len);
        addr += write_len;
        buf += write_len;
        len -= write_len;
    }
}
