/*
 * main.c
 * Created: 10/19/2023 9:56:52 PM
 * Author : manask
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"


#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

volatile unsigned long t_1;
volatile unsigned long t_2;
volatile int done_measure;
volatile unsigned long distance;
volatile int mode = 0;
volatile int adc_complete = 0;
char buffer[50];

void Initialize() {
	cli();
	UART_init(BAUD_PRESCALER);
	DDRB &= ~(1 << DDB1); // Setting the echo pin as an input
	DDRB |= (1 << DDB0); // Setting the trig pin as an output
	DDRD &= ~(1 << DDD3); //Input for button
	PORTD |= (1 << DDD3); // Enabling internal pull up for button
	
	DDRD |= (1 << DDD5); // This pin will be connected to the resistor and effectively to the base of BJT
	PORTD &= ~(1 << PORTD5); 
	TCCR0A = (1 << WGM00); //Phase correct PWM mode
	TCCR0B |= (1 << WGM02);
	TCCR0A |= (1 << COM0B1); //Clearing OC0B
	TCCR0B |= ((1 << CS01) | (1 << CS00));
	OCR0A = 119;
	OCR0B = 60;
	
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
	
	DDRC &= ~(1 << DDC0);
	PRR &= ~(1 << PRADC);
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	ADCSRA |= ((1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2));
	
	ADMUX &= ~(1 << MUX0);
	ADMUX &= ~(1 << MUX1);
	ADMUX &= ~(1 << MUX2);
	ADMUX &= ~(1 << MUX3);
	
	ADCSRA |= (1 << ADATE);
	
	ADCSRB &= ~(1 << ADTS0);
	ADCSRB &= ~(1 << ADTS1);
	ADCSRB &= ~(1 << ADTS2);
	
	DIDR0 |= (1 << ADC0D);
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE);
	ADCSRA |= ( 1 << ADSC);			
	
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

ISR(ADC_vect) {
	adc_complete = 1;
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
				distance = period / 58;
				sprintf(buffer, "%ld%s", distance, " cm\r\n");
				UART_putstring(buffer);
				
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
		if (adc_complete == 1) {
			if (ADC >= 0 && ADC < 101) {
				OCR0B = (int)(OCR0A * 5.0 / 100.0);
				UART_putstring("ADC: ");
				sprintf(buffer, "%d, Duty Cycle:0.05", ADC);
				UART_putstring(buffer);
				UART_putstring("\r\n");
			}
			else if (ADC >= 101 && ADC < 190) {
				OCR0B = (int)(OCR0A * 10.0 / 100.0);
				UART_putstring("ADC: ");
				sprintf(buffer, "%d, Duty Cycle:0.1", ADC);
				UART_putstring(buffer);
				UART_putstring("\r\n");
			}
			else if (ADC >= 190 && ADC < 279) {
				OCR0B = (int)(OCR0A * 15.0 / 100.0);
				UART_putstring("ADC: ");
				sprintf(buffer, "%d, Duty Cycle:0.15", ADC);
				UART_putstring(buffer);
				UART_putstring("\r\n");
			}
			else if (ADC >= 279 && ADC < 368) {
				OCR0B = (int)(OCR0A * 20.0 / 100.0);
				UART_putstring("ADC: ");
				sprintf(buffer, "%d, Duty Cycle:0.2", ADC);
				UART_putstring(buffer);
				UART_putstring("\r\n");
			}
			else if (ADC >= 368 && ADC < 457) {
				OCR0B = (int)(OCR0A * 25.0 / 100.0);
				UART_putstring("ADC: ");
				sprintf(buffer, "%d, Duty Cycle:0.25", ADC);
				UART_putstring(buffer);
				UART_putstring("\r\n");
			}
			else if (ADC >= 457 && ADC < 546) {
				OCR0B = (int)(OCR0A * 30.0 / 100.0);
				UART_putstring("ADC: ");
				sprintf(buffer, "%d, Duty Cycle:0.3", ADC);
				UART_putstring(buffer);
				UART_putstring("\r\n");
			}
			else if (ADC >= 546 && ADC < 635) {
				OCR0B = (int)(OCR0A * 35.0 / 100.0);
				UART_putstring("ADC: ");
				sprintf(buffer, "%d, Duty Cycle:0.35", ADC);
				UART_putstring(buffer);
				UART_putstring("\r\n");
			}
			else if (ADC >= 635 && ADC < 724) {
				OCR0B = (int)(OCR0A * 40.0 / 100.0);
				UART_putstring("ADC: ");
				sprintf(buffer, "%d, Duty Cycle:0.4", ADC);
				UART_putstring(buffer);
				UART_putstring("\r\n");
			}
			else if (ADC >= 724 && ADC < 813) {
				OCR0B = (int)(OCR0A * 45.0 / 100.0);
				UART_putstring("ADC: ");
				sprintf(buffer, "%d, Duty Cycle:0.45", ADC);
				UART_putstring(buffer);
				UART_putstring("\r\n");
			}
			else if (ADC >= 813 && ADC < 903) {
				OCR0B = (int)(OCR0A * 50.0 / 100.0);
				UART_putstring("ADC: ");
				sprintf(buffer, "%d, Duty Cycle:0.5", ADC);
				UART_putstring(buffer);
				UART_putstring("\r\n");
			}
			adc_complete = 0;
		}		
	}
}

