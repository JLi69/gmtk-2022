class Bomb
{
	int mPosX, mPosY, 
		mMovementX, mMovementY;
public:
	int getX();
	int getY();
	
	void Move();

	//Constructor
	Bomb(int x, int y, int movX, int movY);
};
