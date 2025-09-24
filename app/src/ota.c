#include "ota.h"

#include <bsp/norflash.h>
#include <libhbcheck/hb_check.h>

#include "uart.h"

#define OTA_TIMEOUT_MAX 20    // seconds
#define OTA_CHECK_INTERVAL 5  // seconds

enum {
    OTA_STATE_IDLE,
    OTA_STATE_TIMEUP,
    OTA_STATE_TIMEOUT,
    OTA_STATE_CHECKING,
    OTA_STATE_PREPARING,
    OTA_STATE_ERASING,
    OTA_STATE_DOWNLOADING,
    OTA_STATE_APPLYING,
    OTA_STATE_ERROR,
};

static app_info_t *papp = NULL;   // current running app info
static app_info_t *pappu = NULL;  // app info of the app to be upgraded
static uint8_t ota_state = OTA_STATE_IDLE;
static uint16_t ota_timeout = 0;
static uint16_t ota_1s_counter = 0;
static uint8_t appid_to_upgrade = 0;
static uint32_t flash_addr = 0;
static uint32_t erase_size = 0;
static uint32_t addr = 0;

static void request_next_block(void) {
    get_app_data_req_t req;
    req.offset = ota_info.dlctx.received;
    req.size = pappu->size - ota_info.dlctx.received;
    if (req.size > PKT_DAT_MAX_LEN) {
        req.size = PKT_DAT_MAX_LEN;
    }
    req.version = pappu->version;
    uart1_send_get_app_data(&req);
}

static bool verify_app_data_res(const get_app_data_res_t *res) {
    uint32_t crc = hb_crc32_slow_init();
    crc = hb_crc32_slow_update(crc, res->dat, res->size);
    crc = hb_crc32_slow_finalize(crc);
    debugf3("calc=0x%08lX, recv=0x%08lX", crc, res->crc);
    return crc == res->crc;
}

void ota_init(void) {
    findout_which_ota_info_is_older();
    debugf2("otaid=%bu", sysctx.st.otaid);
    // read out the newer ota info
    if (sysctx.st.otaid == FLASH_OTA_ID_MASTER) {
        norflash_read(NORFLASH_OTA_BACKUP_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
    } else {
        norflash_read(NORFLASH_OTA_MASTER_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
    }

    if (ota_info.current_app == FLASH_APP_ID_FACTORY) {
        papp = &ota_info.factory;
        pappu = &ota_info.app1;
        flash_addr = NORFLASH_APP1_ADDR;
        appid_to_upgrade = FLASH_APP_ID_APP1;
    } else if (ota_info.current_app == FLASH_APP_ID_APP1) {
        papp = &ota_info.app1;
        pappu = &ota_info.app2;
        flash_addr = NORFLASH_APP2_ADDR;
        appid_to_upgrade = FLASH_APP_ID_APP2;
    } else {  // ota_info.current_app == FLASH_APP_ID_APP2
        papp = &ota_info.app2;
        pappu = &ota_info.app1;
        flash_addr = NORFLASH_APP1_ADDR;
        appid_to_upgrade = FLASH_APP_ID_APP1;
    }
    debugf4("Current App%bu info: size=0x%08lX, crc=0x%08lX", ota_info.current_app, papp->size, papp->crc);
    debugf4("Version: %bu.%bu.%u",
            version_major(papp->version),
            version_minor(papp->version),
            version_patch(papp->version));
    debugf2("App ready to upgrade:%bu", appid_to_upgrade);
    debugf2("dlctx.state=%bu", ota_info.dlctx.state);
    ota_info.dlctx.appid = appid_to_upgrade;  // ensure dlctx.appid is correct

    switch (ota_info.dlctx.state) {
        case FLASH_APP_DL_STATE_INVALID:
        case FLASH_APP_DL_STATE_IDLE:
            // normal state
            break;
        case FLASH_APP_DL_STATE_ERASING:
        case FLASH_APP_DL_STATE_DOWNLOADING:  // TODO: resume download
        case FLASH_APP_DL_STATE_VERIFYING:
        case FLASH_APP_DL_STATE_APPLYING:
        default:
            // abnormal state, reset to IDLE
            ota_info.dlctx.state = FLASH_APP_DL_STATE_IDLE;
            ota_info.seq++;
            // write back to norflash
            if (sysctx.st.otaid == FLASH_OTA_ID_MASTER) {
                norflash_erase_sector(NORFLASH_OTA_MASTER_ADDR);
                norflash_write_page(NORFLASH_OTA_MASTER_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
            } else {
                norflash_erase_sector(NORFLASH_OTA_BACKUP_ADDR);
                norflash_write_page(NORFLASH_OTA_BACKUP_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
            }
            sysctx.st.otaid = !(sysctx.st.otaid);
            debugf1("Abnormal dlctx.state, reset to IDLE");
            break;
    }
}

void ota_1s_event(void) {
    if (ota_state == OTA_STATE_IDLE &&
        ++ota_1s_counter == OTA_CHECK_INTERVAL) {
        ota_1s_counter = 0;
        ota_state = OTA_STATE_TIMEUP;
    }

    if (ota_timeout &&
        ota_state >= OTA_STATE_CHECKING &&
        ota_state <= OTA_STATE_DOWNLOADING) {
        if (--ota_timeout == 0) {
            if (ota_state == OTA_STATE_DOWNLOADING) {
                ota_timeout = OTA_TIMEOUT_MAX;
                request_next_block();
            } else {
                ota_state = OTA_STATE_TIMEOUT;
            }
        }
    }
}

void ota_on_latest_app_info(const latest_app_info_t *info) {
    if (ota_state == OTA_STATE_CHECKING) {
        debugf3("Latest App info: result=%bu %s",
                info->result, ota_result_to_string(info->result));
        debugf3("size=0x%08lX, crc=0x%08lX", info->info.size, info->info.crc);
        debugf4("Version: %bu.%bu.%u",
                version_major(info->info.version),
                version_minor(info->info.version),
                version_patch(info->info.version));
        switch (info->result) {
            case OTA_OK:
                if (info->info.version > papp->version &&
                    info->info.size > 0 &&
                    info->info.size <= APP_MAX_SIZE) {
                    debugf1("Newer app found, preparing to download...");
                    *pappu = info->info;
                    ota_timeout = OTA_TIMEOUT_MAX;
                    ota_state = OTA_STATE_PREPARING;
                } else {
                    debugf1("No newer app");
                    ota_1s_counter = 0;
                    ota_state = OTA_STATE_IDLE;
                }
                break;

            case OTA_NO_NEW_VERSION:
                debugf1("No newer app");
                ota_1s_counter = 0;
                ota_state = OTA_STATE_IDLE;
                break;

            default:
                // let OTA_STATE_TIMEOUT handle it
                break;
        }
    }
}

void ota_on_app_data(const get_app_data_res_t *res) {
    if (ota_state == OTA_STATE_DOWNLOADING) {
        debugf3("Received app data: result=%bu %s",
                res->result, ota_result_to_string(res->result));
        debugf4("offset=0x%08lX, size=0x%08lX, crc=0x%08lX", res->offset, res->size, res->crc);
        if (res->result == OTA_OK &&
            verify_app_data_res(res) &&
            res->offset == ota_info.dlctx.received &&
            0 < res->size && res->size <= PKT_DAT_MAX_LEN) {
            uint8_t xdata buf[PKT_DAT_MAX_LEN];
            // write to norflash
            norflash_write_page(flash_addr + ota_info.dlctx.received, res->dat, res->size);
            // read out verify
            norflash_read(flash_addr + ota_info.dlctx.received, buf, res->size);
            if (memcmp(buf, res->dat, res->size) != 0) {
                debugf1("Norflash verify error");
                ota_state = OTA_STATE_ERROR;
                return;
            }
            ota_info.dlctx.received += res->size;
            ota_info.dlctx.crc = hb_crc32_slow_update(ota_info.dlctx.crc, res->dat, res->size);

            debugf3("download progress %lu/%lu",
                    ota_info.dlctx.received, pappu->size);
            debugf2("%02lu%%", ota_info.dlctx.received * 100 / pappu->size);

            if (ota_info.dlctx.received < pappu->size) {
                request_next_block();
            } else {
                ota_info.dlctx.crc = hb_crc32_slow_finalize(ota_info.dlctx.crc);
                debugf3("Downloaded 0x%08lX bytes, crc=0x%08lX", ota_info.dlctx.received, ota_info.dlctx.crc);
                if (ota_info.dlctx.crc == pappu->crc) {
                    debugf1("App data verified.");
                    ota_state = OTA_STATE_APPLYING;
                    ota_timeout = OTA_TIMEOUT_MAX;
                    // update ota_info
                    ota_info.seq++;
                    ota_info.current_app = appid_to_upgrade;
                    ota_info.dlctx.state = FLASH_APP_DL_STATE_IDLE;
                    ota_info.dlctx.received = 0;
                    ota_info.dlctx.crc = 0;
                    // write back to norflash
                    if (sysctx.st.otaid == FLASH_OTA_ID_MASTER) {
                        norflash_erase_sector(NORFLASH_OTA_MASTER_ADDR);
                        norflash_write_page(NORFLASH_OTA_MASTER_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
                    } else {
                        norflash_erase_sector(NORFLASH_OTA_BACKUP_ADDR);
                        norflash_write_page(NORFLASH_OTA_BACKUP_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
                    }
                    sysctx.st.otaid = !(sysctx.st.otaid);
                    debugf1("OTA update applied, will use new app on next reboot.");
                    ota_state = OTA_STATE_APPLYING;
                } else {
                    debugf3("App data crc error, calc=0x%08lX, info=0x%08lX", ota_info.dlctx.crc, pappu->crc);
                    ota_state = OTA_STATE_ERROR;
                }
            }
        } else {
            debugf1("ota error");
            ota_state = OTA_STATE_ERROR;
        }
    }
}

void ota_run(void) {
    switch (ota_state) {
        case OTA_STATE_IDLE:
            break;
        case OTA_STATE_TIMEUP:
            ota_state = OTA_STATE_CHECKING;
            uart1_send_check_ota(papp);
            ota_timeout = OTA_TIMEOUT_MAX;
            break;
        case OTA_STATE_CHECKING:
            break;
        case OTA_STATE_TIMEOUT:
            ota_timeout = OTA_TIMEOUT_MAX;
            ota_state = OTA_STATE_IDLE;
            debugf1("OTA check timeout");
            break;
        case OTA_STATE_PREPARING:
            erase_size = flash_addr + (pappu->size + NORFLASH_SECTOR_SIZE - 1) & ~(NORFLASH_SECTOR_SIZE - 1);  // 罪魁祸首，忘了+flash_addr
            addr = flash_addr;
            debugf3("App size=0x%08lX, erase size=0x%08lX", pappu->size, erase_size);
            ota_info.seq++;
            ota_info.dlctx.state = FLASH_APP_DL_STATE_ERASING;
            ota_info.dlctx.received = 0;
            ota_info.dlctx.crc = 0;
            // write back to norflash
            if (sysctx.st.otaid == FLASH_OTA_ID_MASTER) {
                norflash_erase_sector(NORFLASH_OTA_MASTER_ADDR);
                norflash_write_page(NORFLASH_OTA_MASTER_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
            } else {
                norflash_erase_sector(NORFLASH_OTA_BACKUP_ADDR);
                norflash_write_page(NORFLASH_OTA_BACKUP_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
            }
            sysctx.st.otaid = !(sysctx.st.otaid);
            debugf1("Erasing norflash app area...");
            ota_state = OTA_STATE_ERASING;
            break;
        case OTA_STATE_ERASING:
            if (addr < erase_size) {
                debugf2("Erasing sector at 0x%08lX...", addr);
                norflash_erase_sector(addr);
                addr += NORFLASH_SECTOR_SIZE;
            } else {
                debugf1("Erased.");
                addr = flash_addr;
                ota_info.seq++;
                ota_info.dlctx.state = FLASH_APP_DL_STATE_DOWNLOADING;
                ota_info.dlctx.received = 0;
                ota_info.dlctx.crc = hb_crc32_slow_init();
                // write back to norflash
                if (sysctx.st.otaid == FLASH_OTA_ID_MASTER) {
                    norflash_erase_sector(NORFLASH_OTA_MASTER_ADDR);
                    norflash_write_page(NORFLASH_OTA_MASTER_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
                } else {
                    norflash_erase_sector(NORFLASH_OTA_BACKUP_ADDR);
                    norflash_write_page(NORFLASH_OTA_BACKUP_ADDR, (uint8_t *)&ota_info, sizeof(ota_info_t));
                }
                sysctx.st.otaid = !(sysctx.st.otaid);
                ota_state = OTA_STATE_DOWNLOADING;
                debugf1("Start downloading...");
                request_next_block();
            }
            break;
        case OTA_STATE_DOWNLOADING:
            break;
        case OTA_STATE_APPLYING:
            // stay here forever, until next power up
            break;
        case OTA_STATE_ERROR:
            // stay here forever, until next power up
            break;
        default:
            ota_1s_counter = 0;
            ota_timeout = OTA_TIMEOUT_MAX;
            ota_state = OTA_STATE_IDLE;
            break;
    }
}
