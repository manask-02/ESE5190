/*
 * partd.c
 * Created: 10/10/2023 9:53:02 PM
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

volatile unsigned long t_1;
volatile unsigned long t_2;
volatile int done_measure;
volatile unsigned long distance;
volatile int mode = 0;
char buffer[50];

volatile int curr_ocr0a = 118;

void Initialize() {
	cli();
	UART_init(BAUD_PRESCALER);
	DDRB &= ~(1 << DDB1);
	DDRB |= (1 << DDB0);
	DDRD &= ~(1 << DDD3);
	PORTD |= (1 << DDD3);
	DDRD |= (1 << DDD5);
	PORTD &= ~(1 << PORTD5);
	
	TCCR0A |= (1 << WGM00);
	TCCR0B |= (1 << WGM02);
	TCCR0B |= (1 << CS01);
	TCCR0B |= (1 << CS00);
	TCCR0A |= (1 << COM0A0);
	
	t_1 = 0;
	t_2 = 0;
	done_measure = 1;
	distance = 0;
	
	TCCR1B |= (1 << CS11);
	
	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT1);
	EICRA |= (1 << ISC11);
	EICRA &= ~ (1 << ISC10);	
	EIMSK |= (1 << INT1);
	sei();
}


ISR(PCINT0_vect) {
	if (PINB & (1 << PINB1)) {
		done_measure = 0;
		t_1 = TCNT1;
	}
	else {
		t_2 = TCNT1;
		done_measure = 1;
	}	
}


ISR(INT1_vect) {
	mode = 1 - mode;
}

int main(void)
{
	Initialize();
	while (1)
	{
		if (done_measure == 1) {
			if (t_2 > 0) {
				long period;
				if (t_2 > t_1)
					period = (t_2 - t_1) / 2;
				else
					period = (65535 - t_1 + t_2) / 2;
				distance = period / 59;
				UART_putstring(buffer);	
				sprintf(buffer, "%ld%s", distance, " cm\r\n");			
				if (mode == 0) {
					OCR0A = 29 * distance / 223 + 30;
				}
				
				else {
					if (distance >= 0 && distance < 30) {
						OCR0A = 30;
					}
					else if (distance >= 30 && distance < 60) {
						OCR0A = 31;
					}
					else if (distance >= 60 && distance < 90) {
						OCR0A = 35;
					}
					else if (distance >= 90 && distance < 120) {
						OCR0A = 39;
					}
					else if (distance >= 120 && distance < 150) {
						OCR0A = 44;
					}
					else if (distance >= 150 && distance < 180) {
						OCR0A = 46;
					}
					else if (distance >= 180 && distance < 210) {
						OCR0A = 52;
					}
					else if (distance >= 210 && distance <= 223) {
						OCR0A = 59;
					}
				}
				if (mode == 0) {
					UART_putstring("continuous mode \n");
				}
				else {
					UART_putstring("discrete mode \n");
				}
				t_1 = 0;
				t_2 = 0;
			}
			PORTB |= (1 << PORTB0);
			_delay_us(10);
			PORTB &= ~(1 << PORTB0);
			_delay_ms(60);
		}
	}
}

