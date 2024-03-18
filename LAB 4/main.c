/*
 * Initialize_Screen.c
 *
 * Created: 2023/10/20 14:33:48
 * Author : manask
 */ 

#define F_CPU 16000000UL

#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)
#define BALL_RADIUS 3
#define PADDLE_SPEED 1  // speed at which the paddle moves
#define Height_Boundary0 7
#define Height_Boundary1 117
#define LED1_PIN PD5
#define LED2_PIN PD2
#define BUZZER_PIN PD4
#define Switch1_PIN PD7
//#define duration_ms=

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h> // Include the delay header
#include "lib\ST7735.h"
#include "lib\LCD_GFX.h"
#include "uart.h"

uint8_t leftPaddleY = 50;
uint8_t leftPaddleY0= 50;
uint8_t leftPaddleY1= 70;
	
uint8_t rightPaddleY = 50;
uint8_t rightPaddleY0= 50;
uint8_t rightPaddleY1= 70;

uint8_t leftPlayerScore=0;
uint8_t rightPlayerScore = 0;

uint8_t round_win_leftPlayer=0;
uint8_t round_win_rightPlayer=0;

// Initialize ball
uint8_t ballX = LCD_WIDTH / 2;
uint8_t ballY = LCD_HEIGHT / 2;
int8_t rightPaddleDir = 1;  // initial direction of right paddle, 1 for down, -1 for up

//initialize the ball direction and speed
int8_t ballDirX =1;  //
int8_t ballDirY = 1;
uint8_t ballSpeed = 1; 

uint8_t buttonPressed = 0; // Flag to check if the button is pressed

char String[50];
char ScoreBoard[50];  
char RoundScoreBoard[50]; //String to draw

void Initialize()
{
	UART_init(BAUD_PRESCALER);
	lcd_init();
	
	// Set PD7 as an input
	DDRD &= ~(1 << Switch1_PIN);
	// Enable internal pull-up resistor for PD7
	PORTD |= (1 << Switch1_PIN);
	
	// Configure PD5 as outputs for LED & buzzer
	DDRD |=  (1 << LED1_PIN) |(1 << LED2_PIN)| (1 << BUZZER_PIN);
	
	// Initialize ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Set ADC prescaler to 128
	ADMUX |= (1 << REFS0);  // Set reference voltage to AVcc
	ADCSRA |= (1 << ADEN);  // Enable ADC
	
}

uint16_t ReadADC(uint8_t channel)
{
	// Select ADC channel
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

	// Start conversion
	ADCSRA |= (1 << ADSC);

	// Wait for conversion to complete
	while (ADCSRA & (1 << ADSC));

	// Return result
	return ADCW;
}

//Reset the move direction and speed of ball randomly
void Reset_Ball_Randomly(){
// Reset the ball's position to the center of the screen
ballX = LCD_WIDTH / 2;
ballY = LCD_HEIGHT / 2;
ballDirX = rand() % 2 ? 1 : -1;  // Randomly choose initial direction
ballDirY = rand() % 2 ? 1 : -1;
//ballSpeed = rand() % 3 + 1;  // Randomly choose initial speed
}

//update scoreboard
void updateScoreBoard(){
sprintf(ScoreBoard, "H:%d C:%d D:%d", leftPlayerScore, rightPlayerScore, ballSpeed); // Format the string
LCD_drawString(55, 0,ScoreBoard,rgb565(255, 255, 255),rgb565(0,0,0));
sprintf(RoundScoreBoard, "Round H:%d C:%d", round_win_leftPlayer, round_win_rightPlayer); // Format the string
LCD_drawString(45, 118,RoundScoreBoard,rgb565(255, 255, 255),rgb565(0,0,0));
}

void updateRightPaddle()
{
	if (rightPaddleDir == 1)
	{
		// Move the paddle down
		if (rightPaddleY < Height_Boundary1 - 20)
		{
			LCD_drawBlock(156, rightPaddleY, 159, rightPaddleY+20, rgb565(0, 0, 0));
			rightPaddleY += PADDLE_SPEED;
			LCD_drawBlock(156, rightPaddleY, 159, rightPaddleY+20, rgb565(255, 255, 255));
		}
		else
		{
			// Reverse the direction if the paddle hits the bottom boundary
			rightPaddleDir = -1;
		}
	}
	// If the paddle is moving up
	else if (rightPaddleDir == -1)
	{
		// Move the paddle up
		if (rightPaddleY > Height_Boundary0)
		{
			LCD_drawBlock(156, rightPaddleY, 159, rightPaddleY+20, rgb565(0, 0, 0));
			rightPaddleY -= PADDLE_SPEED;
			LCD_drawBlock(156, rightPaddleY, 159, rightPaddleY+20, rgb565(255, 255, 255));
		}
		else
		{
			// Reverse the direction if the paddle hits the top boundary
			rightPaddleDir = 1;
		}
	}
}


void flashLED1()
{
	PORTD |= (1 << LED1_PIN);    // Turn ON the LED at PB1 for Dot
    Delay_ms(50);          // Keep it ON for 50ms
    PORTD &= ~(1 << LED1_PIN);   // Turn OFF the LED at PB1
}

void flashLED2()
{
	PORTD |= (1 << LED2_PIN);    // Turn ON the LED at PB1 for Dot
	Delay_ms(50);          // Keep it ON for 50ms
	PORTD &= ~(1 << LED2_PIN);   // Turn OFF the LED at PB1
}

void buzzer()
{
	uint16_t frequency=2500;
	uint16_t duration_ms=50;
	uint16_t period_us = 1000000UL / frequency;  // Calculate the period in microseconds
	    
	// Calculate the number of cycles needed for the desired duration
	uint16_t num_cycles = (duration_ms * 1000) / (2 * period_us);

    
    for (uint16_t i = 0; i < num_cycles; i++) {
	    // Toggle the buzzer pin to create the sound
	    PORTD ^= (1 << BUZZER_PIN);
	    _delay_us(period_us / 2);  // Half the period
    }
    
    // Turn off the buzzer
    PORTD &= ~(1 << BUZZER_PIN);
}


int main(void)
{
	Initialize();

	LCD_setScreen(rgb565(0, 0, 0));
	// Define top-left corner, width, height, and color of the block
	uint16_t color = rgb565(255, 255, 255);  // White
	
	//draw the boundary region
/*	LCD_drawLine(0,Height_Boundary0,LCD_WIDTH,Height_Boundary0,color);
	LCD_drawLine(0,Height_Boundary0,0,Height_Boundary1,color);
	LCD_drawLine(0,Height_Boundary1,LCD_WIDTH,Height_Boundary1,color);
	LCD_drawLine(LCD_WIDTH,Height_Boundary1,LCD_WIDTH,Height_Boundary1,color);*/
	
	// Draw block
	LCD_drawBlock(0, 50, 3, 70, color); //left paddle
	LCD_drawBlock(156, 50, 159, 70, color); //right paddle
	
	uint16_t adcValue;
	
	//Randomly choose the initial direction and speed of ball
	Reset_Ball_Randomly();


	// Draw initial ball
	LCD_drawCircle(ballX, ballY, BALL_RADIUS, rgb565(255, 255, 255));
	updateScoreBoard();//initialize scoreboard

	while (1)
	{
		// Erase previous ball
		LCD_drawCircle(ballX, ballY, BALL_RADIUS, rgb565(0, 0, 0));
		
		// Update ball position		
		ballX += ballDirX * ballSpeed;
		ballY += ballDirY * ballSpeed;
		
		// Constrain ball within horizontal range
		if (ballX - BALL_RADIUS <= 4)
		{
		  // Check if ball is within left paddle range
		  if (ballY >= leftPaddleY0 && ballY <= leftPaddleY1)
		  {
			  ballDirX = 1;
			  buzzer();
		  }
		  else  // Ball missed the left paddle, right player scores a point
		  {
		      rightPlayerScore++;
			  flashLED1();
			  buzzer();
			  
			  	//Win the round if current round score of one player >=10
			  if(rightPlayerScore>=5)
			  {
				  round_win_rightPlayer++;
				  //Reset the current round score
				  rightPlayerScore=0;
				  leftPlayerScore=0;
			  }
		      // Display the new score
		      updateScoreBoard();

		      // Reset the ball to set the direction and speed randomly
			  Reset_Ball_Randomly();	  
		  }
		}
		else if (ballX + BALL_RADIUS >= 155) 
		{
		   // Check if ball is within right paddle range
            if (ballY >= rightPaddleY0 && ballY <= rightPaddleY1)
            {
                ballDirX = -1;
				buzzer();
            }
            else // Ball missed the right paddle, left player scores a point
            {
       		      leftPlayerScore++;
				  flashLED2();
				  buzzer();
					 
				  //Win the round if current round score of one player >=10
				  if (leftPlayerScore>=5)
				  {
					round_win_leftPlayer++;
					leftPlayerScore=0; 
					rightPlayerScore=0;//Reset the current round score
				  }

       		       // Display the new score
       		      updateScoreBoard();

       		     // Reset the ball to set the direction and speed randomly
				  Reset_Ball_Randomly();
            }
		}		
		
		if (round_win_leftPlayer>=3 || round_win_rightPlayer>=3)
		{
			break;
		}
		
		
		// Bounce off top and bottom walls
		if (ballY - BALL_RADIUS <= Height_Boundary0 || ballY + BALL_RADIUS >= Height_Boundary1)
		{
			  ballDirY = -ballDirY;
			  buzzer();
		}
		
		//Button to choose difficulty
		// Check if the button is pressed (PD7 pulled low)
		if (!(PIND & (1 << PD7)) && !buttonPressed) {
			// Update the speed value
			ballSpeed = (ballSpeed % 3) + 1;

			// Mark that the button is pressed
			buttonPressed = 1;
			} else if (PIND & (1 << PD7)) {
			// If the button is released, reset the pressed flag
			buttonPressed = 0;
		}  
		
		// Read ADC value from VRy pin
		adcValue = ReadADC(0);
		
		//sprintf(String, "ADC value: %d\r\n", adcValue); // Format the string
		//UART_putstring(String);
		// Update paddle position based on ADC value
		if (adcValue < 512)
		{
			if (leftPaddleY > Height_Boundary0)
			{
				LCD_drawBlock(0, leftPaddleY, 3, leftPaddleY + 20, rgb565(0, 0, 0));  // Erase previous paddle
				leftPaddleY--;
				LCD_drawBlock(0, leftPaddleY, 3, leftPaddleY + 20, color);  // Draw new paddle
			}
		}
		else if (adcValue > 540)
		{
			if (leftPaddleY < Height_Boundary1 - 20)
			{
				LCD_drawBlock(0, leftPaddleY, 3, leftPaddleY + 20, rgb565(0, 0, 0));  // Erase previous paddle
				leftPaddleY++;
				LCD_drawBlock(0, leftPaddleY, 3, leftPaddleY + 20, color);  // Draw new paddle
			}
		}
		Delay_ms(100);
		leftPaddleY0=leftPaddleY;
		leftPaddleY1=leftPaddleY+20;

		//Move the right paddle (computer)
		adcValue = ReadADC(1);
		
		//sprintf(String, "ADC value: %d\r\n", adcValue); // Format the string
		//UART_putstring(String);
		// Update paddle position based on ADC value
		if (adcValue < 512)
		{
			if (rightPaddleY > Height_Boundary0)
			{
				LCD_drawBlock(156, rightPaddleY, 159, rightPaddleY+20, rgb565(0, 0, 0));
			    rightPaddleY--;
				LCD_drawBlock(156, rightPaddleY, 159, rightPaddleY+20, rgb565(255, 255, 255));
			}
		}
		else if (adcValue > 540)
		{
			if (rightPaddleY < Height_Boundary1 - 20)
			{
				LCD_drawBlock(156, rightPaddleY, 159, rightPaddleY+20, rgb565(0, 0, 0));
				rightPaddleY ++;
				LCD_drawBlock(156, rightPaddleY, 159, rightPaddleY+20, rgb565(255, 255, 255));
			}
		}
		
		rightPaddleY0=rightPaddleY;
		rightPaddleY1=rightPaddleY+20;
		
		
		// Draw new ball
		LCD_drawCircle(ballX, ballY, BALL_RADIUS, rgb565(255, 255, 255));
		Delay_ms(300);
	}
	
LCD_setScreen(rgb565(0, 0, 0));

if (round_win_rightPlayer>=3)
{
	LCD_drawString(40, 63,"P2 Win",rgb565(255, 255, 255),rgb565(0,0,0));
}
else
{
	LCD_drawString(40, 63,"P1 Win",rgb565(255, 255, 255),rgb565(0,0,0));
} 
	
}
