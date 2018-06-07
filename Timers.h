/*
* Timers.h
*
* Purpose: This module initializes Timer1 and Timer3 of ATMEGA2560. Timer 1 works on CTC mode and fires compare match interrupt on ca. 26.6kHz frequency.
* Timer 3 works as a 10-bit counter in phase correct PWM mode on 2Mhz frequency and variable duty cycle.
*
* Input: PWM duty cycle is determined by value_to_write, which is passed from main().
*
* Output: Binary flag Update_PWM_duty_cycle_flag is passed to main when Timer1 compare match interrupt flag fires.
*
* Uses: avr/interrupt.h file to define port addresses and control register addresses, and io.h for standard definitions.
*
* Created: 2016-05-07
* Author: Audrius Stankus s140506
*/
#ifndef TIMERS_H_
#define TIMERS_H_
#include <avr/interrupt.h>
#include <avr/io.h>
#endif /* TIMERS_H_ */
volatile extern uint16_t result_10bit;
volatile extern unsigned char Update_PWM_duty_cycle_flag;
volatile uint16_t value_to_write;

/* Function to be called in main in order to initialise Timers */
void init_timer(void);

/* Function to correctly write 16-bit value to OCR3A by temporarily disabling global interrupts 
so that value is not corrupted by another interrupt. Datasheet s. 135 */
void write_16bit_OCR3A(unsigned int value_to_write );

