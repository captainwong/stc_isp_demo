#ifndef __SPI_H__
#define __SPI_H__

#include <sys/sys.h>

#define spi_select() \
    do {             \
        EA = 0;      \
        SPI_NSS = 0; \
    } while (0)

#define spi_unselect() \
    do {               \
        SPI_NSS = 1;   \
        EA = 1;        \
    } while (0)

void spi_init(void);
void spi_send(uint8_t dat);
uint8_t spi_recv(void);

#endif /* __SPI_H__ */
