/*
 * partb_3.c
 * Created: 10/6/2023 4:08:34 PM
 * Author : manask
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

void Initialize() {
	cli();
	DDRD |= (1 << DDD6); // Setting pin D6 as an input
	PORTD &= ~(1 << PORTD6); //Setting the pin to 0
	TCCR0A |= (1 << WGM01); //CTC mode
	TCCR0B |= (1 << CS02); // Setting the prescaler to 256
	TIMSK0 |= (1 << OCIE0A); //Compare match A interrupt is enabled
	TCCR0A |= (1 << COM0A0); // Toggling OC0A on compare match
	OCR0A = 70; // Setting OCR0A to a fixed value
	sei();
}

int main(void)
{
	Initialize();
    while (1) 
    {
    }
}

