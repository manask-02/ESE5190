/*
 * partc.c
 *
 * Created: 05-10-2023 19:59:58
 * Author : manask
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL

// Function to initialize the input capture
void Initialize_IC() {
	// set PB0 as input with pull-up enabled
	DDRB &= ~(1 << PORTB0);      // set PB0 as i/p
	PORTB |= (1 << PORTB0);      // Enable internal pull-up
	DDRB |= (1 << PORTB5);		// set PB5 as o/p
	PORTB |= (1 << PORTB5);		// Enable internal pull-up
	TCCR1B |= (1 << ICES1);   // Detect rising edge initially
	TIMSK1 |= (1 << ICIE1);   // i/p capture interrupt

	TCCR1B |= (1 << CS10);		// Start Timer1 with no prescaler
	TIFR1 |= (1 << ICF1);
}

int main(void) {
	cli();
	// Calling the function above
	Initialize_IC();
	// Enable global interrupts
	sei();
	while(1) {
		
	}
}

ISR(TIMER1_CAPT_vect) {

	if (PINB & (1 << PORTB0)) {
		PORTB &= ~(1 << PORTB5);
		
		TCCR1B &= ~(1 << ICES1); 		// Detects falling edge
	}
	else {
		
		PORTB |= (1 << PORTB5);
		TCCR1B |= (1 << ICES1);			// Detects rising edge
	}
	TIFR1 |= (1 << ICF1); 				// Clear ICF
}