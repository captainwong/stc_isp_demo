#include "dfu.h"

#include "uart.h"

uint32_t xdata dfuflag __at(0x1FFC);

void dfu_check() {
    if ((dfuflag != DFU_TAG) &&
        (*(uint8_t code *)(LDR_SIZE) == 0x02) &&
        (*(uint16_t code *)(LDR_SIZE + 1) >= LDR_SIZE + 3)) {
        ((void(code *)())(LDR_SIZE))();
    }

    dfuflag = 0;
}

void dfu_events() {
    uint8_t cmd;
    uint16_t addr;
    uint8_t size;
    uint8_t ret;
    uint8_t *ptr;
    uint8_t status;

    if (!bUartRxReady)
        return;

    cmd = UartRxBuffer[1];
    addr = *(uint16_t *)&UartRxBuffer[4];
    size = UartRxBuffer[6];
    ptr = &UartRxBuffer[7];
    status = STATUS_OK;
    ret = 0;

    switch (cmd) {
        case DFU_CMD_CONNECT:
            UartTxBuffer[0] = LDR_VERSION >> 8;
            UartTxBuffer[1] = LDR_VERSION;
            ret = 2;
            break;
        case DFU_CMD_READ:
            status = STATUS_ERRORCMD;
            break;
        case DFU_CMD_PROGRAM:
            while (size--) {
                if (!iap_write_byte_check(addr, *ptr)) {
                    status = STATUS_PROGRAMERR;
                    break;
                }
                addr++;
                ptr++;
            }
            break;
        case DFU_CMD_ERASE:
            addr = 0;
            while (addr < 0xf000) {
                iap_erase_page(addr);
                addr += 0x200;
            }
            break;
        case DFU_CMD_REBOOT:
            IAP_CONTR = 0x20;
            while (1);
            break;
        default:
            status = STATUS_ERRORCMD;
            break;
    }

    uart_send(status, ret);
    uart_recv_done();
}
