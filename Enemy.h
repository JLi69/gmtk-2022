enum EnemyType
{
	SPIDER,
	DEMON,
	SLIME
};

class Enemy
{
	//position of the enemy	
	int mPosX, mPosY;
	EnemyType mEnemy;
public:	
	//Move the enemy by passing in how much the x and y values should change 
	void Move(int xVal, int yVal);

	int getX();
	int getY();
	EnemyType getType();

	//Default constructor
	Enemy();
	//Constructor
	Enemy(int x, int y, EnemyType type);
};
