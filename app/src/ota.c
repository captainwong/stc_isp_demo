#include "ota.h"

#include <bsp/norflash.h>

#include "uart.h"

#define OTA_TIMEOUT_MAX 20    // seconds
#define OTA_CHECK_INTERVAL 5  // seconds

enum {
    OTA_STATE_IDLE,
    OTA_STATE_TIMEUP,
    OTA_STATE_TIMEOUT,
    OTA_STATE_CHECKING,
    OTA_STATE_PREPARING,
};

static app_info_t *papp = NULL;
static uint8_t ota_state = OTA_STATE_IDLE;
static uint16_t ota_timeout = 0;
static uint16_t ota_1s_counter = 0;

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
    } else if (ota_info.current_app == FLASH_APP_ID_APP1) {
        papp = &ota_info.app1;
    } else {  // ota_info.current_app == FLASH_APP_ID_APP2
        papp = &ota_info.app2;
    }
    debugf4("App%bu info: size=0x%08lX, crc=0x%08lX", ota_info.current_app, papp->size, papp->crc);
    debugf4("Version: %bu.%bu.%u",
            version_major(papp->version),
            version_minor(papp->version),
            version_patch(papp->version));
}

void ota_1s_event(void) {
    if (++ota_1s_counter == OTA_CHECK_INTERVAL) {
        ota_1s_counter = 0;
        if (ota_state == OTA_STATE_IDLE) {
            ota_state = OTA_STATE_TIMEUP;
        } else if (ota_state >= OTA_STATE_CHECKING && ota_timeout) {
            if (--ota_timeout == 0) {
                ota_state = OTA_STATE_TIMEOUT;
            }
        }
    }
}

void ota_on_latest_app_info(const latest_app_info_t *info) {
    if (ota_state == OTA_STATE_CHECKING) {
        debugf4("Latest App info: result=%bu, size=0x%08lX, crc=0x%08lX",
                info->result, info->info.size, info->info.crc);
        debugf4("Version: %bu.%bu.%u",
                version_major(info->info.version),
                version_minor(info->info.version),
                version_patch(info->info.version));
        switch (info->result) {
            case OTA_OK:
                ota_timeout = OTA_TIMEOUT_MAX;
                if (info->info.version > papp->version) {
                    debugf1("Newer app found, preparing to download...");
                    ota_state = OTA_STATE_PREPARING;
                } else {
                    debugf1("No newer app");
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

void ota_run(void) {
    switch (ota_state) {
        case OTA_STATE_IDLE:
            break;
        case OTA_STATE_TIMEUP:
            ota_state = OTA_STATE_CHECKING;
            uart1_send_check_ota(papp);
            ota_timeout = OTA_TIMEOUT_MAX;
            break;
        case OTA_STATE_TIMEOUT:
            ota_timeout = OTA_TIMEOUT_MAX;
            ota_state = OTA_STATE_IDLE;
            debugf1("OTA check timeout");
            break;
        case OTA_STATE_PREPARING:
            break;
        case OTA_STATE_CHECKING:
            break;
        default:
            ota_timeout = OTA_TIMEOUT_MAX;
            ota_state = OTA_STATE_IDLE;
            break;
    }
}
