/*
 * partb.c
 *
 * Created: 05-10-2023 15:32:45
 * Author : manask
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>


int main(void) {
	 
	DDRB &= ~(1 << PORTB0); // Set PB0  as input
	PORTB |= (1 << PORTB0); //Internal pull-up enabled
	DDRB |= (1 << PORTB5); // Set PB5 (LED) as output

	while (1) {
		
		if (!(PINB & (1 << PORTB0))) // Read the state of the pushbutton interfaced at PORTB0
		{
			PORTB |= (1 << PORTB5);  // Turn the LED ON
		}
		else
		{
			PORTB &= ~(1 << PORTB5); // Turn the LED OFF
		}
	}

	return 0;
}
