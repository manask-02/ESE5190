/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: manask
 */ 

#include "LCD_GFX.h"
#include "ST7735.h"

/******************************************************************************
* Local Functions
******************************************************************************/



/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
* @brief		Convert RGB888 value to RGB565 16-bit color data
* @note
*****************************************************************************/
uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
}

/**************************************************************************//**
* @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
* @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
* @note
*****************************************************************************/
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
	LCD_setAddr(x,y,x,y);
	SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
* @brief		Draw a character starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
	uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
	int i, j;
	if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
		for(i=0;i<5;i++){
			uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
			for(j=0;j<8;j++){
				if ((pixels>>j)&1==1){
					LCD_drawPixel(x+i,y+j,fColor);
				}
				else {
					LCD_drawPixel(x+i,y+j,bColor);
				}
			}
		}
	}
}


/******************************************************************************
* LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
* You are free to create and add any additional files, libraries, and/or
*  helper function. All code must be authentically yours.
******************************************************************************/

/**************************************************************************//**
* @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
* @brief		Draw a colored circle of set radius at coordinates
* @note
*****************************************************************************/
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color)
{
	int16_t x1 = x0 - radius;
	int16_t x2 = x0 + radius;
	int16_t y1 = y0 - radius;
	int16_t y2 = y0 + radius;

	// Set the address to the entire bounding box of the circle
	LCD_setAddr(x1, y1, x2, y2);
	
	clear(LCD_PORT, LCD_TFT_CS);    //CS pulled low to start communication

	for (int16_t x = x1; x <= x2; x++) {
		for (int16_t y = y1; y <= y2; y++) {
			int16_t dx = x - x0;
			int16_t dy = y - y0;
			if (dx*dx + dy*dy < radius*radius) {
				SPI_ControllerTx_16bit_stream(color);
				} else {
				SPI_ControllerTx_16bit_stream(0x0000);  // background color, white in this case
			}
		}
	}
	
	set(LCD_PORT, LCD_TFT_CS);    //set CS to high
}


/**************************************************************************//**
* @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
* @brief		Draw a line from and to a point with a color
* @note
*****************************************************************************/
void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
{
    short dx, dy, sx, sy, err, e2;

    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    sx = (x0 < x1) ? 1 : -1;
    sy = (y0 < y1) ? 1 : -1;
    err = dx - dy;

    while (1)
    {
	    LCD_drawPixel(x0, y0, c);

	    if (x0 == x1 && y0 == y1)
	    break;

	    e2 = 2 * err;
	    if (e2 > -dy)
	    {
		    err -= dy;
		    x0 += sx;
	    }
	    if (e2 < dx)
	    {
		    err += dx;
		    y0 += sy;
	    }
    }
}



/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
{
	// Set the address to the block area
	LCD_setAddr(x0, y0, x1, y1);

	clear(LCD_PORT, LCD_TFT_CS);    //CS pulled low to start communication

	// Fill the block with the color
	uint16_t x_pixels =x1 - x0 +1;
	uint16_t y_pixels =y1 - y0 +1;
	
	for (uint16_t i = 0; i < x_pixels; i++)
	{
		for (uint16_t j = 0; j < y_pixels; j++)
		{
			SPI_ControllerTx_16bit_stream(color);
		}
	}

	set(LCD_PORT, LCD_TFT_CS);    //set CS to high
}

/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color)
{
    // Set the address to the entire screen
    LCD_setAddr(0, 0, LCD_WIDTH, LCD_HEIGHT);
    
	clear(LCD_PORT, LCD_TFT_CS);	//CS pulled low to start communication
	
    // Fill the screen with the color
    for(uint16_t i = 0; i < LCD_WIDTH; i++)
    {
		 for(uint16_t j = 0; j < LCD_HEIGHT; j++)
		 {
			 SPI_ControllerTx_16bit_stream(color);
		 }
    }
	
	set(LCD_PORT, LCD_TFT_CS);	//set CS to high
}

/**************************************************************************//**
* @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
* @brief		Draw a string starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
{
    while (*str)
    {
	    LCD_drawChar(x, y, *str, fg, bg);
	    x += 6;
	    str++;
    }
}

