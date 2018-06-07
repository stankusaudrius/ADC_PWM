/*
* ADC.h
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

#ifndef ADC_H_
#define ADC_H_
#include <avr/io.h>
#include <avr/interrupt.h>
#endif /* ADC_H_ */

volatile unsigned char sample_ready;
extern char voltage_buffer[4];
volatile extern uint16_t result_10bit;
uint16_t temp;

/* Function to start one conversion and get result saved in variable result_10bit */
void getSample(void);

/* Function to be called in main to initialise ADC */
void initADC(void);

/* Function to represent ADC reading as voltage on ADC input channel */
void convert_to_voltage(uint16_t to_convert);
