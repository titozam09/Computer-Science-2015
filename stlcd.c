


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+Computer science 2015 Project

//+Digital Spirit Level

//+Library for the graphical display CoG with controller ST7565R

//+Modified from the Adafruit Library in order to suit our needs.

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




//Created: 20/06/2015 08:09:22 a. m.


//This library is intended to handle the graphical functions of the CoG (white on black) with ST7565 controller.
//Revision history:
//Version 0: First development
//Version 1: Added functions 26/06/2015 10:46 p. m.

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "stlcd.h"
#include "spi.h"
#include "glcd.h"

#define F_CPU 1000000UL

#define SID_DDR DDRB
#define SID_PIN PINB
#define SID_PORT PORTB
#define SID 5

#define SCLK_DDR DDRB
#define SCLK_PIN PINB
#define SCLK_PORT PORTB
#define SCLK 7

#define A0_DDR DDRD
#define A0_PIN PIND
#define A0_PORT PORTD
#define A0 7

#define RST_DDR DDRD
#define RST_PIN PIND
#define RST_PORT PORTD
#define RST 6

#define CS_DDR DDRB
#define CS_PIN PINB
#define CS_PORT PORTB
#define CS 4

//Send in page 7 contents of page 0, due to "mirroring" effects
int pagemap[] = { 7,6,5,4,3,2,1,0 };

void st7565_init(void)
{
	
	//init SPI communication
	spi_init();
	// set pin directions
	A0_DDR |= (1 << A0);
	RST_DDR |= (1 << RST);
	// toggle RST low to reset; CS low so it'll listen to us
	CS_PORT &= ~(1 << CS);
	RST_PORT &= ~(1 << RST);
	_delay_ms(500);
	RST_PORT |= (1 << RST);
	// LCD bias select
	st7565_command(CMD_SET_BIAS_7);
	// ADC select= setting segments backwards
	st7565_command(CMD_SET_ADC_REVERSE);
	// SHL select = setting columns backwards
	st7565_command(CMD_SET_COM_REVERSE);
	// Initial display line
	st7565_command(CMD_SET_DISP_START_LINE);
	// turn on voltage converter (VC=1, VR=0, VF=0)
	st7565_command(CMD_SET_POWER_CONTROL | 0x4);
	// wait for 50% rising
	_delay_ms(50);
	// turn on voltage regulator (VC=1, VR=1, VF=0)
	st7565_command(CMD_SET_POWER_CONTROL | 0x6);
	// wait more or equal than 50ms
	_delay_ms(50);
	// turn on voltage follower (VC=1, VR=1, VF=1)
	st7565_command(CMD_SET_POWER_CONTROL | 0x7);
	// wait
	_delay_ms(10);
	// set lcd operating voltage (regulator resistor, ref voltage resistor)
	st7565_command(CMD_SET_RESISTOR_RATIO | 0x6);
	//set brightness
	st7565_set_brightness(0x0A);
	//clear screen
	clear_screen();
	//Start visualization of Display
	st7565_command(CMD_DISPLAY_ON);
	st7565_command(CMD_SET_ALLPTS_NORMAL);
	st7565_command(CMD_SET_DISP_NORMAL);

}

//send 0x00 for every part of the screen
void clear_screen(void)
{
	uint8_t page, column;

	for(page = 0; page < 8; page++)
	{
		st7565_command(CMD_SET_PAGE | page);
		
		for(column = 0; column < 128; column++)
		{
			st7565_command(CMD_SET_COLUMN_LOWER | (column & 0x0f));
			st7565_command(CMD_SET_COLUMN_UPPER | ((column >> 4) & 0x0f));
			st7565_data(0x00);

		}

	}

}

//clear A0 for making a command instruction data
void st7565_command(uint8_t info)
{
	A0_PORT &= ~(1 << A0);
	spiwrite(info);
}


//set A0 for making a data instruction data

void st7565_data(uint8_t info)

{
	A0_PORT |= (1 << A0);
	spiwrite(info);
}



//setting "brigthness" = contrast of the screen
void st7565_set_brightness(uint8_t val)

{

	st7565_command(CMD_SET_VOLUME_FIRST);
	st7565_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));

}

//send data stored in the buffer
void write_buffer(uint8_t *buffer)
{

	uint8_t column, page;

	for(page = 0; page < 8; page++)
	{
		//Send in page 7 contents of page 0, due to "mirroring" effects
		st7565_command(CMD_SET_PAGE | pagemap[page]);
		st7565_command(CMD_SET_COLUMN_LOWER | (0x00 & 0xf));
		st7565_command(CMD_SET_COLUMN_UPPER | ((0x00 >> 4) & 0xf));
		for(column = 0; column < 128; column++)
		{
			st7565_data(buffer[(128*page)+column]);
		}
	}
}



uint8_t const ascii_tab_small[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, //space
	0x00, 0x00, 0xFA, 0x00, 0x00, //!
	0x00, 0xE0, 0x00, 0xE0, 0x00, //"
	0x28, 0xFE, 0x28, 0xFE, 0x28, //#
	0x24, 0x54, 0xFE, 0x54, 0x48, //$
	0xC4, 0xC8, 0x10, 0x26, 0x46, //%
	0x6C, 0x92, 0x6A, 0x04, 0x0A, //&
	0x00, 0x10, 0xE0, 0xC0, 0x00, //'
	0x00, 0x38, 0x44, 0x82, 0x00, //(
	0x00, 0x82, 0x44, 0x38, 0x00, //)
	0x54, 0x38, 0xFE, 0x38, 0x54, //*
	0x10, 0x10, 0x7C, 0x10, 0x10, //+
	0x00, 0x01, 0x0E, 0x0C, 0x00, //,
	0x10, 0x10, 0x10, 0x10, 0x10, //-
	0x00, 0x00, 0x06, 0x06, 0x00, //.
	0x04, 0x08, 0x10, 0x20, 0x40, ///
	0x7C, 0x8A, 0x92, 0xA2, 0x7C, //0
	0x00, 0x42, 0xFE, 0x02, 0x00, //1
	0x4E, 0x92, 0x92, 0x92, 0x62, //2
	0x84, 0x82, 0x92, 0xB2, 0xCC, //3
	0x18, 0x28, 0x48, 0xFE, 0x08, //4
	0xE4, 0xA2, 0xA2, 0xA2, 0x9C, //5
	0x3C, 0x52, 0x92, 0x92, 0x8C, //6
	0x82, 0x84, 0x88, 0x90, 0xE0, //7
	0x6C, 0x92, 0x92, 0x92, 0x6C, //8
	0x62, 0x92, 0x92, 0x94, 0x78, //9
	0x00, 0x00, 0x28, 0x00, 0x00, //:
	0x00, 0x02, 0x2C, 0x00, 0x00, //;
	0x00, 0x10, 0x28, 0x44, 0x82, //<
	0x28, 0x28, 0x28, 0x28, 0x28, //=
	0x00, 0x82, 0x44, 0x28, 0x10, //>
	0x40, 0x80, 0x9A, 0x90, 0x60, //?
	0x7C, 0x82, 0xBA, 0x9A, 0x72, //@
	0x3E, 0x48, 0x88, 0x48, 0x3E, //A
	0xFE, 0x92, 0x92, 0x92, 0x6C, //B
	0x7C, 0x82, 0x82, 0x82, 0x44, //C
	0xFE, 0x82, 0x82, 0x82, 0x7C, //D
	0xFE, 0x92, 0x92, 0x92, 0x82, //E
	0xFE, 0x90, 0x90, 0x90, 0x80, //F
	0x7C, 0x82, 0x82, 0x8A, 0xCE, //G
	0xFE, 0x10, 0x10, 0x10, 0xFE, //H
	0x00, 0x82, 0xFE, 0x82, 0x00, //I
	0x04, 0x02, 0x82, 0xFC, 0x80, //J
	0xFE, 0x10, 0x28, 0x44, 0x82, //K
	0xFE, 0x02, 0x02, 0x02, 0x02, //L
	0xFE, 0x40, 0x38, 0x40, 0xFE, //M
	0xFE, 0x20, 0x10, 0x08, 0xFE, //N
	0x7C, 0x82, 0x82, 0x82, 0x7C, //O
	0xFE, 0x90, 0x90, 0x90, 0x60, //P
	0x7C, 0x82, 0x8A, 0x84, 0x7A, //Q
	0xFE, 0x90, 0x98, 0x94, 0x62, //R
	0x64, 0x92, 0x92, 0x92, 0x4C, //S
	0x80, 0x80, 0xFE, 0x80, 0x80, //T
	0xFC, 0x02, 0x02, 0x02, 0xFC, //U
	0xF8, 0x04, 0x02, 0x04, 0xF8, //V
	0xFC, 0x02, 0x1C, 0x02, 0xFC, //W
	0xC6, 0x28, 0x10, 0x28, 0xC6, //X
	0xC0, 0x20, 0x1E, 0x20, 0xC0, //Y
	0x86, 0x9A, 0x92, 0xB2, 0xC2, //Z
	0x00, 0xFE, 0x82, 0x82, 0x82, //[
	0x40, 0x20, 0x10, 0x08, 0x04, /* \ */
	0x00, 0x82, 0x82, 0x82, 0xFE, //]
	0x20, 0x38, 0x3E, 0x38, 0x20, //0x20, 0x40, 0x80, 0x40, 0x20, //^ modified to get degree symbol
	0xE0, 0xA0, 0xE0, 0x00, 0x00  //0x02, 0x02, 0x02, 0x02, 0x02, //_ modified to get "arrow"
};


//put in buffer one character 5x7 
void drawchar(uint8_t *buff, uint8_t x, uint8_t line, uint8_t c)
{
	for (uint8_t i =0; i<5; i++ )
	{
		buff[x + (line*128) ] = ascii_tab_small[(c-32)*5+i];
		x++;
	}
}

//put in buffer one set of characters
void drawstring(uint8_t *buff, uint8_t x, uint8_t line, uint8_t text[])
{
	uint8_t i=0;
	while (text[i] != 0)
	{
		drawchar(buff, x, line, text[i]);
		i++;
		x += 6; // 6 pixels wide
		if (x + 6 >= LCDWIDTH)
		{
			x = 0;    // ran out of this line
			line++;
		}
		if (line >= (LCDHEIGHT/8))
		return;        // ran out of space
	}
}

// clear everything
void clear_buffer(uint8_t *buff)
{
	memset(buff, 0, 1024);
}


//write just one page
void write_page(uint8_t *buffer, uint8_t page)
{
	uint8_t column;
	
	st7565_command(CMD_SET_PAGE | pagemap[page]);
	
	for(column = 0; column < 128; column++)
	{
		st7565_command(CMD_SET_COLUMN_LOWER | (column & 0x0f));
		st7565_command(CMD_SET_COLUMN_UPPER | ((column >> 4) & 0x0f));
		st7565_data(buffer[(128*page)+column]);
	}
	

}

//erase just one page
void clear_page(uint8_t *buffer, uint8_t page)
{
	uint8_t column;
	
	st7565_command(CMD_SET_PAGE | pagemap[page]);
	
	for(column = 0; column < 128; column++)
	{
		buffer[(128*page)+column]=0;
		st7565_command(CMD_SET_COLUMN_LOWER | (column & 0x0f));
		st7565_command(CMD_SET_COLUMN_UPPER | ((column >> 4) & 0x0f));
		st7565_data(buffer[(128*page)+column]);
	}
	

}




