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
//Version 1: Deleted some functions not needed
//Version 2: Deleted functions not needed 28/06/2015 07:47 p. m.
 
 
 
#include <avr/io.h>
#include <stdlib.h>
#include "glcd.h"
 
// the most basic function, set a single pixel
void setpixel(uint8_t *buff, uint8_t x, uint8_t y, uint8_t color) 
{
    if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return;
    // x is which column ,color means 1 for white,0 for black
    if (color)
        buff[x+ (y/8)*128] |= (1<<(7-(y%8)));
    else
        buff[x+ (y/8)*128] &= ~(1<<(7-(y%8)));
}
 
 
 
// drawing a line using bresenham's algorithm
void drawline(uint8_t *buff, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color)
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
        ystep = 1;
    else
        ystep = -1;
 
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
