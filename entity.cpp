#include "entity.hpp"
#include "display.hpp"
#include "pico/stdlib.h"


// Section :: public methods

/*!
	@brief  constructor for Entity class
    @param  bitmap pointer to bitmap data
    @param  width width of bitmap
    @param  height height of bitmap
    @param  x x position of entity
    @param  y y position of entity
*/
Entity::Entity(uint8_t *bitmap, int width, int height, int x, int y)
{
    this->bitmap = bitmap;
    this->width = width;
    this->height = height;
    this->x = x;
    this->y = y;

}

/*!
	@brief update position of entity based on velocity
    @note units of velocity are pixels per frame
*/
void Entity::updatePosn(void)
{
    //update position
    x += xVelocity;
    y += yVelocity;

    //TODO: check for collision with other entities
    //TODO: check for out of bounds, and change posn instead of velocity??, or fix so it can't be 
    //drawn out of bounds but can still move out of bounds

    //check for collision with screen edges
    // TODO: move this only to class that needs it
    if (x > 127 - width) {xVelocity *= -1;}
	if (x <= 0) {xVelocity *= -1;}
	if (y > 159 - height) {yVelocity *= -1;}
	if (y <= 0) {yVelocity *= -1;}
}


/*!
	@brief set velocity of entity
    @note units of velocity are pixels per frame
*/
void Entity::setVelocity(float xVelocity, float yVelocity)
{
    this->xVelocity = xVelocity;
    this->yVelocity = yVelocity;
}


/*!
	@brief set position of entity
    @note units of position are pixels, origin is top left corner, x increases to the right, y increases down
*/
void Entity::setPosn(int x, int y)
{
    this->x = x;
    this->y = y;
}

/*!
	@brief draw entity to frameBuffer
    @param *frameBuffer pointer to frameBuffer
*/
void Entity::draw(uint8_t *frameBuffer)
{
    frameBufferDrawBitmap(frameBuffer, bitmap, x, y, width, height);
}