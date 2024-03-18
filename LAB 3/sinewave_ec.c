/*
 * Lab3_sinewave_ec.c
 * Created: 10/19/2023 6:42:10 PM
 * Author : manask
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
int size = 100;
int sine_table[100] = {128, 136, 143, 151, 159, 167, 174, 182, 189, 196, 202, 209, 215, 220, 226, 231, 235, 239, 243, 246, 249, 251, 253, 254, 255, 255, 255, 254, 253, 251, 249, 246, 243, 239, 235, 231, 226, 220, 215, 209, 202, 196, 189, 182, 174, 167, 159, 151, 143, 136, 128, 119, 112, 104, 96, 88, 81, 73, 66, 59, 53, 46, 40, 35, 29, 24, 20, 16, 12, 9, 6, 4, 2, 1, 0, 0, 0, 1, 2, 4, 6, 9, 12, 16, 20, 24, 29, 35, 40, 46, 53, 59, 66, 73, 81, 88, 96, 104, 112, 119};
volatile int lookup_count = 0;

void Initialize() {
	cli();
	DDRB |= (1 << DDB1);
	DDRB |= (1 << DDB5);
	
	TCCR1B |= (1 << CS10);
	TCCR1A |= (1 << WGM10);
	OCR1A = 5;
	TIMSK1 |= (1 << TOIE1);
	TIFR1 |= (1 << TOV1);
	TCCR1A |= (1 << COM1A1);
	sei();
}

ISR(TIMER1_OVF_vect) {
	PORTB ^= (1 << PORTB5);
	if (lookup_count < size) {
		OCR1A = sine_table[lookup_count];
		lookup_count++;
	}
	else {
		lookup_count = 0;
	}
}

int main(void)
{
    Initialize();
    while (1) 
    {
    }
}

