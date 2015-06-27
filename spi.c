//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+Computer science 2015 Project

//+Digital Spirit Level

//+Library for the SPI communication

//+

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




//Created: 17/06/2015 08:09:22 a. m.


//This library is intended to handle the SPI communication handling.
//Revision history:
//Version 0: First development
//Version 1: Added functions 26/06/2015 10:46 p. m.

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
	SPCR |= (1<<SPE) | (1<<MSTR);//SPI master mode SS' not affecting, SPI communication enabled, fosc/4
}

void spi_off(void)
{
	SPCR &= ~(1<<SPE);
}

void spi_on(void)
{
	SPCR |= (1<<SPE);
}
