
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+Computer science 2015 Project

//+Digital Spirit Level

//+Main program for the Computer Science project

//+

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++




//Created: 27/06/2015 07:33 p. m.


//This program is intended to handle the main function of the digital spirit level.
//Revision history:
//Version 0: First development
//

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "stlcd.h"
#include "glcd.h"
#include "spi.h"

volatile uint8_t screenmess=0x01;
uint8_t print=0;

int main(void)
{
	uint8_t buffer[1024]={0};
	
	DDRB &= ~(1<<2); //setting PB2 as input for interrupt INT 2
	DDRD &= ~(1<<2) & (1<<3); //setting PD2 and PD3 as input for interrupt INT0, INT1
	PORTB |= (1<<2);
	PORTD |= (1<<2) | (1<<3); //enabling pull-up resistor in corresponding pins
	MCUCR &= ~(1<<ISC10) & ~(1<<ISC11); //setting for INT1 a low value interrupt
	MCUCR &= ~(1<<ISC00) & ~(1<<ISC01);//setting for  INT0 a low value interrupt
	MCUCSR &= ~(1<< ISC2); ////setting for INT2 as a falling edge interrupt
	GICR |= (1<<INT0) | (1<<INT1) | (1<<INT2); //Enabling interrupt
	sei();
	st7565_init();
	DDRC = 0xFF; //purposes of testing
	PORTC = 0x00;
	while(1)
	{
		if ((screenmess==1) && (print==0))
		{
			//measuremode message
			print=1;
			clear_page(buffer,4);
			drawstring(buffer,15,4,"MEASUREMENT MODE");
			write_page(buffer,4);	
		}
		
		if ((screenmess==2) && (print==0))
		{
			//setanglemode message
			print=1;
			clear_page(buffer,4);
			drawstring(buffer,25,4,"SET ANGLE MODE");
			write_page(buffer,4);
		}
		
		if ((screenmess==4) && (print==0))
		{
			//setreference message
			print=1;
			clear_page(buffer,4);
			drawstring(buffer,8,4,"SET REFERENCE MODE");
			write_page(buffer,4);
						
		}
		
		if ((screenmess==8) && (print==0))
		{
			//Start displaying the angle measured
			print=1;
			clear_page(buffer,4);
			for (uint8_t i=26;i<=116;i+=5)
			drawline(buffer,i,41,i,44,1);
			drawline(buffer,26,47,26,44,1);
			drawline(buffer,48,47,48,44,1);
			drawline(buffer,49,47,49,44,1);
			drawline(buffer,71,47,71,44,1);
			drawline(buffer,93,47,93,44,1);
			drawline(buffer,94,47,94,44,1);
			drawline(buffer,116,47,116,44,1);
			drawstring(buffer,15,6,"-90 -45  0  45  90");
			drawstring(buffer,1,1,"NUMERIC ANGLE VALUES");
			write_page(buffer,6);
			write_page(buffer,1);
			write_page(buffer,5);
			
			
			while (screenmess==8)
			{
				for (int8_t i=-90;i<=90;i++)
				{
					clear_page(buffer,4);
					drawchar(buffer,69+i/2,4,94);
					write_page(buffer,4);
					_delay_ms(250);
				}
				//ACCELEROMETER FUNCTION
			}
			for(uint8_t i=1;i<7;i++)
			clear_page(buffer,i);
			
		}
		
		if ((screenmess==16) && (print==0))
		{
			//Ask for the desired angle
			print=1;
			clear_page(buffer,4);
			drawstring(buffer,1,3,"SELECT DESIRED ANGLE");
			write_page(buffer,3);
			while (screenmess==16)
			{
				//function setting angle
			}
			clear_page(buffer,3);
			
		}
		
		if ((screenmess==128) && (print==0))
		{
			//Message
			print=1;
			clear_page(buffer,4);
			drawstring(buffer,1,2,"1. ALIGN PRODUCT WITH   REFERENCE SURFACE");
			drawstring(buffer,1,4,"2. PRESS BUTTON 2 FOR   CALIBRATION");
			write_page(buffer,2);
			write_page(buffer,3);
			write_page(buffer,4);
			write_page(buffer,5);
			while (screenmess==128)
			{
				//wait for pressing button 2
			}
			for(uint8_t i=2;i<6;i++)
			clear_page(buffer,i);
			
			
		}
	}
}

//button 1 actions
ISR(INT0_vect)
{
	while(!(PIND & (1<<2)));
	//antibounce effect
	_delay_ms(150);
	if (screenmess<127)
	{
		//increment to variable checking screens
		screenmess *= 2;
		//if button is pressed again when we reach the final part of the menu reinitialize the variable
		if ((screenmess>4))
		screenmess=1;
	}
	else
	screenmess=1;
	print=0;//in order to rewrite the screen and to exit some modes
	
		
}


//button 2 actions
ISR(INT1_vect)
{
	while(!(PIND & (1<<3)));
	_delay_ms(150);
	//change to screen display angle
	if (screenmess==1)
	screenmess=8;
	else
	//change to screen select angle
	if (screenmess==2)
	screenmess=16;
	else
	//change to screen calibration
	if (screenmess==4)
	screenmess=128;
	print=0;
		
}
