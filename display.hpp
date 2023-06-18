#ifndef __DISPLAY_HPP__
#define __DISPLAY_HPP__

// Section ::  libraries 
#include "pico/stdlib.h"

// Section ::  defines
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160


//Function prototypes

void Setup(void);  // setup + user options
void frameBufferDrawBitmap(uint8_t *frameBuffer, uint8_t *bitmap, int x, int y, int w, int h);
void setPixel(uint8_t *frameBuffer, int x, int y, uint16_t color);
void frameBufferDrawChar(uint8_t *frameBuffer, char letter, int x0, int y0, uint16_t color);
void frameBufferDrawText(uint8_t *frameBuffer, char *text, int x, int y, uint16_t color);
void updateScreen(uint8_t *frameBuffer);
void clearScreen(uint8_t *frameBuffer);


#endif