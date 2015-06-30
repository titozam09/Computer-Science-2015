
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
//Version 1: Added most of functions, accelerometer not included 28/06/2015 11:35 p. m.
//Version 2: Accelerometer included, sleep function included 29/06/2015 12:45 a. m.
//Version 3: Comments added, fix delay in message of warning, issue with warning screen and clearing screen from "set reference" screen 30/06/2015 06:31 p. m.

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/sleep.h>
#include "stlcd.h"
#include "glcd.h"
#include "spi.h"
#include "accelerometer.h"

volatile uint8_t screenmess=0x01;
uint8_t print=0;
uint8_t selection=1;
uint8_t calibrationdone=0;
volatile uint8_t anglesetting=1;
volatile uint8_t tempvariable=0;
uint8_t buffer[1024]={0};

int main(void)
{
	//Declaration of variables, in order: angle from function set an angle, array containing the values of the set angle function, counter of the set angle function
	//ascii value of the counter, array containin measurement of the accelerometer, array containin the calibration values of the accelerometer, array
	//array containing the values of the calculation of angle, arrays containing ascii values of the calculation of angle,
	
	float angleset=0;
	uint8_t anglesetstring[4]={0};
	uint8_t j=0; //to print
	uint8_t stringj[1]={};
	uint8_t measurement[3];
	uint8_t calibrate[3];
	float angle[2];
	char num1[10]={0};
	char num2[10]={0};

	
	DDRB &= ~(1<<2) & ~(1<<3); //setting PB2 as input for interrupt INT 2
	DDRD &= ~(1<<2) & (1<<3); //setting PD2 and PD3 as input for interrupt INT0, INT1
	PORTB |= (1<<2) |(1<<3);
	PORTD |= (1<<2) | (1<<3); //enabling pull-up resistor in corresponding pins
	MCUCR &= ~(1<<ISC10) & ~(1<<ISC11); //setting for INT1 a low value interrupt
	MCUCR &= ~(1<<ISC00) & ~(1<<ISC01);//setting for  INT0 a low value interrupt
	MCUCSR &= ~(1<< ISC2); ////setting for INT2 as a falling edge interrupt
	GICR |= (1<<INT0) | (1<<INT1) | (1<<INT2); //Enabling interrupt
	sei();
	st7565_init();
	setup_adc();
	turnon_adc();
	DDRC = 0xFF;
	PORTC = 0x00;
	
	//endless loop of the uC
	
	while(1)
	{
		//Printing out the measure mode screen just once for preventing consuming time in the uC
		
		if ((screenmess==1) && (print==0))
		{
			//measuremode message
			print=1;
			clear_page(buffer,4);
			drawstring(buffer,15,4,"MEASUREMENT MODE");
			write_page(buffer,4);
		}
		
		//Printing out the set angle mode screen just once for preventing consuming time in the uC

		if ((screenmess==2) && (print==0))
		{
			//setanglemode message
			print=1;
			clear_page(buffer,4);
			drawstring(buffer,25,4,"SET ANGLE MODE");
			write_page(buffer,4);
		}

		//Printing out the set reference mode screen just once for preventing consuming time in the uC
		
		if ((screenmess==4) && (print==0))
		{
			//setreference message
			print=1;
			clear_page(buffer,4);
			drawstring(buffer,8,4,"SET REFERENCE MODE");
			write_page(buffer,4);
		}
		
		//Second screen of measurement mode activated by the button 2
		//Prints the corresponding "background" and then refreshes the screen with the arrow and the measured angles

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
			write_page(buffer,2);

			//Refreshes the values on the screen obtained from the accelerometer
			
			while (screenmess==8)
			{
				//ACCELEROMETER FUNCTION
				measure(measurement);
				conversion(calibrate,angle,measurement,0);
				dtostrf(angle[0],6,1,num1);
				dtostrf(angle[1],6,1,num2);
				drawstring(buffer,1,2,"X=       _ Y=      _");
				drawstring(buffer,13,2,num1);
				drawstring(buffer,79,2,num2);
				drawchar(buffer,1,3,88); //printing X
				drawchar(buffer,1,4,89); //printing Y
				drawchar(buffer,69+angle[0]/2,3,94);
				drawchar(buffer,69+angle[1]/2,4,94);
				
				write_page(buffer,2);
				write_page(buffer,3);
				write_page(buffer,4);
				_delay_ms(150);
				clear_page(buffer,2);
				clear_page(buffer,3);
				clear_page(buffer,4);
			}
			for(uint8_t i=1;i<7;i++)
			clear_page(buffer,i);
			angle[0]=0;
			angle[1]=0;
			angle[2]=0;
			
		}

		//Second screen of set angle mode activated by the button 2
		//Prints the corresponding "background" and waits for the button 3 to change digit
		//and waits for the button 2 (3 times) to change screen
		
		if ((screenmess==16) && (print==0))
		{
			//Ask for the desired angle
			angleset=0;
			print=1;
			selection=2;
			anglesetting=1;
			j=0;
			clear_page(buffer,4);
			drawstring(buffer,1,3,"SELECT DESIRED ANGLE");
			write_page(buffer,3);
			while (screenmess==16)
			{
				//function setting angle
				drawchar(buffer,85,4,95);
				
				//first digit
				do
				{
					itoa(j,stringj,10);
					drawchar(buffer,78,4,stringj[0]);
					write_page(buffer,4);
					j++;
					if (j>9)
					j=0;
					write_page(buffer,4);
					while ((PINB & (1<<3)) && anglesetting==1)
					_delay_ms(100);
					while (!(PINB & (1<<3)) && anglesetting==1)
					_delay_ms(100);
					
				} while (anglesetting==1);
				
				if (j==0)
				j=9;
				else
				j--;
				angleset+=j/10.;
				j=0;
				drawchar(buffer,72,4,'.');
				
				//second digit

				do {
					itoa(j,stringj,10);
					drawchar(buffer,66,4,stringj[0]);
					write_page(buffer,4);
					j++;
					if (j>9)
					j=0;
					write_page(buffer,4);
					while ((PINB & (1<<3)) && anglesetting==2)
					_delay_ms(100);
					while (!(PINB & (1<<3)) && anglesetting==2)
					_delay_ms(100);
					
				} while (anglesetting==2);
				
				if (j==0)
				j=9;
				else
				j--;
				angleset+=j;
				j=0;
				
				//third digit
				
				do
				{
					itoa(j,stringj,10);
					drawchar(buffer,60,4,stringj[0]);
					write_page(buffer,4);
					j++;
					if (j>9)
					j=0;
					while ((screenmess!=32) && ((PINB & (1<<3)) && (anglesetting==4)));
					_delay_ms(100);
					
					while ((screenmess!=32) && (!(PINB & (1<<3)) && (anglesetting==4)));
					_delay_ms(100);
					
				} while (anglesetting==4);
				if (j==0)
				j=9;
				else
				j--;
				angleset+=j*10;
				
			}
			clear_page(buffer,3);
			if (angleset>90)
			angleset=90;
			
		}
		

		//Third screen of set angle mode activated after the last screen is completed
		//Prints the corresponding "background" and then it updates measure and
		//graphical arrow

		if ((screenmess==32) && (print==0))
		{
			//Set angle mode
			print=1;
			selection=1;
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
			drawstring(buffer,25,1,"COMPARE ANGLE");
			write_page(buffer,1);
			write_page(buffer,6);
			write_page(buffer,5);
			drawstring(buffer,1,2,"SET ANGLE=       _");
			dtostrf(angleset,6,1,anglesetstring);
			drawstring(buffer,67,2,anglesetstring);
			write_page(buffer,2);
			while (screenmess==32)
			{
				//function setting angle
				measure(measurement);
				conversion(calibrate,angle,measurement,1);
				dtostrf(angle[0],6,1,num1);
				drawstring(buffer,1,3,"MEASURE  =       _");
				drawstring(buffer,67,3,num1);
				drawchar(buffer,1,4,77); //printing M
				drawchar(buffer,69+angle[0]/2,4,94);
				write_page(buffer,4);
				write_page(buffer,3);
				if ((angle[0]<=(angleset+0.2)) && (angle[0]>=(angleset-0.2)))
				PORTC= 0x01;
				else
				PORTC = 0x00;
				_delay_ms(150);
				
				clear_page(buffer,3);
				clear_page(buffer,4);
				
			}
			for(uint8_t i=1;i<7;i++)
			clear_page(buffer,i);
			angleset=0;
			
		}

		//Warning message when no reference has previously defined

		
		if ((screenmess==64) && (print==0))
		{
			//Warning message
			GICR &= ~(1<<INT0) & ~(1<<INT1) & ~(1<<INT2);
			print=1;
			clear_page(buffer,4);
			drawstring(buffer,9,0,"   WARNING! NO");
			drawstring(buffer,9,1," REFERENCE DEFINED.");
			drawstring(buffer,9,2," FOR ENTERING THIS");
			drawstring(buffer,9,3," MODE PLEASE SET A");
			drawstring(buffer,9,4,"  REFERENCE PLANE.");
			drawstring(buffer,9,5," YOU ARE GUIDED TO");
			drawstring(buffer,9,6,"   REFERENCE MODE");
			drawstring(buffer,9,7," WITHIN 10 SECONDS");
			for(uint8_t i=0;i<=7;i++)
			write_page(buffer,i);
			_delay_ms(5000);
			_delay_ms(5000);
			
			for(uint8_t i=0;i<=7;i++)
			clear_page(buffer,i);
			
			GICR |= (1<<INT0) | (1<<INT1) | (1<<INT2);
			tempvariable=0;
			screenmess=128;
			print=0;
			selection=2;
		}

		//Calibration message
		
		if ((screenmess==128) && (print==0))
		{
			//Calibration message
			print=1;
			selection=2;
			clear_page(buffer,4);
			drawstring(buffer,1,2,"1. ALIGN PRODUCT WITH   REFERENCE SURFACE.");
			drawstring(buffer,1,4,"2. PRESS BUTTON 2 FOR   CALIBRATION.");
			write_page(buffer,2);
			write_page(buffer,3);
			write_page(buffer,4);
			write_page(buffer,5);
			while (screenmess==128)
			{
				//wait for pressing button 2
			}
			calibration(calibrate);
			for(uint8_t i=2;i<6;i++)
			clear_page(buffer,i);
			tempvariable=0;
			
		}
		
		//Sleep mode
		
		if (tempvariable==1)
		{
			PORTC |= (1<<1);
			turnoff_adc();
			st7565_command(CMD_DISPLAY_OFF);
			spi_off();
			sleep_cpu();
			
			PORTC &= ~(1<<1);
			
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
	{
		screenmess=1;
		selection=1;
		
	}
	
	anglesetting=0;
	selection=1;
	print=0;//in order to rewrite the screen and to exit some modes
	
	
}


//button 2 actions
ISR(INT1_vect)
{
	while(!(PIND & (1<<3)));
	_delay_ms(150);
	
	if (selection==1)
	{
		
		if ((calibrationdone==0) && ((screenmess==1) || (screenmess==2))) //fix problem with set reference
		screenmess=64;
		else
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
	if (selection==2)
	{
		if (screenmess==16)
		{
			if (anglesetting==1)
			{
				anglesetting=2;
				
			}else
			
			if (anglesetting==2)
			{
				anglesetting=4;
				
			}else
			
			if (anglesetting==4)
			{
				
				screenmess=32;
				anglesetting=1;
				selection=1;
			}
			
		}
		if (screenmess==128)
		{
			calibrationdone=1;
			screenmess=1;
			selection=1;
			for(uint8_t i=2;i<6;i++)
			clear_page(buffer,i);
			drawstring(buffer,1,2,"   CALIBRATION DONE!");
			write_page(buffer,2);
			_delay_ms(1500);
			clear_page(buffer,2);
			
		}
		
		print=0;
	}
}

//button 4 action
ISR(INT2_vect)
{
	while(!(PINB & (1<<2)));
	_delay_ms(250);
	if(tempvariable==0)
	{
		if ( screenmess==1)
		{
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_enable();
		}else
		if ( screenmess==2)
		{
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_enable();
		}else
		if ( screenmess==4)
		{
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);
			sleep_enable();
		}
		tempvariable=1;
	} else
	
	{
		turnon_adc();
		spi_on();
		st7565_command(CMD_DISPLAY_ON);
		sleep_disable();
		screenmess=1;
		print=0;
		selection=1;
		anglesetting=1;
		tempvariable=0;
		_delay_ms(200);
		
	}
	
	if (screenmess==64)
	{
		tempvariable=0;
	}
	
	
	
	
	
}
