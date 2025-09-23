#include <bsp/norflash.h>
#include <sys/build_time.h>
#include <sys/gpio.h>
#include <sys/sys.h>
#include <sys/version.h>

#include "ota.h"
#include "timer.h"
#include "uart.h"

system_context_t xdata sysctx __at(SYSTEM_CONTEXT_ADDR);

void isp_handle(void);

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
    ota_init();

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

            ota_1s_event();
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
            if (rx.pkt.size == sizeof(latest_app_info_t)) {
                latest_app_info_t *info = (latest_app_info_t *)rx.pkt.dat;
                app_info_to_big_endian(info->info);
                ota_on_latest_app_info(info);
            }
            return;  // no need to reply
        case OTA2APP_CMD_APP_DATA:
            if (rx.pkt.size >= sizeof(get_app_data_res_t)) {
                get_app_data_res_t *res = (get_app_data_res_t *)rx.pkt.dat;
                res->offset = rev32(res->offset);
                res->size = rev32(res->size);
                res->crc = rev32(res->crc);
                ota_on_app_data(res);
            }
            return;  // no need to reply
        default:
            tx.pkt.status = LDR_STATUS_UNKNOWN_CMD;
            break;
    }
    uart1_send_tx();
}
