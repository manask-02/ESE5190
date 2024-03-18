/*
 * partc.c
 * Created: 10/9/2023 9:44:36 PM
 * Author : manask
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "inc/uart.h"


#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

volatile unsigned long pulse_start;
volatile unsigned long pulse_end;;
volatile unsigned long pulse_duration;
volatile int measure_complete;
char buffer[50];

void Initialize() {
	cli();
	UART_init(BAUD_PRESCALER);
	DDRB |= (1 << DDB0);
	DDRB &= ~(1 << DDB1);
	PORTB |= (1 << DDB1);
	
	pulse_start = 0;
	pulse_end = 0;
	measure_complete = 1;
	
	TCCR1B |= (1 << CS11);	
	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT1);
	sei();	
}


ISR(PCINT0_vect) {
	if (PINB & (1 << PINB1)) {
		measure_complete = 0;
		pulse_start = TCNT1;
	} else {
		pulse_end = TCNT1;
		measure_complete = 1;
	}
}

int main(void)
{
	Initialize();
    while (1) 
    {
		if (measure_complete == 1) {
			if (pulse_end > 0) {
				long period;
				if (pulse_end > pulse_start)
					period = (pulse_end - pulse_start) / 2;
				else
					period = (65535 - pulse_start + pulse_end) / 2;
				long distance = period / 58;
				sprintf(buffer, "%ld%s", distance, " cm\r\n");
				UART_putstring(buffer);
				pulse_start = 0;
				pulse_end = 0;			
			}
			PORTB |= (1 << PORTB0);
			_delay_us(10);
			PORTB &= ~(1 << PORTB0);
			_delay_ms(60);
		}
    }
}

