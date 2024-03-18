/*
 * partb_1.c
 *
 * Created: 10/6/2023 11:39:56 AM
 * Author : manask
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

void Initialize() {
	cli();
	DDRD |= (1 << DDD6); // Setting pin D6 as an input
	PORTD &= ~(1 << PORTD6); //Setting the pin to 0
	
	TCCR0B |= (1 << CS02); // Setting the prescaler to 256
	TIMSK0 |= (1 << TOIE0); // Overflow Interrupt Enabled
	TIFR0 |= (1 << TOV0); 
	
	sei();	
}

ISR(TIMER0_OVF_vect) {
	PORTD ^= (1 << PORTD6);
}

int main(void)
{
    
	Initialize();
    while (1) 
    {
    }
}

