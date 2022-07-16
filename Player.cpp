#include "Player.h"

void Player::Move(int xVal, int yVal)
{
	mPosX += xVal;
	mPosY += yVal;
}

int Player::getX()
{
	return mPosX;
}

int Player::getY()
{
	return mPosY;
}

Player::Player()
{
	mPosX = 0;
	mPosY = 0;
}

Player::Player(int x, int y)
{
	mPosX = x;
	mPosY = y;
}
