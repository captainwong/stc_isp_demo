#include <bsp/norflash.h>
#include <sys/build_time.h>
#include <sys/gpio.h>
#include <sys/sys.h>
#include <sys/version.h>

#include "uart.h"

system_context_t xdata sysctx __at(SYSTEM_CONTEXT_ADDR);
uint16_t counter_1ms = 0;
uint16_t counter_1s = 0;
bool should_check_ota = false;

void isp_handle(void);

void t0_isr() INTERRUPT(TMR0_VECTOR) {
    if (++counter_1ms == 1000) {
        counter_1ms = 0;
        led_run_toggle();

        if (++counter_1s == 5) {  // 暂时每5秒检测一次有没有OTA更新
            counter_1s = 0;
            should_check_ota = true;
        }
    }
}

void main() {
    enable_xsfr();
    gpio_init();
    pin_pu(3, 2);  // KEY_BOOT 上拉

    t0_mode0_16bit_auto_reload();
    t0_1t();
    t0_as_timer();
    t0_load(T1MS);
    t0_enable_irq();
    t0_run();

    uart_init();
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

    {
        app_info_t *papp = NULL;
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

    while (1) {
        uart_run();
        if (isp_parse_ok) {
            isp_parse_ok = false;
            isp_handle();
        }

        if (key_boot_pressed()) {
            sysctx.st.dfu = 1;
            sys_reset();
        }

        if (should_check_ota) {
            should_check_ota = false;
            uart_send_check_ota();
        }
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
            if (rx.pkt.size == sizeof(latest_app_info_t)) {
                latest_app_info_t *info = (latest_app_info_t *)rx.pkt.dat;
                app_info_to_big_endian(info->info);
                debugf4("Latest App info: status=%bu, size=0x%08lX, crc=0x%08lX", info->status, info->info.size, info->info.crc);
                debugf4("Version: %bu.%bu.%u",
                        version_major(info->info.version),
                        version_minor(info->info.version),
                        version_patch(info->info.version));
                if (info->status == OTA_OK) {
                    if (info->info.size > 0 && info->info.size <= APP_MAX_SIZE) {
                        // flash_app_info_t *papp = NULL;
                        // if (ota_info.current_app == FLASH_APP_ID_FACTORY) {
                        //     papp = &ota_info.factory;
                        // } else if (ota_info.current_app == FLASH_APP_ID_APP1) {
                        //     papp = &ota_info.app1;
                        // } else {  // ota_info.current_app == FLASH_APP_ID_APP2
                        //     papp = &ota_info.app2;
                        // }
                        // if(info->info.version > papp->info.version) {
                        //     debugf1("Newer app found, start to download");
                        //     ota_info.dlctx.offset = 0;
                        //     ota_info.dlctx.size = info->info.size;
                        //     ota_info.dlctx.crc = info->info.crc;
                        //     ota_info.dlctx.version = info->info.version;
                        //     ota_info.dlctx.state = FLASH_APP_DL_STATE_DOWNLOADING;
                        //     // reply latest app info
                        //     reply_latest_ota_app_info(*info);
                        // } else {
                        //     debugf1("No newer app");
                        // }
                    } else {
                        debugf1("Invalid app size");
                    }
                }
                return;  // no need to reply
            }
            break;
        default:
            tx.pkt.status = LDR_STATUS_UNKNOWN_CMD;
            break;
    }
    uart_send_tx();
}
