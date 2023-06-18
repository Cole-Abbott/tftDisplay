// Section ::  libraries 
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "st7735/ST7735_TFT.hpp"
#include "st7735/ST7735_TFT_Bitmap_Data.hpp"

// Section :: Defines   
#define TEST_DELAY1 1000


// Section :: Globals 
ST7735_TFT myTFT;

// Section ::  Function Headers 
void Setup(void);  // setup + user options
void test1(void);  // bouncing ball with graphics library
void test2(void);  // bouncing ball with double buffering and bi-color
void test3(void);  // bouncing ball with double buffering and 16 bit color
void frameBufferDrawBiColorBitmap(uint8_t *frameBuffer, char *bitmap, int x, int y, int w, int h);
void frameBufferDrawBitmap(uint8_t *frameBuffer, uint8_t *bitmap, int x, int y, int w, int h);
void setPixel(uint8_t *frameBuffer, int x, int y, uint16_t color);
void frameBufferDrawChar(uint8_t *frameBuffer, char letter, int x0, int y0, uint16_t color);
void frameBufferDrawText(uint8_t *frameBuffer, char *text, int x, int y, uint16_t color);


//  Section ::  MAIN 
int main(void) 
{
	
	Setup();
	stdio_init_all(); // Initialize chosen serial port for FPS results
	test3();

	while(1){
		printf("done testing\r\n");
		sleep_ms(1000);
	}

}
// *** End OF MAIN **


//  Section ::  Function Space 

/*
	setup + user options
	32mhz SPI, 128x160 screen, hardware SPI
	GPIO pins used:
		18 = SCLK
		19 = SDIN
		12 = DC
		14 = CS
		13 = RST
*/
void Setup(void)
{

	TFT_MILLISEC_DELAY(TEST_DELAY1);
	stdio_init_all(); // Initialize chosen serial port
	TFT_MILLISEC_DELAY(TEST_DELAY1);
	printf("TFT Start\r\n");
	
//*************** USER OPTION 0 SPI_SPEED + TYPE ***********
	bool bhardwareSPI = true; // true for hardware spi, 
	
	if (bhardwareSPI == true) { // hw spi
		uint32_t TFT_SCLK_FREQ = 16000 ; // Spi freq in KiloHertz , 1000 = 1Mhz , max 62500
		myTFT.TFTInitSPIType(TFT_SCLK_FREQ, spi0); 
	} else { // sw spi
		myTFT.TFTInitSPIType(); 
	}
//**********************************************************

// ******** USER OPTION 1 GPIO *********
// NOTE if using Hardware SPI clock and data pins will be tied to 
// the chosen interface (eg Spi0 CLK=18 DIN=19)
	int8_t SDIN_TFT = 19; 
	int8_t SCLK_TFT = 18; 
	int8_t DC_TFT = 12;
	int8_t CS_TFT = 14 ;  
	int8_t RST_TFT = 13;
	myTFT.TFTSetupGPIO(RST_TFT, DC_TFT, CS_TFT, SCLK_TFT, SDIN_TFT);
//**********************************************************

// ****** USER OPTION 2 Screen Setup ****** 
	uint8_t OFFSET_COL = 0;  // 2, These offsets can be adjusted for any issues->
	uint8_t OFFSET_ROW = 0; // 3, with manufacture tolerance/defects
	uint16_t TFT_WIDTH = 128;// Screen width in pixels
	uint16_t TFT_HEIGHT = 160; // Screen height in pixels
	myTFT.TFTInitScreenSize(OFFSET_COL, OFFSET_ROW , TFT_WIDTH , TFT_HEIGHT);
// ******************************************

// ******** USER OPTION 3 PCB_TYPE  **************************
	myTFT.TFTInitPCBType(myTFT.TFT_ST7735S_Black); // pass enum,4 choices,see README
//**********************************************************
	
}

/*!
	@brief  Test 1: bouncing ball with fps counter using graphics library
	@note  Uses bitmap image

*/
void test1(void) {

	myTFT.TFTfillScreen(ST7735_BLACK);


	int x = 0;
	int y = 0;
	int xdir = 1;
	int ydir = 1;

	#define WIDTH 20
	#define HEIGHT 24

	int t0 = 0, t1 = 0, fps = 0, frameTime = 0;


	//int frameBuffer[128 * 160];
	
	for(int i = 0; i < 500; i++) {
		myTFT.TFTfillScreen(ST7735_BLACK);

		////erase bitmap in old position
		myTFT.TFTdrawBitmap(x, y, WIDTH, HEIGHT, ST7735_BLACK, ST7735_BLACK, (uint8_t*)pSmallImage);
		

		x = x + xdir;
		y = y + ydir;
		if (x > 128 - WIDTH) {xdir = -1;}
		if (x < 0) {xdir = 1;}
		if (y > 160 - HEIGHT) {ydir = -1;}
		if (y < 0) {ydir = 1;}

		//draw bitmap in new position
		myTFT.TFTdrawBitmap(x, y, WIDTH, HEIGHT, ST7735_RED, ST7735_BLACK, (uint8_t*)pSmallImage);


		//calc fps and frame time and print to screen
		t1 = to_ms_since_boot(get_absolute_time());
		frameTime = t1 - t0;
		fps = 1000 / frameTime;

		t0 = t1;

		char printStr[100];
		sprintf(printStr,"FPS = %d, FT = %d", fps, frameTime);
		myTFT.TFTdrawText(5, 5, printStr, ST7735_WHITE, ST7735_BLACK, 1);
		
		TFT_MILLISEC_DELAY(20);
	}
	
}



/*!
	@brief  Test 2: boucing ball with fps counter using bi-color double buffering

*/
void test2(void) {
	#define SCREEN_WIDTH 128
	#define SCREEN_HEIGHT 160


	// 'small image', 20x24px bitmap bi-colour horizontally addressed
	char ball[64] = {
		0, 0, 0, 0, 0, 0, 0, 0, //top row
		0, 0, 0, 1, 1, 0, 0, 0,
		0, 0, 1, 1, 1, 1, 0, 0,
		0, 1, 1, 1, 1, 1, 1, 0,
		0, 1, 1, 1, 1, 1, 1, 0,
		0, 0, 1, 1, 1, 1, 0, 0,
		0, 0, 0, 1, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0 ,0 //bottom row
	};

	//frame buffer, 128x160px bitmap bi-colour horizontally addressed, 1 bit per pixel, 2560 bytes
	uint8_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT / 8];

	memset(frameBuffer, 0, sizeof(frameBuffer));  //clear frame buffer

	//ball position and direction
	int x = 1;
	int y = 50;
	int xdir = 1;
	int ydir = 1;

	//for fps counter
	int t0 = 0, t1 = 0, fps = 0, frameTime = 0;

	while(1) {
		//clear frame buffer
		memset(frameBuffer, 0, sizeof(frameBuffer));

		//draw ball to frame buffer
		frameBufferDrawBiColorBitmap(frameBuffer, ball, x, y, 8, 8);

		//update ball position
		x = x + xdir;
		y = y + ydir;
		if (x > 128 - 8) {xdir = -1;}
		if (x < 0) {xdir = 1;}
		if (y > 160 - 8) {ydir = -1;}
		if (y < 0) {ydir = 1;}


		//draw frame buffer to screen
		myTFT.TFTdrawBitmap(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, ST7735_WHITE, ST7735_BLACK, frameBuffer); //draw frame buffer to screen

		//calc fps and frame time and print to screen
		t1 = to_ms_since_boot(get_absolute_time());
		frameTime = t1 - t0;
		fps = 1000 / frameTime;
		t0 = t1;
		char printStr[100];
		sprintf(printStr,"FPS = %d, FT = %d", fps, frameTime);	
		myTFT.TFTdrawText(5, 5, printStr, ST7735_WHITE, ST7735_BLACK, 1);

		//delay
		//TFT_MILLISEC_DELAY(100);
	}
}	

/*!
	@brief Draw a bitmap to the frame buffer
	@param frameBuffer Pointer to the frame buffer, uint8_t array, 1 bit per pixel
	@param bitmap Pointer to the bitmap, char array, 1 byte per pixel
	@param x X position of the bitmap
	@param y Y position of the bitmap
	@param w Width of the bitmap
	@param h Height of the 
	
	@note framebuffer is 128x160px bitmap bi-colour horizontally addressed, 1 bit per pixel, 2560 bytes
	@note bitmap is bitmap bi-colour horizontally addressed 1 byte per pixel

*/
void frameBufferDrawBiColorBitmap(uint8_t *frameBuffer, char *bitmap, int x, int y, int w, int h) {
	
	//loop through w, h and copy bitmap to frame buffer
	for(int i = 0; i < w; i++) { //loop through width
		for(int j = 0; j < h; j++) { //loop through height
			int buffIndex = ((x + i) + ((y + j) * 128)) / 8; //calculate index of frame buffer
			int shiftIndex = ((x + i) + ((y + j) * 128)) % 8; //calculate shift index

			if(bitmap[i + (j * w)] == 1) { //if pixel is set
				frameBuffer[buffIndex] |= (0b10000000 >> shiftIndex); //set pixel in frame buffer
			} else {
				//unset pixel in frame buffer
				frameBuffer[buffIndex] &= ~(0b10000000 >> shiftIndex);
			}

		}
	}

}



/*!
	@brief  Test 3: boucing ball with fps counter using 16-bit color double buffering

*/
void test3(void) {
	//frame buffer, 128x160px bitmap 16-bit color horizontally addressed, 2 bytes per pixel, 40960 bytes
	uint8_t frameBuffer[SCREEN_WIDTH * SCREEN_HEIGHT * 2];
	memset(frameBuffer, 0, sizeof(frameBuffer));  //clear frame buffer

	// ball, 8x8 bitmap 16-bit color horizontally addressed, red
	uint8_t ball[128] = {
    	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	};

	//ball position and direction
	int x = 1;
	int y = 50;
	int xdir = 1;
	int ydir = 1;

	//for fps counter
	int t0 = 0, t1 = 0, fps = 0, frameTime = 0;


	while(1) {
		//clear frame buffer
		memset(frameBuffer, 0, sizeof(frameBuffer));

		//draw ball to frame buffer
		frameBufferDrawBitmap(frameBuffer, ball, x, y, 8, 8);

		//update ball position
		x = x + xdir;
		y = y + ydir;
		if (x > 127 - 8) {xdir = -1;}
		if (x <= 0) {xdir = 1;}
		if (y > 159 - 8) {ydir = -1;}
		if (y <= 0) {ydir = 1;}

		//calc fps and frame time and print to screen
		t1 = to_ms_since_boot(get_absolute_time());
		frameTime = t1 - t0;
		fps = 1000 / frameTime;
		t0 = t1;
		char printStr[100];

		sprintf(printStr,"FPS = %d, FT = %d", fps, frameTime);
		frameBufferDrawText(frameBuffer, printStr, 5, 5, ST7735_WHITE);	



		//draw frame buffer to screen
		myTFT.TFTdrawBitmap16Data(0, 0, frameBuffer, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

}

/*!
	@brief Draw a bitmap to the frame buffer
	@param frameBuffer Pointer to the frame buffer, uint8_t array, 2 byte per pixel
	@param bitmap Pointer to the bitmap, uint_8 array, 2 byte per pixel
	@param x X position of the bitmap
	@param y Y position of the bitmap
	@param w Width of the bitmap
	@param h Height of the 
*/
void frameBufferDrawBitmap(uint8_t *frameBuffer, uint8_t *bitmap, int x, int y, int w, int h) {
	
	//loop through w, h and copy bitmap to frame buffer
	//loop over each pixel in the bitmap and copy the high and low byte to the frame buffer

	for(int i = 0; i < w; i++) { //loop through width
		for(int j = 0; j < h; j++) { //loop through height
			int buffIndex = ((x + i) + ((y + j) * 128)) * 2; //calculate index of frame buffer
			frameBuffer[buffIndex] = bitmap[(i + (j * w)) * 2]; //copy high byte
			frameBuffer[buffIndex + 1] = bitmap[(i + (j * w)) * 2 + 1]; //copy low byte
		}
	}
}


#define CHAR_WIDTH 5
#define CHAR_HEIGHT 8

/*!
	@brief Draw text to the frame buffer
	@param frameBuffer Pointer to the frame buffer, uint8_t array, 2 byte per pixel
	@param text pointer to char array
	@param x X position of the text
	@param y Y position of the text
	@param color Color of the text
*/
void frameBufferDrawText(uint8_t *frameBuffer, char *text, int x, int y, uint16_t color) {
	//loop through text and draw each character
	#define CHAR_WIDTH 5
	int i = 0;
	while(text[i] != '\0') {
		frameBufferDrawChar(frameBuffer, text[i], x, y , color);
		i++;
		x += CHAR_WIDTH;
	}
}

void frameBufferDrawChar(uint8_t *frameBuffer, char letter, int x0, int y0, uint16_t color) {
	
	// "default" 5 by 8 font pixel ASCII font 
	// Standard ASCII 5x7 font 
	static const unsigned char font[] = {
    0x00, 0x00, 0x00, 0x00, 0x00,
	0x3E, 0x5B, 0x4F, 0x5B, 0x3E,  // :)
	0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
	0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
	0x18, 0x3C, 0x7E, 0x3C, 0x18,
	0x1C, 0x57, 0x7D, 0x57, 0x1C,
	0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
	0x00, 0x18, 0x3C, 0x18, 0x00,
	0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
	0x00, 0x18, 0x24, 0x18, 0x00,
	0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
	0x30, 0x48, 0x3A, 0x06, 0x0E,
	0x26, 0x29, 0x79, 0x29, 0x26,
	0x40, 0x7F, 0x05, 0x05, 0x07,
	0x40, 0x7F, 0x05, 0x25, 0x3F,
	0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
	0x7F, 0x3E, 0x1C, 0x1C, 0x08,
	0x08, 0x1C, 0x1C, 0x3E, 0x7F,
	0x14, 0x22, 0x7F, 0x22, 0x14,
	0x5F, 0x5F, 0x00, 0x5F, 0x5F,
	0x06, 0x09, 0x7F, 0x01, 0x7F,
	0x00, 0x66, 0x89, 0x95, 0x6A,
	0x60, 0x60, 0x60, 0x60, 0x60,
	0x94, 0xA2, 0xFF, 0xA2, 0x94,
	0x08, 0x04, 0x7E, 0x04, 0x08,
	0x10, 0x20, 0x7E, 0x20, 0x10,
	0x08, 0x08, 0x2A, 0x1C, 0x08,
	0x08, 0x1C, 0x2A, 0x08, 0x08,
	0x1E, 0x10, 0x10, 0x10, 0x10,
	0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
	0x30, 0x38, 0x3E, 0x38, 0x30,
	0x06, 0x0E, 0x3E, 0x0E, 0x06,
	0x00, 0x00, 0x00, 0x00, 0x00,  // Space
	0x00, 0x00, 0x5F, 0x00, 0x00,  // !
	0x00, 0x07, 0x00, 0x07, 0x00,
	0x14, 0x7F, 0x14, 0x7F, 0x14,
	0x24, 0x2A, 0x7F, 0x2A, 0x12,
	0x23, 0x13, 0x08, 0x64, 0x62,
	0x36, 0x49, 0x56, 0x20, 0x50,
	0x00, 0x08, 0x07, 0x03, 0x00,
	0x00, 0x1C, 0x22, 0x41, 0x00,
	0x00, 0x41, 0x22, 0x1C, 0x00,
	0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
	0x08, 0x08, 0x3E, 0x08, 0x08,
	0x00, 0x80, 0x70, 0x30, 0x00,
	0x08, 0x08, 0x08, 0x08, 0x08,
	0x00, 0x00, 0x60, 0x60, 0x00,
	0x20, 0x10, 0x08, 0x04, 0x02,
	0x3E, 0x51, 0x49, 0x45, 0x3E,
	0x00, 0x42, 0x7F, 0x40, 0x00,
	0x72, 0x49, 0x49, 0x49, 0x46,
	0x21, 0x41, 0x49, 0x4D, 0x33,
	0x18, 0x14, 0x12, 0x7F, 0x10,
	0x27, 0x45, 0x45, 0x45, 0x39,
	0x3C, 0x4A, 0x49, 0x49, 0x31,
	0x41, 0x21, 0x11, 0x09, 0x07,
	0x36, 0x49, 0x49, 0x49, 0x36,
	0x46, 0x49, 0x49, 0x29, 0x1E,
	0x00, 0x00, 0x14, 0x00, 0x00,
	0x00, 0x40, 0x34, 0x00, 0x00,
	0x00, 0x08, 0x14, 0x22, 0x41,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x00, 0x41, 0x22, 0x14, 0x08,
	0x02, 0x01, 0x59, 0x09, 0x06,
	0x3E, 0x41, 0x5D, 0x59, 0x4E,
	0x7C, 0x12, 0x11, 0x12, 0x7C,
	0x7F, 0x49, 0x49, 0x49, 0x36,
	0x3E, 0x41, 0x41, 0x41, 0x22,
	0x7F, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x49, 0x49, 0x49, 0x41,
	0x7F, 0x09, 0x09, 0x09, 0x01,
	0x3E, 0x41, 0x41, 0x51, 0x73,
	0x7F, 0x08, 0x08, 0x08, 0x7F,
	0x00, 0x41, 0x7F, 0x41, 0x00,
	0x20, 0x40, 0x41, 0x3F, 0x01,
	0x7F, 0x08, 0x14, 0x22, 0x41,
	0x7F, 0x40, 0x40, 0x40, 0x40,
	0x7F, 0x02, 0x1C, 0x02, 0x7F,
	0x7F, 0x04, 0x08, 0x10, 0x7F,
	0x3E, 0x41, 0x41, 0x41, 0x3E,
	0x7F, 0x09, 0x09, 0x09, 0x06,
	0x3E, 0x41, 0x51, 0x21, 0x5E,
	0x7F, 0x09, 0x19, 0x29, 0x46,
	0x26, 0x49, 0x49, 0x49, 0x32,
	0x03, 0x01, 0x7F, 0x01, 0x03,
	0x3F, 0x40, 0x40, 0x40, 0x3F,
	0x1F, 0x20, 0x40, 0x20, 0x1F,
	0x3F, 0x40, 0x38, 0x40, 0x3F,
	0x63, 0x14, 0x08, 0x14, 0x63,
	0x03, 0x04, 0x78, 0x04, 0x03,
	0x61, 0x59, 0x49, 0x4D, 0x43,
	0x00, 0x7F, 0x41, 0x41, 0x41,
	0x02, 0x04, 0x08, 0x10, 0x20,
	0x00, 0x41, 0x41, 0x41, 0x7F,
	0x04, 0x02, 0x01, 0x02, 0x04,
    0x40, 0x40, 0x40, 0x40, 0x40,
    0x00, 0x03, 0x07, 0x08, 0x00,
	0x20, 0x54, 0x54, 0x78, 0x40,
	0x7F, 0x28, 0x44, 0x44, 0x38,
	0x38, 0x44, 0x44, 0x44, 0x28,
	0x38, 0x44, 0x44, 0x28, 0x7F,
	0x38, 0x54, 0x54, 0x54, 0x18,
	0x00, 0x08, 0x7E, 0x09, 0x02,
	0x18, 0xA4, 0xA4, 0x9C, 0x78,
	0x7F, 0x08, 0x04, 0x04, 0x78,
	0x00, 0x44, 0x7D, 0x40, 0x00,
	0x20, 0x40, 0x40, 0x3D, 0x00,
	0x7F, 0x10, 0x28, 0x44, 0x00,
	0x00, 0x41, 0x7F, 0x40, 0x00,
	0x7C, 0x04, 0x78, 0x04, 0x78,
	0x7C, 0x08, 0x04, 0x04, 0x78,
	0x38, 0x44, 0x44, 0x44, 0x38,
	0xFC, 0x18, 0x24, 0x24, 0x18,
	0x18, 0x24, 0x24, 0x18, 0xFC,
	0x7C, 0x08, 0x04, 0x04, 0x08,
	0x48, 0x54, 0x54, 0x54, 0x24,
	0x04, 0x04, 0x3F, 0x44, 0x24,
	0x3C, 0x40, 0x40, 0x20, 0x7C,
	0x1C, 0x20, 0x40, 0x20, 0x1C,
	0x3C, 0x40, 0x30, 0x40, 0x3C,
	0x44, 0x28, 0x10, 0x28, 0x44,
	0x4C, 0x90, 0x90, 0x90, 0x7C,
	0x44, 0x64, 0x54, 0x4C, 0x44,  // z
	0x00, 0x08, 0x36, 0x41, 0x00, // {
	0x00, 0x00, 0x77, 0x00, 0x00, // |
	0x00, 0x41, 0x36, 0x08, 0x00,  // }
	0x02, 0x01, 0x02, 0x04, 0x02, // capital letter c with cedilla
	0x3C, 0x26, 0x23, 0x26, 0x3C,
	0x1E, 0xA1, 0xA1, 0x61, 0x12,
	0x3A, 0x40, 0x40, 0x20, 0x7A,
	0x38, 0x54, 0x54, 0x55, 0x59,
	0x21, 0x55, 0x55, 0x79, 0x41,
	0x22, 0x54, 0x54, 0x78, 0x42, // a-umlaut
	0x21, 0x55, 0x54, 0x78, 0x40,
	0x20, 0x54, 0x55, 0x79, 0x40,
	0x0C, 0x1E, 0x52, 0x72, 0x12,
	0x39, 0x55, 0x55, 0x55, 0x59,
	0x39, 0x54, 0x54, 0x54, 0x59,
	0x39, 0x55, 0x54, 0x54, 0x58,
	0x00, 0x00, 0x45, 0x7C, 0x41,
	0x00, 0x02, 0x45, 0x7D, 0x42,
	0x00, 0x01, 0x45, 0x7C, 0x40,
	0x7D, 0x12, 0x11, 0x12, 0x7D, // A-umlaut
	0xF0, 0x28, 0x25, 0x28, 0xF0,
	0x7C, 0x54, 0x55, 0x45, 0x00,
	0x20, 0x54, 0x54, 0x7C, 0x54,
	0x7C, 0x0A, 0x09, 0x7F, 0x49,
	0x32, 0x49, 0x49, 0x49, 0x32,
	0x3A, 0x44, 0x44, 0x44, 0x3A, // o-umlaut
	0x32, 0x4A, 0x48, 0x48, 0x30,
	0x3A, 0x41, 0x41, 0x21, 0x7A,
	0x3A, 0x42, 0x40, 0x20, 0x78,
	0x00, 0x9D, 0xA0, 0xA0, 0x7D,
	0x3D, 0x42, 0x42, 0x42, 0x3D, // O-umlaut
	0x3D, 0x40, 0x40, 0x40, 0x3D,
	0x3C, 0x24, 0xFF, 0x24, 0x24,
	0x48, 0x7E, 0x49, 0x43, 0x66,
	0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
	0xFF, 0x09, 0x29, 0xF6, 0x20,
	0xC0, 0x88, 0x7E, 0x09, 0x03,
	0x20, 0x54, 0x54, 0x79, 0x41,
	0x00, 0x00, 0x44, 0x7D, 0x41,
	0x30, 0x48, 0x48, 0x4A, 0x32,
	0x38, 0x40, 0x40, 0x22, 0x7A,
	0x00, 0x7A, 0x0A, 0x0A, 0x72,
	0x7D, 0x0D, 0x19, 0x31, 0x7D,
	0x26, 0x29, 0x29, 0x2F, 0x28,
	0x26, 0x29, 0x29, 0x29, 0x26,
	0x30, 0x48, 0x4D, 0x40, 0x20,
	0x38, 0x08, 0x08, 0x08, 0x08,
	0x08, 0x08, 0x08, 0x08, 0x38,
	0x2F, 0x10, 0xC8, 0xAC, 0xBA,
	0x2F, 0x10, 0x28, 0x34, 0xFA,
	0x00, 0x00, 0x7B, 0x00, 0x00,
	0x08, 0x14, 0x2A, 0x14, 0x22,
	0x22, 0x14, 0x2A, 0x14, 0x08,
	0xAA, 0x00, 0x55, 0x00, 0xAA,
	0xAA, 0x55, 0xAA, 0x55, 0xAA,
	0x00, 0x00, 0x00, 0xFF, 0x00,
	0x10, 0x10, 0x10, 0xFF, 0x00,
	0x14, 0x14, 0x14, 0xFF, 0x00,
	0x10, 0x10, 0xFF, 0x00, 0xFF,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x14, 0x14, 0x14, 0xFC, 0x00,
	0x14, 0x14, 0xF7, 0x00, 0xFF,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x14, 0x14, 0xF4, 0x04, 0xFC,
	0x14, 0x14, 0x17, 0x10, 0x1F,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0x1F, 0x00,
	0x10, 0x10, 0x10, 0xF0, 0x00,
	0x00, 0x00, 0x00, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0x1F, 0x10,
	0x10, 0x10, 0x10, 0xF0, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0xFF, 0x10,
	0x00, 0x00, 0x00, 0xFF, 0x14,
	0x00, 0x00, 0xFF, 0x00, 0xFF,
	0x00, 0x00, 0x1F, 0x10, 0x17,
	0x00, 0x00, 0xFC, 0x04, 0xF4,
	0x14, 0x14, 0x17, 0x10, 0x17,
	0x14, 0x14, 0xF4, 0x04, 0xF4,
	0x00, 0x00, 0xFF, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x14, 0x14,
	0x14, 0x14, 0xF7, 0x00, 0xF7,
	0x14, 0x14, 0x14, 0x17, 0x14,
	0x10, 0x10, 0x1F, 0x10, 0x1F,
	0x14, 0x14, 0x14, 0xF4, 0x14,
	0x10, 0x10, 0xF0, 0x10, 0xF0,
	0x00, 0x00, 0x1F, 0x10, 0x1F,
	0x00, 0x00, 0x00, 0x1F, 0x14,
	0x00, 0x00, 0x00, 0xFC, 0x14,
	0x00, 0x00, 0xF0, 0x10, 0xF0,
	0x10, 0x10, 0xFF, 0x10, 0xFF,
	0x14, 0x14, 0x14, 0xFF, 0x14,
	0x10, 0x10, 0x10, 0x1F, 0x00,
	0x00, 0x00, 0x00, 0xF0, 0x10,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xFF, 0xFF, 0xFF, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xFF, 0xFF,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x38, 0x44, 0x44, 0x38, 0x44,
	0xFC, 0x4A, 0x4A, 0x4A, 0x34, // sharp-s or beta
	0x7E, 0x02, 0x02, 0x06, 0x06,
	0x02, 0x7E, 0x02, 0x7E, 0x02,
	0x63, 0x55, 0x49, 0x41, 0x63,
	0x38, 0x44, 0x44, 0x3C, 0x04,
	0x40, 0x7E, 0x20, 0x1E, 0x20,
	0x06, 0x02, 0x7E, 0x02, 0x02,
	0x99, 0xA5, 0xE7, 0xA5, 0x99,
	0x1C, 0x2A, 0x49, 0x2A, 0x1C,
	0x4C, 0x72, 0x01, 0x72, 0x4C,
	0x30, 0x4A, 0x4D, 0x4D, 0x30,
	0x30, 0x48, 0x78, 0x48, 0x30,
	0xBC, 0x62, 0x5A, 0x46, 0x3D,
	0x3E, 0x49, 0x49, 0x49, 0x00,
	0x7E, 0x01, 0x01, 0x01, 0x7E,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x44, 0x44, 0x5F, 0x44, 0x44,
	0x40, 0x51, 0x4A, 0x44, 0x40,
	0x40, 0x44, 0x4A, 0x51, 0x40,
	0x00, 0x00, 0xFF, 0x01, 0x03,
	0xE0, 0x80, 0xFF, 0x00, 0x00,
	0x08, 0x08, 0x6B, 0x6B, 0x08,
	0x36, 0x12, 0x36, 0x24, 0x36,
	0x06, 0x0F, 0x09, 0x0F, 0x06,
	0x00, 0x00, 0x18, 0x18, 0x00,
	0x00, 0x00, 0x10, 0x10, 0x00,
	0x30, 0x40, 0xFF, 0x01, 0x01,
	0x00, 0x1F, 0x01, 0x01, 0x1E,
	0x00, 0x19, 0x1D, 0x17, 0x12,
	0x00, 0x3C, 0x3C, 0x3C, 0x3C,
	0x00, 0x00, 0x00, 0x00, 0x00
};


	//loop through each pixel in the character
	for (uint8_t x = 0; x < CHAR_WIDTH; x++) {
		uint8_t line = font[(letter * CHAR_WIDTH) + x];
		for (uint8_t y = 0; y < CHAR_HEIGHT; y++, line >>= 1) {
			if (line & 1) {
				setPixel(frameBuffer, x + x0, y + y0, color);
			}
		}
	}
}
void setPixel(uint8_t *frameBuffer, int x, int y, uint16_t color)  {
	uint8_t hi = color >> 8;
	uint8_t lo = color & 0xFF;

	frameBuffer[(y * 128 + x) * 2] = hi;
	frameBuffer[(y * 128 + x) * 2 + 1] = lo;

}