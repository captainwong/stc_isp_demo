#include "stc8h.h"

uint8_t iap_read_byte(uint16_t addr) {
    uint8_t data b;
    iap_enable();
    iap_cmd_read();
    iap_set_addr(addr);
    iap_trigger();
    b = IAP_DATA;
    iap_idle();
    return b;
}

void iap_read_bytes(uint16_t addr, uint8_t* buf, uint16_t len) {
    iap_enable();
    iap_cmd_read();

    while (len--) {
        iap_set_addr(addr);
        iap_trigger();
        *buf++ = IAP_DATA;  // read byte
        addr++;
    }

    iap_idle();
}

void iap_write_byte(uint16_t addr, uint8_t b) {
    iap_enable();
    iap_cmd_write();
    iap_set_addr(addr);
    IAP_DATA = b;  // write byte
    iap_trigger();
    iap_idle();
}

bool iap_write_byte_check(uint16_t addr, uint8_t b) {
    iap_write_byte(addr, b);
    return !iap_is_fail() && iap_read_byte(addr) == b;  // check if write was successful
}

void iap_write_bytes(uint16_t addr, uint8_t* buf, uint16_t len) {
    iap_enable();
    iap_cmd_write();
    while (len--) {
        iap_set_addr(addr);
        IAP_DATA = *buf++;  // write byte
        iap_trigger();
        addr++;
    }

    iap_idle();
}

bool iap_write_bytes_check(uint16_t addr, uint8_t* buf, uint16_t len) {
    while (len--) {
        if (!iap_write_byte_check(addr++, *buf++)) {
            return false;
        }
    }
    return true;
}

void iap_erase_page(uint16_t addr) {
    iap_enable();
    iap_cmd_erase();
    iap_set_addr(addr);
    iap_trigger();
    iap_idle();
}

bool iap_erase_page_check(uint16_t addr) {
    iap_erase_page(addr);
    return !iap_is_fail();
}
