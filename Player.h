class Player
{
	//position of the player	
	int mPosX, mPosY;
public:	
	//Move the player by passing in how much the x and y values should change 
	void Move(int xVal, int yVal);

	int getX();
	int getY();

	//Default constructor
	Player();
	//Constructor
	Player(int x, int y);
};
