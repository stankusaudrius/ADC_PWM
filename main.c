/*
*
* ADC_PWM_Opg5.c
*
* Purpose: This module first initializes two timers, one of which on Pulse Width Modulation, 
* A/D converter, and communication through UART0. UART0 is used to communicate with terminal 
* and set minimum and maximum values of PWM duty cycle. When values are set, program checks if
* ADC reading value is within limits and if so, sets PWM duty cycle on that value. If ADC 
* reading value is outside limits, PWM duty cycle is set on either lower on higher limit which were set 
* through terminal.
* 
*
* Input: voltage_buffer[] and result_10bit are passed from ADC.c module. Variables lower_limit and 
* higher_limit are set through terminal. Update_PWM_duty_cycle_flag is a flag passed from Timers.c module

* Uses: avr/interrupt.h file to define port addresses and control register addresses, and io.h for 
* standard definitions. Libraries stdlib.h and stdio.h for certain functions and header files for 
* other modules uart.h, ADC.h, Timers.h
*
* Created: 2016-05-07
* Author: Audrius Stankus s140506
*/

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "ADC.h"
#include "Timers.h"
void initInterrupts(void);

enum state {Initialization, Idle, uart, Update_PWM_duty_cycle};
volatile char state = Initialization;
char voltage_buffer[4];
unsigned int lower_limit;
unsigned int higher_limit;
unsigned char limits_ready;
volatile  uint16_t result_10bit;
volatile unsigned char Update_PWM_duty_cycle_flag;
volatile unsigned int ii;
unsigned int i, a; 

int main(void)
{
	initInterrupts();
	init_timer();
	initADC();
	initUSART0(MY_UBRR);		//Initialize USART0

    while (1) 
    {
		switch (state)
		{
			case Initialization:
			newline();		//sends 0x0D and 0x0A to go to the next line
			newline();
			putstring_USART0("Please input PWM lower and higher limit 0-1023 in format x-y");
			newline();
			continue_logging = 1;	//Accept input from terminal
			limits_ready=0;			//PWM outputs zero until this flag is raised
			state = Idle;
			
			case Idle:
		
			if (string_klar == 1 )			//if enter was pressed or carriage return command sent
			{
				sscanf(buffe, "%04d-%04d", &lower_limit, &higher_limit); //function to scan input string and extract decimal values from there
				if ((!(lower_limit>=0 && lower_limit<=1023) || !(higher_limit>=lower_limit && higher_limit<=1023)))		//if input values are not ok, reports error
				{
					putstring_USART0("Error, try again");
					newline();
					newline();
					continue_logging = 1;
					lower_limit = 0;
					higher_limit = 0;
					for (a=0; a<=ii; a++)
					{
						buffe[a]=0; //clear contents of array
					}
						
				}

				else{limits_ready = 1;	}
			
				ii=0;		//put first character of next string again on first position in array buffe.
				string_klar = 0 ;		//reset flag so that if statement is only executed once when new string is received
			}
			
			if ((Update_PWM_duty_cycle_flag==1) & (limits_ready == 1 )) //PWM outputs 0 (OCR3A=0) until limits are set
			{
				state = Update_PWM_duty_cycle;
				Update_PWM_duty_cycle_flag = 0;
			}
			break;
			
			case Update_PWM_duty_cycle:
				if ((result_10bit>=lower_limit) && (result_10bit<=higher_limit)) //If pot position within set limits
				{
					write_16bit_OCR3A(result_10bit);	//Set duty cycle to value from ADC reading
				}
				else	//if pot position outside of limits set duty cycle to lower or higher limit
				{
					if (result_10bit<lower_limit)
					{
						write_16bit_OCR3A(lower_limit);
					}
					else if (result_10bit>higher_limit)
					{
						write_16bit_OCR3A(higher_limit);
					}
				}
				state = uart;
				break;

			case uart:
	
			if ((result_10bit<lower_limit) || (result_10bit>higher_limit)) 
			{
				i++;
				if (i>=100)		//Slowing down output to uart
				{
					putstring_USART0("Pot position out of limits: ");
					putinteger_USART0(lower_limit);
					putchUSART0(0x20);		//Space
					putchUSART0(0x2D);		//dash
					putchUSART0(0x20);		//Space
					putinteger_USART0(higher_limit);
					putstring_USART0(",  ADC reading: ");
					putinteger_USART0(result_10bit);
					putstring_USART0(",  Input Voltage: ");
					convert_to_voltage(result_10bit);
					putstring_USART0(&voltage_buffer[0]);
					putstring_USART0(" [V] ");
					newline();
					i=0;
				}
			}
			
			else	//if pot position is within limits
			{
				i++;
				if (i>=100)		//Slowing down output to UART0
				{
					putstring_USART0("Nice! Limits: ");
					putinteger_USART0(lower_limit);
					putchUSART0(0x20);
					putchUSART0(0x2D);
					putchUSART0(0x20);
					putinteger_USART0(higher_limit);
					putstring_USART0(",  ADC reading: ");
					putinteger_USART0(result_10bit);
					putstring_USART0(",  Input Voltage: ");
					convert_to_voltage(result_10bit);
					putstring_USART0(&voltage_buffer[0]);
					putstring_USART0(" [V] ");
					newline();
					i=0;
				}			
			}
			state=Idle;
			
			break;
			}

/*
		if(result_10bit >= 1000)  {	PORTB |= (1<<PB7);}		//Initial testing with boards' LED
		else{PORTB &=~(1<<PB7);}
*/
    }
}

void initInterrupts(void)
{
	sei();							//Enable global interrupts
}









