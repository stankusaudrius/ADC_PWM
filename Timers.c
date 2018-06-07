/*
* 
* Timers.c
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Timers.h"
void init_timer(void)
{
	PRR1 &=~ (1<<PRTIM3);				//Disable power saving on Timer3
	PRR0 &=~ (1<<PRTIM1);				//Disable power saving on Time1
	TCCR1B =  (1<<CS10) | (1<<WGM12);	//timer/counter1 prescaler 1; CTC mode
	OCR1A = 600;						//16000000Hz/(prescaler*600) = 26.666kHz  -  frequency of matches with TCNT1.
	TCCR3A |= (1<<WGM31) | (1<<WGM30) | (1<<COM3A1); //Timer 3. Mode 3 - Phase correct PWM, set OC3A high on compare match when downcounting, clear when upcounting
	TCCR3B |= (1<<CS31);				//Timer3 prescaler 8
	DDRE |= (1<<DD3);					//DDR for OC3A pin as output for LED
	TIMSK1 |= (1<<OCIE1A);				//timer1 1 compare match A interrupt enable
	TIMSK3 |= (1<<TOIE3);				//Enable timer3 overflow interrupt
}

void write_16bit_OCR3A(uint16_t value_to_write )
{
	unsigned char sreg;
	sreg = SREG;	/* Save global interrupt flag */
	cli();			/* Disable interrupts */
	OCR3A = value_to_write;	/* Set register value */
	SREG = sreg;	/* Restore global interrupt flag */
}

ISR(TIMER3_OVF_vect)
{
	Update_PWM_duty_cycle_flag = 1;
}

ISR(TIMER1_COMPA_vect) 
{
	DIDR0 &=~(1<<0);
	ADCSRA |=(1<<ADSC);				//Start conversion 1x
	while (!(ADCSRA & (1<<ADIF))){};
}