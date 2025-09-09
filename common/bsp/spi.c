#include <bsp/spi.h>

void spi_init(void) {
    SPI_NSS = 1;
    SPI_SCK = 0;
    SPI_MOSI = 1;
}

void spi_send(uint8_t dat) {
    uint8_t i;
    for (i = 0; i < 8; i++) {
        dat <<= 1;
        SPI_MOSI = CY;
        SPI_SCK = 1;
        SPI_SCK = 0;
    }
    SPI_MOSI = 1;
}

uint8_t spi_recv(void) {
    uint8_t dat = 0, i;
    for (i = 0; i < 8; i++) {
        dat <<= 1;
        if (SPI_MISO) {
            dat |= 0x01;
        }
        SPI_SCK = 1;
        SPI_SCK = 0;
    }
    return dat;
}
