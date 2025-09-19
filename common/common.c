#include "common.h"

#include <bsp/norflash.h>

void delay_ms(uint16_t ms) {
    volatile uint16_t data i;
    while (ms--) {
        i = MAIN_Fosc / 10000;
        while (i--);
    }
}

void findout_which_ota_info_is_older(void) {
    uint32_t seq_master, seq_backup;
    norflash_read(NORFLASH_OTA_MASTER_ADDR, (uint8_t *)&seq_master, sizeof(seq_master));
    norflash_read(NORFLASH_OTA_BACKUP_ADDR, (uint8_t *)&seq_backup, sizeof(seq_backup));
    if (seq_master == 0xFFFFFFFF && seq_backup == 0xFFFFFFFF) {
        sysctx.st.otaid = FLASH_OTA_ID_MASTER;  // both invalid, set master is older
    } else if (seq_master == 0xFFFFFFFF) {
        sysctx.st.otaid = FLASH_OTA_ID_MASTER;  // master invalid, master is older
    } else if (seq_master == 0xFFFFFFFF) {
        sysctx.st.otaid = FLASH_OTA_ID_BACKUP;  // backup invalid, backup is older
    } else {
        // both valid, which seq is lower which is the older
        if (seq_master <= seq_backup) {
            sysctx.st.otaid = FLASH_OTA_ID_MASTER;  // master is older
        } else {
            sysctx.st.otaid = FLASH_OTA_ID_BACKUP;  // backup is older
        }
    }
}
