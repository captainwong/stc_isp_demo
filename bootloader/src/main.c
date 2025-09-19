#include <libhbcheck/hb_check.h>

#include "bsp/norflash.h"
#include "protocol.h"
#include "sys/build_time.h"
#include "sys/gpio.h"
#include "sys/version.h"
#include "uart.h"

system_context_t xdata sysctx __at(SYSTEM_CONTEXT_ADDR);
uint16_t cycles;
uint16_t rx_time, rx_timeout;

app_info_t xdata meta;
ota_info_t xdata ota_info;

void isp_handle(void);

void delay() {
    volatile uint16_t data i = 0;
#ifdef DEBUG
    volatile uint8_t data j;
    while (++i) {
        j = 200;
        while (++j) {
            nop1();
        }
    }
#else
    while (++i) {
        nop1();
    }
#endif
}

void check_onchip_factory_metadata(void);
void copy_onchip_factory_app_to_norflash(void);
void check_ota(void);

void main() {
    delay();
    disable_irq();
    isp_parse_init(ctx);
    gpio_init();
    pin_pu(3, 5);       // KEY_REBOOT 上拉
    sysctx.st.ldr = 1;  // indicate running in bootloader mode
    sysctx.st.onchip_app_valid = is_valid_on_chip_app_program();
    sysctx.st.onchip_meta_valid = false;
    sysctx.st.appid = FLASH_APP_ID_FACTORY;
    sysctx.st.otaid = FLASH_OTA_ID_MASTER;
    uart_init();
    enable_irq();
    led_run_on();
    enable_xsfr();  // for xsfr `IAP_TPS`
    iap_tps(iap_calc_tps(MAIN_Fosc));

    while (!norflash_init()) {
        debugf1("Norflash init failed");
        led_run_toggle();
        delay_ms(1000);
    }
    debugf3("Norflash init ok, type=%04X, %s", norflash_type, norflash_get_type_string());

    debugf2("sysctx=0b%s", unsafe_u8_to_bits(sysctx.b));
    debugf4("first 3 byte: %02bX %02bX %02bX",
            *(uint8_t code *)(IAP_ADDR_APP_START),
            *(uint8_t code *)(IAP_ADDR_APP_START + 1),
            *(uint8_t code *)(IAP_ADDR_APP_START + 2));

    if (sysctx.st.onchip_app_valid) {
        // check if its the first time boot after firmware upgrade
        debugf1("Valid application found, checking factory metadata");
        check_onchip_factory_metadata();
        if (sysctx.st.onchip_meta_valid) {
            copy_onchip_factory_app_to_norflash();
        }
    }

    if (!sysctx.st.dfu) {  // check if the force DFU mode flag was set by application
        // check_ota();
        if (sysctx.st.onchip_app_valid) {
            debugf1("Jump to application");
            uart_release();
            sysctx.st.dfu = 0;               // clear force DFU mode flag
            sysctx.st.ldr = 0;               // indicate current mode is application mode
            jump_to_on_chip_app_program(0);  // LJMP #IAP_ADDR_APP_START, from here the CPU is running application code
        } else {
            debugf1("No valid application, stay in bootloader");
            sysctx.st.dfu = 1;  // force DFU mode
        }
    }

    // now CPU is running bootloader code

    debugf1("bootloader running");
    sysctx.st.dfu = 0;  // clear force DFU mode flag
    cycles = 0;

    while (1) {
        uart_run();
        if (isp_parse_ok) {
            isp_parse_ok = false;
            rx_time = rx_timeout = cycles;
            isp_handle();
        }

        if (++rx_timeout == rx_time) {  // 转一圈还没有新的数据，说明这一包超时
            isp_parse_init(ctx);
        }

        if (++cycles == 0) {
            led_run_toggle();
        }

        if (key_reboot_pressed()) {
            sys_reset();
            while (1);
        }
    }
}

void isp_handle(void) {
    uint32_t addr = rx.pkt.addr;
    tx.pkt.status = LDR_STATUS_OK;
    tx.pkt.size = 0;

    switch (rx.pkt.cmd) {
        case ISP_CMD_CONNECT:
            tx.pkt.size = 2;
            tx.pkt.dat[0] = LDR_VERSION_MAJOR;
            tx.pkt.dat[1] = LDR_VERSION_MINOR;
            break;
        case ISP_CMD_READ:
            tx.pkt.status = LDR_STATUS_ROM;
            tx.pkt.size = rx.pkt.size;
            iap_read_bytes(addr, tx.pkt.dat, rx.pkt.size);
            break;
        case ISP_CMD_PROGRAM:
            if (!iap_write_bytes_check(IAP_ADDR_APP_START + addr, rx.pkt.dat, rx.pkt.size)) {
                tx.pkt.status = LDR_STATUS_PROGRAM_FAILED;
            }
            break;
        case ISP_CMD_ERASE_APP_AREA:
            for (addr = IAP_ADDR_APP_START; addr < IAP_ADDR_APP_END; addr += IAP_PAGE_SIZE) {
                if (!iap_erase_page_check(addr)) {
                    tx.pkt.status = LDR_STATUS_PROGRAM_FAILED;
                    break;
                }
            }
            break;
        case ISP_CMD_ERASE_PAGE:
            if (!iap_erase_page_check(addr)) {
                tx.pkt.status = LDR_STATUS_ERASE_PAGE_FAILED;
            }
            break;
        case ISP_CMD_REBOOT:
            sys_reset();
            break;
        case ISP_CMD_READ_CHIP_INFO:
            tx.pkt.status = LDR_STATUS_CHIP_INFO;
            tx.pkt.size = sizeof(stc_chipid_t);
            *(stc_chipid_t *)tx.pkt.dat = STC_CHIPID();
            break;
        case ISP_CMD_READ_CHIP_VERSION:
            tx.pkt.status = LDR_STATUS_CHIP_VERSION;
            tx.pkt.size = 1;
            tx.pkt.dat[0] = stc_chip_version();
            break;
        case ISP_CMD_READ_LDR_VERSION:
            tx.pkt.status = LDR_STATUS_LDR_VERSION;
            tx.pkt.size = 8;
            *(uint32_t *)&tx.pkt.dat[0] = LDR_VERSION;
            *(uint32_t *)&tx.pkt.dat[4] = LDR_BUILD_TIME;
            break;
        case ISP_CMD_READ_W25Q_SIZE:
            tx.pkt.status = LDR_STATUS_W25Q_SIZE;
            tx.pkt.size = 2;
            tx.pkt.dat[0] = (uint8_t)(norflash_type >> 8);
            tx.pkt.dat[1] = (uint8_t)(norflash_type & 0xFF);
            break;
        case ISP_CMD_ERASE_W25Q_ALL:
            norflash_erase_chip();
            tx.pkt.status = LDR_STATUS_W25Q_ERASE_ALL_RES;
            break;
        case ISP_CMD_ERASE_W25Q_SECTOR:
            addr = rev32(*(uint32_t *)&rx.pkt.dat[0]);
            norflash_erase_sector(addr);
            *(uint32_t *)&tx.pkt.dat[0] = *(uint32_t *)&rx.pkt.dat[0];  // echo back addr
            tx.pkt.size = 4;
            tx.pkt.status = LDR_STATUS_W25Q_ERASE_SECTOR_RES;
            break;
        case ISP_CMD_READ_W25Q:
            addr = rev32(*(uint32_t *)&rx.pkt.dat[0]);
            if (rx.pkt.size > 128) {
                rx.pkt.size = 128;  // limit max read size
            }
            *(uint32_t *)&tx.pkt.dat[0] = *(uint32_t *)&rx.pkt.dat[0];  // echo back addr
            norflash_read(addr, &tx.pkt.dat[4], rx.pkt.size);
            tx.pkt.size = rx.pkt.size + 4;
            tx.pkt.status = LDR_STATUS_W25Q_DATA;
            break;
        case ISP_CMD_PROGRAM_W25Q:
            addr = rev32(*(uint32_t *)&rx.pkt.dat[0]);
            if (rx.pkt.size > 128) {
                rx.pkt.size = 128;  // limit max program size
            }
            norflash_write_no_check(addr, &rx.pkt.dat[4], rx.pkt.size);
            *(uint32_t *)&tx.pkt.dat[0] = *(uint32_t *)&rx.pkt.dat[0];  // echo back addr
            norflash_read(addr, &tx.pkt.dat[4], rx.pkt.size);           // read back for verify
            tx.pkt.size = 4 + rx.pkt.size;
            tx.pkt.status = LDR_STATUS_W25Q_PROGRAM_RES;
            break;
        case ISP_CMD_CALC_CRC32: {
            uint32_t crc = hb_crc32_slow_init();
            crc = hb_crc32_slow_update(crc, rx.pkt.dat, rx.pkt.size);
            crc = hb_crc32_slow_finalize(crc);
            tx.pkt.status = LDR_STATUS_CALC_CRC32_RES;
            *(uint32_t *)&tx.pkt.dat[0] = crc;
            tx.pkt.size = 4;
            break;
        }

        default:
            tx.pkt.status = LDR_STATUS_UNKNOWN_CMD;
            break;
    }
    uart_send_tx();
}

/**
 * @brief Check factory metadata in on-chip flash
 * If valid, copy factory application from on-chip flash to norflash factory app area,
 * and erase on-chip flash factory metadata area
 */
void check_onchip_factory_metadata(void) {
    uint8_t dat;
    uint16_t addr, i;
    uint32_t crc = hb_crc32_slow_init();

    // check if meta valid
    iap_read_bytes(IAP_ADDR_FACTORY_META, (uint8_t *)&meta, sizeof(app_info_t));
    // meta = *(app_info_t code *)IAP_ADDR_FACTORY_META; // 居然是iap函数占用更少的ROM空间
    debugf2("Checking factory metadata, size=0x%08lX", meta.size);
    debugf2("crc=0x%08lX", meta.crc);
    debugf2("timestamp=0x%08lX", meta.timestamp);
    debugf2("version=0x%08lX", meta.version);
    debugf4("Version: %bu.%bu.%u",
            version_major(meta.version),
            version_minor(meta.version),
            version_patch(meta.version));
    if (meta.size == 0 || meta.size > APP_MAX_SIZE) {
        sysctx.st.onchip_meta_valid = 0;
        debugf2("No factory metadata, size=0x%08lX", meta.size);
        return;
    }

    // check meta crc
    for (i = 0, addr = IAP_ADDR_APP_START; i < meta.size; i++) {
        dat = iap_read_byte(addr++);
        crc = hb_crc32_slow_update1(crc, dat);
    }
    crc = hb_crc32_slow_finalize(crc);
    if (crc != (meta.crc)) {
        sysctx.st.onchip_meta_valid = 0;
        debugf3("Factory metadata crc error, calc=0x%08lX, meta=0x%08lX", crc, (meta.crc));
        return;
    }

    // meta valid
    sysctx.st.onchip_meta_valid = 1;
    debugf1("Factory metadata valid");
}

// 上电时若检测到有片上合法固件和元数据，则将片上固件复制到外部Flash的Factory App区域，
// 并擦除片上固件元数据，防止下次上电时重复复制
// 这是为了生产方便，第一次烧录完整64KB固件即可自动将出厂固件复制到外部Flash
void copy_onchip_factory_app_to_norflash(void) {
    uint32_t iap_addr, flash_addr, i, app_size;
    ota_info_t xdata ota_verify;

    union {
        uint8_t total[NORFLASH_PAGE_SIZE];
        struct {
            uint8_t a[NORFLASH_PAGE_SIZE / 2];
            uint8_t b[NORFLASH_PAGE_SIZE / 2];
        } split;
    } xdata buf;

    debugf1("Copying factory app to norflash...");

    /////////////////////////// 1. copy factory app to norflash ///////////////////////////

    // 1.1 erase factory app area
    debugf1("Erasing norflash factory app area...");
    // no need to erase all NORFLASH_APP_SIZE, just erase the sectors that will be written
    app_size = (meta.size + NORFLASH_SECTOR_SIZE - 1) & ~(NORFLASH_SECTOR_SIZE - 1);
    debugf3("App size=0x%08lX, erase size=0x%08lX", meta.size, app_size);
    flash_addr = NORFLASH_FACTORY_APP_ADDR;
    for (i = 0; i < app_size; i += NORFLASH_SECTOR_SIZE) {
        debugf2("Erasing sector at 0x%08lX...", flash_addr);
        norflash_erase_sector(flash_addr);
        flash_addr += NORFLASH_SECTOR_SIZE;
    }
    debugf1("Erase done.");

    // 1.2 copy data page by page
    debugf1("Copying...");
    iap_addr = IAP_ADDR_APP_START;
    flash_addr = NORFLASH_FACTORY_APP_ADDR;
    for (i = 0; i < meta.size;) {
        uint16_t len = (meta.size - i) > NORFLASH_PAGE_SIZE ? NORFLASH_PAGE_SIZE : (meta.size - i);
        iap_read_bytes(iap_addr, buf.total, len);
        norflash_write_page(flash_addr, buf.total, len);
        i += len;
        iap_addr += len;
        flash_addr += len;
    }
    debugf1("Copy done.");

    // 1.3 read back verify
    debugf1("Verifying...");
    iap_addr = IAP_ADDR_APP_START;
    flash_addr = NORFLASH_FACTORY_APP_ADDR;
    for (i = 0; i < meta.size;) {
        // its safe to use uint16_t here because meta.size <= APP_MAX_SIZE < 64KB
        uint16_t len = (meta.size - i);
        if (len > sizeof(buf.split.a)) {
            len = sizeof(buf.split.a);
        }
        iap_read_bytes(iap_addr, buf.split.a, len);
        norflash_read(flash_addr, buf.split.b, len);
        if (memcmp(buf.split.a, buf.split.b, len) != 0) {
            debugf2("Verify failed! flash_addr=0x%08lX", flash_addr);
            sys_reset();
            while (1);
        }
        i += len;
        iap_addr += len;
        flash_addr += len;
    }
    debugf1("Verify success.");

    /////////////////////////// 2. update ota info ///////////////////////////

    // 2.1 find out which ota info is older(smaller seq)
    findout_which_ota_info_is_older();
    // 2.2 read out the older ota info
    if (sysctx.st.otaid == FLASH_OTA_ID_MASTER) {
        norflash_read(NORFLASH_OTA_MASTER_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
    } else {
        norflash_read(NORFLASH_OTA_BACKUP_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
    }
    // 2.3 update ota info
    ota_info.seq++;  // make it newer
    ota_info.current_app = FLASH_APP_ID_FACTORY;
    ota_info.factory.info = meta;
    invalidate_flash_app_info(ota_info.app1);
    invalidate_flash_app_info(ota_info.app2);
    invalidate_flash_app_download_ctx(ota_info.dlctx);
    // 2.4 write updated ota info
    if (sysctx.st.otaid == FLASH_OTA_ID_MASTER) {
        norflash_erase_sector(NORFLASH_OTA_MASTER_ADDR);
        norflash_write_page(NORFLASH_OTA_MASTER_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
    } else {
        norflash_erase_sector(NORFLASH_OTA_BACKUP_ADDR);
        norflash_write_page(NORFLASH_OTA_BACKUP_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
    }
    // 2.5 verify updated ota info
    if (sysctx.st.otaid == FLASH_OTA_ID_MASTER) {
        norflash_read(NORFLASH_OTA_MASTER_ADDR, (uint8_t *)&ota_verify, sizeof(ota_info_t));
    } else {
        norflash_read(NORFLASH_OTA_BACKUP_ADDR, (uint8_t *)&ota_verify, sizeof(ota_info_t));
    }
    if (memcmp(&ota_info, &ota_verify, sizeof(ota_info_t)) != 0) {
        debugf1("Ota info verify failed!");
        sys_reset();
        while (1);
    }
    // 2.6 update otaid
    sysctx.st.otaid = (sysctx.st.otaid == FLASH_OTA_ID_MASTER) ? FLASH_OTA_ID_BACKUP : FLASH_OTA_ID_MASTER;

    /////////////////////////// 3. erase on-chip factory metadata ///////////////////////////
    debugf1("Erasing on-chip factory metadata...");
    if (!iap_erase_page_check(IAP_ADDR_FACTORY_META)) {
        debugf1("Erase on-chip factory metadata failed!");
        sys_reset();
        while (1);
    }
    debugf1("Erase done.");
    sysctx.st.onchip_meta_valid = 0;
}

// bool try_load_flash_app(uint8_t appid) {
//     uint32_t iap_addr, flash_addr, i, app_size, crc;
//     flash_app_info_t *papp = NULL;

//     // read app info
//     if (appid == FLASH_APP_ID_FACTORY) {
//         flash_addr = NORFLASH_FACTORY_APP_ADDR;
//         papp = &ota_info.factory;
//     } else if (appid == FLASH_APP_ID_APP1) {
//         flash_addr = NORFLASH_APP1_ADDR;
//         papp = &ota_info.app1;
//     } else {  // appid == FLASH_APP_ID_APP2
//         flash_addr = NORFLASH_APP2_ADDR;
//         papp = &ota_info.app2;
//     }
//     debugf4("App%bu info: size=0x%08lX, crc=0x%08lX", appid, papp->info.size, papp->info.crc);
//     debugf4("Version: %bu.%bu.%u",
//             version_major(papp->info.version),
//             version_minor(papp->info.version),
//             version_patch(papp->info.version));
//     if (papp->info.size == 0 || papp->info.size > APP_MAX_SIZE) {
//         debugf2("App size invalid, size=0x%08lX", papp->info.size);
//         return false;
//     }

//     // validate crc


//     return true;
// }

// /** boot order:
//  * 1. current_app
//  * 2. the other app
//  * 3. factory app
//  * 4. on-chip app
//  */
// void check_ota(void) {
//     uint8_t boot[3], n = 0, i;
//     bool retry_failed = false;

//     debugf1("Checking ota info...");
//     // find out which ota info is older(smaller seq)
//     findout_which_ota_info_is_older();

// retry:
//     // read out the newer ota info
//     if (sysctx.st.otaid == FLASH_OTA_ID_MASTER) {
//         debugf1("Loading ota info from backup");
//         norflash_read(NORFLASH_OTA_BACKUP_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
//     } else {
//         debugf1("Loading ota info from master");
//         norflash_read(NORFLASH_OTA_MASTER_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
//     }
//     debugf3("ota_info.seq=0x%08lX, current_app=%bu", ota_info.seq, ota_info.current_app);
//     if (ota_info.current_app > FLASH_APP_ID_MAX) {
//         debugf1("No valid current_app");
//         if (!retry_failed) {
//             sysctx.st.otaid = (sysctx.st.otaid == FLASH_OTA_ID_MASTER) ? FLASH_OTA_ID_BACKUP : FLASH_OTA_ID_MASTER;
//             retry_failed = true;
//             goto retry;
//         } else {
//             debugf1("No valid app on flash");
//             return;
//         }
//     } else if(ota_info.current_app == FLASH_APP_ID_FACTORY) {
//         boot[n++] = FLASH_APP_ID_FACTORY;
//     } else {
//         boot[n++] = ota_info.current_app;
//         boot[n++] = (ota_info.current_app == FLASH_APP_ID_APP1) ? FLASH_APP_ID_APP2 : FLASH_APP_ID_APP1;
//         boot[n++] = FLASH_APP_ID_FACTORY;
//     }

//     for (i = 0; i < n; i++) {
//         debugf2("Trying to load app%bu...", boot[i]);
//         if (try_load_flash_app(boot[i])) {
//             sysctx.st.appid = boot[i];
//             sysctx.st.onchip_app_valid = true;
//             debugf2("App%bu loaded", boot[i]);
//             return;
//         } else {
//             debugf2("Load app%bu failed", boot[i]);
//         }
//     }
// }
