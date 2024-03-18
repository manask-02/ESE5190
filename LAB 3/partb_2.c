/*
 * partb_2.c
 * Created: 10/6/2023 1:03:35 PM
 * Author : manask
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
volatile int curr_ocr0a = 70;

void Initialize() {
	cli();
	DDRD |= (1 << DDD6); // Setting pin D6 as an input
	PORTD &= ~(1 << PORTD6); //Setting the pin to 0
	
	TCCR0B |= (1 << CS02); // Setting the prescaler to 256
	TIMSK0 |= (1 << OCIE0A); //Compare match A interrupt is enabled
	TCCR0A |= (1 << COM0A0); // Toggling OC0A on compare match
	OCR0A = 70; // Setting OCR0A to a fixed value
	TIFR0 |= (1 << OCF0A); // Clearing OCF0A
	sei();	
}

ISR(TIMER0_COMPA_vect)
{
	PORTD ^= (1 << PORTD6); //Toggle PD6
	if (curr_ocr0a + 70 > 255) {
		OCR0A = 70 - (255 - curr_ocr0a);
	} else {
		OCR0A = curr_ocr0a + 70;
	}
	curr_ocr0a = OCR0A;
}

int main(void)
{
	Initialize();
    while (1) 
    {
    }
}

