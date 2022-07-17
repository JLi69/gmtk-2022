#include "Bomb.h"

void Bomb::Move()
{
	mPosX += mMovementX;
	mPosY += mMovementY;
}

Bomb::Bomb(int x, int y, int movX, int movY)
{
	mPosX = x;
	mPosY = y;
	mMovementX = movX;
	mMovementY = movY;
}

int Bomb::getX()
{
	return mPosX;
}

int Bomb::getY()
{
	return mPosY;
}
