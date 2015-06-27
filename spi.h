#ifndef _SPI_H
#define _SPI_H


#include <avr/io.h>


void spiwrite (uint8_t data);
void spi_init(void);
void spi_off(void);
void spi_on(void);

#endif
