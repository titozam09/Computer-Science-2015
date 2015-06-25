//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+Computer science 2015 Project

//+Digital Spirit Level

//+Library for the accelerometer
 MMA7260QT
//+

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++





//Created: 05/05/2015 11:26:15 a. m.


//This library is intended to convert the ADC data to a measurable angle from the accelerometer MMA7260QT.
//Revision history:
//Version 0: First development
//


#include <avr/io.h>
#include <math.h>
#include <util/delay.h>



void calibration(uint8_t *measurement)
{
	
	uint8_t i=0;
	
	for (i=0;i<=2;i++)
	{
		
		//Add button to calibrate every axis... Just for testing purposes/will be changed in the oncoming versions
		while((PINB & (1<<2))){}
		while(!(PINB & (1<<2))){}
		//setting the channel to measure the corresponding angle
		ADMUX &= 0b11110000;
		ADMUX = ADMUX | i;
		//start the conversion. Then wait until conversion is completed. After redo this procedure in roder to make it more reliable
		ADCSRA |= (1<<ADSC);
		loop_until_bit_is_clear(ADCSRA,ADSC);
		ADCSRA |= (1<<ADSC);
		loop_until_bit_is_clear(ADCSRA,ADSC);
		//copy result into array
		measurement[i]=ADCH;
		
	}
	
		
}

void measure(uint8_t *measurement)
{
	
	uint8_t i=0;
	
	for (i=0;i<=2;i++)
	{
		//setting the channel to measure the corresponding angle
		ADMUX &= 0b11110000;
		ADMUX = ADMUX | i;
		//start the conversion. Then wait until conversion is completed. After redo this procedure in roder to make it more reliable
		ADCSRA |= (1<<ADSC);
		loop_until_bit_is_clear(ADCSRA,ADSC);
		ADCSRA |= (1<<ADSC);
		loop_until_bit_is_clear(ADCSRA,ADSC);
		//copy result into array
		measurement[i]=ADCH;
	}
	

}

//converting raw measurement from ADC values into a floating data type
void conversion(uint8_t *calibrate, float *angle, uint8_t *measurement)
{
	float components[3]={};
	float resultant=0;
	uint8_t i=0;
	
	for (i=0;i<=2;i++)
	{
		//Convert measurement and zero's into a component of gravity vector ((measurement-zero)*Vref[5V]/ADC resolution[255])/Sensibility[800mV/g]
		components[i]=((measurement[i]-calibrate[i])*4/255.0);
		//Add up the components squared in order to obtain...
		resultant=resultant + (components[i]*components[i]);
	}
	// ...Pythgoras theorem
	resultant=sqrt(resultant);
	
	for (i=0;i<=2;i++)
	{
		//obtain direction cosines of the resultant/truncate the value just to one decimal point.
		angle[i]=truncf((acos(components[i]/resultant))*572.9)/10;
		
	}
	
	
	
	
}

//self explaining function. Turns on the adc module of the uC
void turnon_adc(void)
{
	ADCSRA |= (1<<ADEN);
}

//self explaining function. Turns off the adc module of the uC
void turnoff_adc(void)
{
	ADCSRA &= ~(1<<ADEN);
}

//Initial setting of the adc module.
void setup_adc(void)
{
	//PINA0-2 as input with no pull-up
	DDRA = 0b11111000;
	PORTA = 0b00000000;
	//ADCSRA Not initialize ADC, no conversion, no autotrigger, no interrupt
	//ADMUX  right aligned, channel will be modified later, fosc/16, AVCC as reference
	ADMUX |= (1<<ADLAR) | (1<<REFS0) | (1<<ADPS2);
	
}

