TFT Display library for Pi Pico C/C++ sdk using ST7735 TFT display
==================================================================

display.cpp:
wrapper for ST7735_TFT library by Gavin Lyons
adds double buffering to prevent flickering when drawing to screen frequently such as 
when animating a game

provides functions to init a tft display and draw to it through a frame buffer


entity.cpp
provides a base entity class that can be used easily with display.cpp


must make a build directory and run cmake .. from there
then run make to build the library
source files must be in the same directory as the CMakeLists.txt file
pico-sdk must be in the parent directory of the build directory