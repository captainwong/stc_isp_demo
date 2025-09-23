#include <bsp/norflash.h>
#include <sys/build_time.h>
#include <sys/gpio.h>
#include <sys/sys.h>
#include <sys/version.h>

#include "timer.h"
#include "uart.h"

#define OTA_TIMEOUT_MAX 20  // seconds
#define OTA_CHECK_INTERVAL 5 // seconds

enum {
    OTA_STATE_IDLE,
    OTA_STATE_TIMEUP,
    OTA_STATE_TIMEOUT,
    OTA_STATE_CHECKING,
    OTA_STATE_PREPARING,
};

system_context_t xdata sysctx __at(SYSTEM_CONTEXT_ADDR);
app_info_t *papp = NULL;
uint8_t ota_state = OTA_STATE_IDLE;
uint16_t ota_timeout = 0;
uint16_t counter_1s = 0;

void isp_handle(void);
void ota_run(void);

void main() {
    enable_xsfr();
    gpio_init();
    pin_pu(3, 2);  // KEY_BOOT 上拉

    timer0_init();
    uart1_init();
    enable_irq();
    led_run_on();

    sysctx.st.dfu = 0;

    debugf1("App start");

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

    while (1) {
        uart1_run();
        if (isp_parse_ok) {
            isp_parse_ok = false;
            isp_handle();
        }

        if (key_boot_pressed()) {
            sysctx.st.dfu = 1;
            sys_reset();
        }

        if (flag_1s) {
            flag_1s = false;
            led_run_toggle();

            if (++counter_1s == 5) {  // 暂时每5秒检测一次有没有OTA更新
                counter_1s = 0;
                if (ota_state == OTA_STATE_IDLE) {
                    ota_state = OTA_STATE_TIMEUP;
                } else if (ota_state >= OTA_STATE_CHECKING) {
                    if (ota_timeout > 0) {
                        if (--ota_timeout == 0) {
                            ota_state = OTA_STATE_TIMEOUT;
                        }
                    }
                }
            }
        }

        ota_run();
    }
}

void isp_handle(void) {
    uint16_t addr = rx.pkt.addr;
    tx.pkt.status = LDR_STATUS_OK;
    tx.pkt.size = 0;

    switch (rx.pkt.cmd) {
        case ISP_CMD_CONNECT:
            tx.pkt.size = 2;
            tx.pkt.dat[0] = APP_VERSION_MAJOR;
            tx.pkt.dat[1] = APP_VERSION_MINOR;
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
        case ISP_CMD_READ_APP_VERSION:
            tx.pkt.status = LDR_STATUS_APP_VERSION;
            tx.pkt.size = 8;
            *(uint32_t *)&tx.pkt.dat[0] = APP_VERSION;
            *(uint32_t *)&tx.pkt.dat[4] = APP_BUILD_TIME;
            break;
        case OTA2APP_CMD_LATEST_APP_INFO:
            if (rx.pkt.size == sizeof(latest_app_info_t) && ota_state == OTA_STATE_CHECKING) {
                latest_app_info_t *info = (latest_app_info_t *)rx.pkt.dat;
                app_info_to_big_endian(info->info);
                debugf4("Latest App info: result=%bu, size=0x%08lX, crc=0x%08lX",
                        info->result, info->info.size, info->info.crc);
                debugf4("Version: %bu.%bu.%u",
                        version_major(info->info.version),
                        version_minor(info->info.version),
                        version_patch(info->info.version));
                if (info->result == OTA_OK) {
                    if (info->info.version > papp->version) {
                        ota_state = OTA_STATE_PREPARING;
                    } else {
                        debugf1("No newer app");
                    }
                }
                return;  // no need to reply
            }
            break;
        default:
            tx.pkt.status = LDR_STATUS_UNKNOWN_CMD;
            break;
    }
    uart1_send_tx();
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
            break;
        case OTA_STATE_PREPARING:
            break;
        case OTA_STATE_CHECKING:
            break;
        default:
            ota_state = OTA_STATE_IDLE;
            break;
    }
}
