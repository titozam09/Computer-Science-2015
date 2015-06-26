/*
$Id:$

ST7565 LCD library!

Copyright (C) 2010 Limor Fried, Adafruit Industries


This library is free software; you can redistribute it and/or 
modify it under
the terms of the GNU Lesser General Public
License as published by the Free Software
Foundation; either
version 2.1 of the License, or (at your option) any later version.


This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY;
without even the implied warranty of 
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU
Lesser General Public License for more details.

You should have received a copy
of the GNU Lesser General Public
License along with this library; if not, write to the Free
Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+Computer science 2015 Project

//+Digital Spirit Level

//+Library for the graphical display CoG with controller ST7565R

//+Modified from the Adafruit Library in order to suit our needs.

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




//Created: 19/06/2015 10:33:15 a. m.


//This library is intended to handle the graphical functions of the CoG (white on black) with ST7565 controller.
//Revision history:
//Version 0: First development
//



#include <avr/io.h>

#include <stdlib.h>

#include <avr/pgmspace.h>

#include "glcd.h"





// the most basic function, set a single pixel

void setpixel(uint8_t *buff, uint8_t x, uint8_t y, uint8_t color) 
{
	
	if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))

	return;

	
	// x is which column
,color means 1 for white,0 for black
	if (color)

		buff[x+ (y/8)*128] |= (1<<(7-(y%8)));

	else
	
		buff[x+ (y/8)*128] &= ~(1<<((7-(y%8)));

}




// the most basic function, clear a single pixel

void clearpixel(uint8_t *buff, uint8_t x, uint8_t y)
{
	
	// x is which column
	
	buff[x+ (y/8)*128] &= ~(1<<(7-(y%8)));

}



// bresenham's algorithm
drawline(uint8_t *buff, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
{

	
	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);


	if (steep)
	{

		swap(x0, y0);

		swap(x1, y1);

	}


	
	if (x0 > x1)
	{

		swap(x0, x1);

		swap(y0, y1);
	}


	uint8_t dx, dy;

	dx = x1 - x0;

	dy = abs(y1 - y0);
	int8_t err = dx / 2;

	int8_t ystep;



	if (y0 < y1)
	{

		ystep = 1;

	} 

	else 
	{

		ystep = -1;

	}



	for (; x0<x1; x0++)
	{

		if (steep)
		{

			setpixel(buff, y0, x0, color);

		}
		else
		{

			setpixel(buff, x0, y0, color);

		}

		err -= dy;

		if (err < 0)
		{

			y0 += ystep;

			err += dx;

		}

	}

}



// draw a rectangle

void drawrect(uint8_t *buff, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{

	
	for (uint8_t i=x; i<x+w; i++)
	{

		setpixel(buff, i, y, color);

		setpixel(buff, i, y+h-1, color);

	}


	for (uint8_t i=y; i<y+h; i++)
	{

		setpixel(buff, x, i, color);

		setpixel(buff, x+w-1, i, color);

	}

}




// draw a circle

void drawcircle(uint8_t *buff, uint8_t x0, uint8_t y0, uint8_t r, uint8_t color)
{
	int8_t f = 1 - r;

	int8_t ddF_x = 1;

	int8_t ddF_y = -2 * r;

	int8_t x = 0;

	int8_t y = r;


	setpixel(buff, x0, y0+r, color);
	setpixel(buff, x0, y0-r, color);

	setpixel(buff, x0+r, y0, color);

	setpixel(buff, x0-r, y0, color);


	while (x<y)
	{

		if (f >= 0)
		{

			y--;

			ddF_y += 2;

			f += ddF_y;

		}


		x++;

		ddF_x += 2;

		f += ddF_x;
		setpixel(buff, x0 + x, y0 + y, color);

		setpixel(buff, x0 - x, y0 + y, color);

		setpixel(buff, x0 + x, y0 - y, color);

		setpixel(buff, x0 - x, y0 - y, color);

		setpixel(buff, x0 + y, y0 + x, color);

		setpixel(buff, x0 - y, y0 + x, color);

		setpixel(buff, x0 + y, y0 - x, color);

		setpixel(buff, x0 - y, y0 - x, color);

	}

}



// clear everything

void clear_buffer(uint8_t *buff)
{

	memset(buff, 0, 1024);

}
