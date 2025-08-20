#include "uart.h"

bit bUartRxReady;

uint8_t UartrecvIndex;
uint8_t UartRecvStep;
uint8_t UartRecvSum;

uint8_t xdata UartTxBuffer[256];
uint8_t xdata UartRxBuffer[256];

void uart_init() {
    t1_1t();
    t1_as_timer();
    t1_load(uart_calc_load_1t(MAIN_Fosc, UART1_BAUD));
    t1_disable_irq();
    t1_run();

    uart1_use_p30_p31();
    pin_mode_io_pup(3, 0);
    pin_mode_io_pup(3, 1);
    uart1_mode1_8bit_brtx();
    uart1_brt_use_timer1();
    uart1_enable_recv();
    TI = 1;

    uart_recv_done();
}

void uart_isr() {
    uint8_t dat;

    if (RI) {
        RI = 0;

        UartRecvSum += (dat = SBUF);
        switch (UartRecvStep) {
            case 0:
L_CheckHead:
                UartRecvStep = ((UartRecvSum = dat) == '#');
                break;
            case 1:
                UartRxBuffer[0] = dat;
                UartrecvIndex = 0;
                UartRecvStep++;
                break;
            case 2:
                UartRxBuffer[1 + UartrecvIndex++] = dat;
                if (UartrecvIndex >= UartRxBuffer[0])
                    UartRecvStep++;
                break;
            case 3:
                if (dat != '$') goto L_CheckHead;
                UartRecvStep++;
                break;
            case 4:
                if (UartRecvSum != 0) goto L_CheckHead;
                bUartRxReady = 1;
                UartRecvStep++;
                break;
            default:
                break;
        }
    }
}

static uint8_t send(uint8_t dat) {
    while (!TI)
        ;
    TI = 0;
    SBUF = dat;

    return dat;
}

void uart_send(uint8_t status, uint8_t size) {
    uint8_t sum;
    uint8_t i;

    sum = send('@');
    sum += send(status);
    sum += send(size);
    if (size) {
        for (i = 0; i < size; i++) {
            sum += send(UartTxBuffer[i]);
        }
    }
    sum += send('$');
    send(-sum);

    while (!TI)
        ;
}

void uart_recv_done() {
    bUartRxReady = 0;
    UartrecvIndex = 0;
    UartRecvStep = 0;
    UartRecvSum = 0;
}
