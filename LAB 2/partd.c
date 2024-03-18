#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

volatile uint32_t t2_pushbutton = 0;
volatile uint32_t t1_pushbutton = 0;
volatile uint32_t ovf_cnt = 0;

void Timer1_Initialize()
{
	TCCR1B |= (1 << CS11);   // Setting the prescaler value to 8
	TCCR1B |= (1 << ICES1);  // Capture on rising edge
	TIMSK1 |= (1 << ICIE1);  // Enable input capture
	TIMSK1 |= (1 << TOIE1); // Enable overflow interrupts
}


ISR(TIMER1_OVF_vect)
{
	ovf_cnt++;
	uint32_t time_instance = (ovf_cnt << 16) + TCNT1;
	if(time_instance - t1_pushbutton >800000) {
		UART_putstring(" ");
		t1_pushbutton = time_instance;
	}
}

ISR(TIMER1_CAPT_vect)
{
	uint16_t time_millis = ICR1;
	uint8_t state_millis = TCCR1B & (1 << ICES1);

	t2_pushbutton = (ovf_cnt << 16) | time_millis;

	if (state_millis)
	{  
		TCCR1B &= ~(1 << ICES1);  // Subsequent capture on falling edge 
		PORTB &= ~(1 << PORTB5);  // Resetting PORTB5
	}
	else
	{  
		TCCR1B |= (1 << ICES1);  // Subsequent capture on rising edge
		PORTB |= (1 << PORTB5);  // Setting PORTB5

		uint32_t time_elapsed = t2_pushbutton - t1_pushbutton;

		if (time_elapsed >= 60000 && time_elapsed <= 400000)  // The values for a prescaler value of 8 for 30ms to 200ms are 400000 and 60000
		{  
			UART_putstring(".");
		}
		else if (time_elapsed > 400000 && time_elapsed <= 800000) // The values for a prescaler value of 8 for 200ms to 400ms are 400000 and 800000
		{  
			UART_putstring("-");
		}
	}

	t1_pushbutton = t2_pushbutton;
}

int main(void)
{
	UART_init(BAUD_PRESCALER);
	Timer1_Initialize();

	DDRB &= ~(1 << PORTB0);   // Set PORTB0 as i/p
	PORTB |= (1 << PORTB0);   // Internal pull-up for PORTB0
	DDRB |= (1 << PORTB5);    // Set PORTB5 as o/p
	PORTB |= (1 << PORTB5);   // Set PORTB5 to HIGH

	sei();  // Enable global interrupts

	while(1)
	{
		
	}

	return 0;
}