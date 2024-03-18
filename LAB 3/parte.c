/*
 * parte.c
 * Created: 10/17/2023 11:38:33 AM
 * Author : manask
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "inc/uart.h"

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

volatile int adc_complete = 0;
char buffer[100];

void Initialize() {
	cli();
	UART_init(BAUD_PRESCALER);
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

ISR(ADC_vect) {
	adc_complete = 1;
}

int main(void)
{
	Initialize();
    while (1) 
    {
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

