#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

// Section ::  libraries
#include "pico/stdlib.h"

/*!
	@brief  base Entity class for game objects
    @param  bitmap pointer to bitmap data
    @param  width width of bitmap
    @param  height height of bitmap
    @param  x x position of entity
    @param  y y position of entity
*/
class Entity
{
    // Section :: public members
    public:

    //constructor
    Entity(uint8_t *bitmap, int width, int height, int x, int y);

    //position
    int x;
    int y;
    float xVelocity;
    float yVelocity;
    void updatePosn(void);
    void setVelocity(float xVelocity, float yVelocity);
    void setPosn(int x, int y);


    //TODO: add multiple bitmaps for animation
    //appreance
    uint8_t *bitmap;
    int width;
    int height; 
    void draw(uint8_t *frameBuffer);


}; // end of Entity class 



#endif // __ENTITY_HPP__