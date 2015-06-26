#include "spi.h"



void spiwrite (uint8_t data)
{
	SPDR = data;
	//wait for data to be sent
	while((SPSR & (1<<SPIF)) == 0);
}



void spi_init(void)
{
	DDRB |= ((1<<4) |(1<<5) | (1<<7)); // SS, MOSI, SCLK - output
//	DDRB &= ~(1<<6); // MISO - input

	SPCR = (1<<SPE) | (1<<MSTR);//SPI master mode SS' not affecting, SPI communication enabled, fosc/4
}
