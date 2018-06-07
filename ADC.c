/*
* ADC.c
*
* Purpose: This module initializes Analog to Digital converter of ATMEGA2560. Module is written for one ADC channel (no multiplexer).
* ADC is initialised to work on 10-bit resolution and 125kHz frequency on channel 0 and free running mode.
*
* Output: Function getSample() executes ADC conversion and stores result in variable result_10bit. Function convert_to_voltage() creates
* an array of characters in voltage_buffer[] that represents input voltage on ADC channel.
*
* Uses: avr/interrupt.h file to define port addresses and control register addresses, and io.h for standard definitions.
*
* Created: 2016-05-07
* Author: Audrius Stankus s140506
*/

#include "ADC.h"

void initADC(void){
	PRR0 &=~(1<<PRADC);			//Disble power saving on ADC
	ADCSRA &=~(1<<ADATE);		//Free running mode
	ADCSRA |= (1<<ADEN) | (1<<ADIE) | (1<<ADPS0) | (ADPS1) | (1<<ADPS2); //ADC enable, prescaled 128
	ADMUX |= (1<<REFS0);		//AVCC with external capacitor on AREF pin; ADC0 enabled; right adjusted;
	DIDR0 = 0b11111111;			//Disable unused inputs;
	DIDR2 = 0b11111111;			//Disable unused inputs
}

/* Polling method, ADIE in ADCSRA must be disabled

uint16_t getSample(void){
	DIDR0 &=~(1<<0);			//Enable ADC channel
	ADCSRA |=(1<<ADSC);			//Start conversion 1x
	while (!(ADCSRA & (1<<ADIF))){};	//Wait for conversion to complete
	ADCSRA |= (1<<ADIF);
	result_10bit = ADCL + ADCH*256;		//Read digital value
	return result_10bit;
}

*/

/* Function to start one conversion and get result saved in variable result_10bit */
void getSample(void){
	DIDR0 &=~(1<<0);		//Enable ADC channel
	ADCSRA |=(1<<ADSC);		//Start conversion 1x
	while (!(sample_ready));//Wait until interrupt for finished reading raises flag
	sample_ready = 0;		
}

/* Function to represent ADC reading as voltage on ADC input channel */
void convert_to_voltage(uint16_t to_convert){
	temp = to_convert*10;					//Multiplying to avoid floating point division
	voltage_buffer[0] = 0x30 + temp/2046;	//Convert first decimal to ascii
	temp = temp % 2046;						//Discard first decimal of voltage value
	voltage_buffer[1] = '.';
	voltage_buffer[2] = 0x30 + temp*10/2046;	//Convert second decimal to ascii
	temp = (temp*10) % 2046;					//Discard second decimal from temp
	voltage_buffer[3] = 0x30 + temp*10/2046;	//Convert thrid decimal to ascii
	temp = (temp*100) % 2046;					//Discard third decimal

	if (temp>=1023)		//if fourth decimal is higher that half
	{
		voltage_buffer[3]++;	//rounding third decimal up.
	}
	if (voltage_buffer[3]==0x3A) //if third decimal is "10"
	{
		voltage_buffer[3] = 0x30;
		voltage_buffer[2]++;
	}
	if (voltage_buffer[2]==0x3A)//if second decimal is "10"
	{
		voltage_buffer[2] = 0x30;
		voltage_buffer[0]++;
	}
}

ISR(ADC_vect)	//Fires when ADC conversion is complete
{
	result_10bit = ADCL + ADCH*256;
	sample_ready = 1;
}

