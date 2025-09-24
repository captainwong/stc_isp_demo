#include <bsp/norflash.h>
#include <libhbcheck/hb_check.h>
#include <sys/build_time.h>
#include <sys/gpio.h>
#include <sys/sys.h>
#include <sys/version.h>

#include "ota.h"
#include "timer.h"
#include "uart.h"

system_context_t xdata sysctx __at(SYSTEM_CONTEXT_ADDR);
uint16_t cycles;
uint16_t rx_time, rx_timeout;

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
    while (!norflash_init()) {
        debugf1("Norflash init failed");
        led_run_toggle();
        delay_ms(1000);
    }
    debugf3("Norflash init ok, type=%04X, %s", norflash_type, norflash_get_type_string());
    ota_init();
    cycles = 0;

    while (1) {
        cycles++;

        uart1_run();
        if (isp_parse_ok) {
            isp_parse_ok = false;
            rx_time = rx_timeout = cycles;
            isp_handle();
        }
        if (++rx_timeout == rx_time) {  // 转一圈还没有新的数据，说明这一包超时
            isp_parse_init(ctx);
        }

        if (key_boot_pressed()) {
            sysctx.st.dfu = 1;
            sys_reset();
        }

        if (flag_1s) {
            flag_1s = false;
            led_run_toggle();

            ota_1s_event();
            uart1_send_sysctx();
        }

        ota_run();
    }
}

void isp_handle(void) {
    uint32_t addr = rx.pkt.addr;
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
            addr = (*(uint32_t *)&rx.pkt.dat[0]);
            addr = rev32(addr);
            norflash_erase_sector(addr);
            *(uint32_t *)&tx.pkt.dat[0] = *(uint32_t *)&rx.pkt.dat[0];  // echo back addr
            tx.pkt.size = 4;
            tx.pkt.status = LDR_STATUS_W25Q_ERASE_SECTOR_RES;
            break;
        case ISP_CMD_READ_W25Q:
            addr = (*(uint32_t *)&rx.pkt.dat[0]);
            addr = rev32(addr);
            if (rx.pkt.size > PKT_DAT_MAX_LEN) {
                rx.pkt.size = PKT_DAT_MAX_LEN;  // limit max read size
            }
            *(uint32_t *)&tx.pkt.dat[0] = *(uint32_t *)&rx.pkt.dat[0];  // echo back addr
            norflash_read(addr, &tx.pkt.dat[4], rx.pkt.size);
            tx.pkt.size = rx.pkt.size + 4;
            tx.pkt.status = LDR_STATUS_W25Q_DATA;
            break;
        case ISP_CMD_PROGRAM_W25Q:
            addr = (*(uint32_t *)&rx.pkt.dat[0]);
            addr = rev32(addr);
            if (rx.pkt.size > PKT_DAT_MAX_LEN) {
                rx.pkt.size = PKT_DAT_MAX_LEN;  // limit max program size
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
