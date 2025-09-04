#include <sys/sys.h>

void delay_ms(uint16_t ms) {
    volatile uint16_t data i;
    while (ms--) {
        i = MAIN_Fosc / 10000;
        while (i--);
    }
}
