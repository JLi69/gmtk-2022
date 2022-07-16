#include "Enemy.h"

void Enemy::Move(int xVal, int yVal)
{
	mPosX += xVal;
	mPosY += yVal;
}

int Enemy::getX()
{
	return mPosX;
}

int Enemy::getY()
{
	return mPosY;
}

Enemy::Enemy()
{
	mPosX = 0;
	mPosY = 0;
}

Enemy::Enemy(int x, int y, EnemyType type)
{
	mPosX = x;
	mPosY = y;
	mEnemy = type;
}
