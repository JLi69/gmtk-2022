#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "Shader.h"
#include "Map.h"
#include <ctime>
#include "Player.h"
#include "Enemy.h"
#include <vector>
#include "Texture.h"
#include "Bomb.h"

#define DICE_COUNT 3

//This code could supply Olive Garden for 100 years

//Dice values
static int diceVals[DICE_COUNT] = { 0 };
//Dice rotation
static float diceRotation[DICE_COUNT] = { 0.0f };
static float diceRotationSpeed[DICE_COUNT] = { 0.0f }; 

//Map
static std::array<std::array<Tile, MAP_SIZE>, MAP_SIZE> map;
static std::array<std::array<int, MAP_SIZE>, MAP_SIZE> distToPlayer;
static std::array<std::array<bool, MAP_SIZE>, MAP_SIZE> visited;
//Player
static Player player;
static int highlightedX, highlightedY;
//Bombs
static std::vector<Bomb> bombs;
//Enemies
static std::vector<Enemy> enemies;

//Whether the player died
static bool died = false,
			menu = true;

//Do a flood fill to calculate the graph distance from the player in a cell on the grid
void updateDistToPlayer(int x, int y, int dist)
{	
	if(x < 0 || x >= MAP_SIZE || y < 0 || y >= MAP_SIZE)
		return;
	if(visited.at(x).at(y))
		return;	
	if(map.at(x).at(y) != EMPTY)
		return;
	visited.at(x).at(y) = true;	
	distToPlayer.at(x).at(y) = dist;
	updateDistToPlayer(x + 1, y, dist + 1);
	updateDistToPlayer(x - 1, y, dist + 1);
	updateDistToPlayer(x, y + 1, dist + 1);
	updateDistToPlayer(x, y - 1, dist + 1);
}

void updateEnemies()
{
	for(int i = 0; i < MAP_SIZE; i++)
		for(int j = 0; j < MAP_SIZE; j++)
		{
			visited.at(i).at(j) = false;
			distToPlayer.at(i).at(j) = -1;	
		}

	updateDistToPlayer(player.getX(), player.getY(), 0);

	for(auto &enemy : enemies)
	{
		int dirX = 0, dirY = 0;
		int minDist = MAP_SIZE * MAP_SIZE + 1;
		for(int offsetX = -1; offsetX <= 1; offsetX++)
			for(int offsetY = -1; offsetY <= 1; offsetY++)
			{
				if(offsetX * offsetX == offsetY * offsetY)
					continue;
				if(enemy.getX() + offsetX < 0 || enemy.getX() + offsetX >= MAP_SIZE || enemy.getY() + offsetY < 0 || enemy.getY() + offsetY >= MAP_SIZE)
					continue;
				
				bool skip = false;
				for(auto enemy2 : enemies)
					if(enemy.getX() + offsetX == enemy2.getX() && enemy.getY() + offsetY == enemy2.getY())
						skip = true;
				if(skip)
					continue;
				
				if(distToPlayer.at(enemy.getX() + offsetX).at(enemy.getY() + offsetY) == -1)
					continue;
				if(minDist > distToPlayer.at(enemy.getX() + offsetX).at(enemy.getY() + offsetY))
				{
					minDist = distToPlayer.at(enemy.getX() + offsetX).at(enemy.getY() + offsetY);
					dirX = offsetX;
					dirY = offsetY;
				}	
			}
		enemy.Move(dirX, dirY);
	}
}

//Handle window resizing
void onWinResize(GLFWwindow *win, int newWidth, int newHeight)
{
	if(newWidth <= newHeight)
		glViewport(0, newHeight / 2 - newWidth / 2, newWidth, newWidth);
	else if(newWidth > newHeight)
		glViewport(newWidth / 2  - newHeight / 2, 0, newHeight, newHeight);
}

void handleKeyInput(GLFWwindow *win, int key, int scancode, int action, int mods)
{
	if(died && key == GLFW_KEY_R)
	{
		died = false;	

		//Reset the dice
		for(int i = 0; i < DICE_COUNT; i++)
			diceVals[i] = 0;

		//Generate the map	
		genMap(map);
		//Player
		player = Player(rand() % MAP_SIZE, rand() % MAP_SIZE);
		map.at(player.getX()).at(player.getY()) = EMPTY;
		highlightedX = player.getX() + 1;
		highlightedY = player.getY();
		//Place the diamond	
		int diamondX = -1, diamondY = -1;		
		//Don't have the diamond be within 2 blocks of the player	
		do
		{
			diamondX = rand() % MAP_SIZE;
			diamondY = rand() % MAP_SIZE;
		} while((diamondX - player.getX()) * (diamondX - player.getX()) <= 4 && (diamondY - player.getY()) * (diamondY - player.getY()) <= 4); 
		//Enemies		
		enemies.clear();		
		//Create the enemies
		for(int i = 0; i < MAP_SIZE; i++)
		{	
			for(int j = 0; j < MAP_SIZE; j++)
			{	
				//Don't put an enemy on top of the player
				if((j - player.getY()) * (j - player.getY()) <= 1 ||
					(i - player.getX()) * (i - player.getX()) <= 1 ||
					(i == diamondX && j == diamondY))
					continue;

				//If it is an empty spot, put an enemy there 10% of the time	
				if(map.at(i).at(j) == EMPTY && rand() % 10 == 0)
					enemies.push_back(Enemy(i, j, (EnemyType)(rand() % 3)));
				else if((diamondX - i) * (diamondX - i) <= 4 && (diamondY - j) * (diamondY - j) <= 4 && rand() % 3 == 0)
				{
					enemies.push_back(Enemy(i, j, (EnemyType)(rand() % 3)));
					map.at(i).at(j) = EMPTY;	
				}	
			}
		}
	}

	if(menu && key == GLFW_KEY_ENTER)
		menu = false;

	if(died || menu) return;

	if(key == GLFW_KEY_SPACE)
	{		
		bool rolled = 0;	
		for(int i = 0; i < DICE_COUNT; i++)
		{		
			if(diceVals[i] != 0)
				continue;
			diceVals[i] = rand() % 6 + 1;
			diceRotationSpeed[i] = 12.0f;	
			rolled = true;	
		}	
		
		if(rolled)
			updateEnemies();
	}

	//Check if the player selected any of the dice
	static int keys[DICE_COUNT] = { GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3 };
	if(action == GLFW_PRESS)
	{
		for(int i = 0; i < DICE_COUNT; i++)
		{
			if(key == keys[i])
			{
				if(diceVals[i] == 0)
					continue;

				//Update the board
				for(auto &bombObj : bombs)
					bombObj.Move();		

				int movementX, movementY;
				switch(diceVals[i])
				{
				//Move the player
				case 1:	
					movementX = -1;
					movementY = 0;

					highlightedX = player.getX() - 1;
					highlightedY = player.getY();
					break;
				case 2:  
					movementX = 1;
					movementY = 0;
					
					highlightedX = player.getX() + 1;
					highlightedY = player.getY();
					break;
				case 3:  
					movementX = 0;
					movementY = -1;
				
					highlightedX = player.getX();
					highlightedY = player.getY() - 1;	
					break;
				case 4: 	
					movementX = 0;
					movementY = 1;
					
					highlightedX = player.getX();
					highlightedY = player.getY() + 1;	
					break;
				//Mine a block
				case 5:
					if(highlightedX < 0 || highlightedX >= MAP_SIZE || highlightedY < 0 || highlightedY >= MAP_SIZE)
						break;
					map.at(highlightedX).at(highlightedY) = EMPTY;
					//Kill any enemies with pickaxe
					for(auto &enemy : enemies)
						if(enemy.getX() == highlightedX && enemy.getY() == highlightedY)
							enemy.Move(1000, 1000); //Move enemy off the screen
					break;
				//Throw a bomb	
				case 6:
					bombs.push_back(Bomb(highlightedX, highlightedY, highlightedX - player.getX(), highlightedY - player.getY()));
					break;
				}
				
				//Move the player
				player.Move(movementX, movementY);
				highlightedX += movementX;
				highlightedY += movementY;

				//Check for any collisions with the wall
				if(player.getX() < 0 || player.getX() >= MAP_SIZE || player.getY() < 0 || player.getY() >= MAP_SIZE)
				{	
					player.Move(-movementX, -movementY);
					highlightedX -= movementX;
					highlightedY -= movementY;
				}	
				else if(map.at(player.getX()).at(player.getY()) != EMPTY)
				{	
					player.Move(-movementX, -movementY);
					highlightedX -= movementX;
					highlightedY -= movementY;	
				}
				
				//Explode any bombs
				bool explodedAll = false;
				while(!explodedAll)
				{
					explodedAll = true;
					for(auto it = bombs.begin(); it != bombs.end(); it++)
					{
						Bomb temp = *it;
						
						bool hitEnemy = false;
						for(auto enemy : enemies)
							if(temp.getX() == enemy.getX() && temp.getY() == enemy.getY())
								hitEnemy = true;

						if(temp.getX() < 0 || temp.getX() >= MAP_SIZE || temp.getY() < 0 || temp.getY() >= MAP_SIZE)
						{
							//Explode!
							for(int j = temp.getX() - 3; j <= temp.getX() + 3; j++)
								for(int k = temp.getY() - 3; k <= temp.getY() + 3; k++)
								{
									if(j < 0 || j >= MAP_SIZE || k < 0 || k >= MAP_SIZE)
										continue;	
									else if((temp.getX() - j) * (temp.getX() - j) + (temp.getY() - k) * (temp.getY() - k) <= 9)
										map.at(j).at(k) = EMPTY;
								}	

							bombs.erase(it);
							explodedAll = false;	
							break;
						}
						else if(map.at(temp.getX()).at(temp.getY()) != EMPTY || hitEnemy)
						{
							//Explode!
							for(int j = temp.getX() - 3; j <= temp.getX() + 3; j++)
								for(int k = temp.getY() - 3; k <= temp.getY() + 3; k++)
								{
									if(j < 0 || j >= MAP_SIZE || k < 0 || k >= MAP_SIZE)
										continue;	
									else if(map.at(j).at(k) == STONE)
										continue;	
									else if((temp.getX() - j) * (temp.getX() - j) + (temp.getY() - k) * (temp.getY() - k) <= 9)
										map.at(j).at(k) = EMPTY;
								}
							
							//Kill any enemies
							for(auto &enemy : enemies)
								if((enemy.getX() - temp.getX()) * (enemy.getX() - temp.getX()) + (enemy.getY() - temp.getY()) * (enemy.getY() - temp.getY()) <= 9)
									enemy.Move(1000, 1000); //Just move them off the screen

							bombs.erase(it);
							explodedAll = false;	
							break;
						}
					}
				}

				//Reset the dice
				diceVals[i] = 0;		
			}	
		}	
	
		//Move around with the arrow keys
		if(key == GLFW_KEY_UP || key == GLFW_KEY_DOWN || key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)
		{	
			updateEnemies();
			//Update the board
			for(auto &bombObj : bombs)
				bombObj.Move();	

			int movementX, movementY;			

			switch(key)
			{
			case GLFW_KEY_UP:
				movementX = 0;
				movementY = 1;

				highlightedX = player.getX();
				highlightedY = player.getY() + 1;	
				break;
			case GLFW_KEY_DOWN:
				movementX = 0;
				movementY = -1;

				highlightedX = player.getX();
				highlightedY = player.getY() - 1;	
				break;
			case GLFW_KEY_LEFT:
				movementX = -1;
				movementY = 0;

				highlightedX = player.getX() - 1;
				highlightedY = player.getY();	
				break;
			case GLFW_KEY_RIGHT:
				movementX = 1;
				movementY = 0;

				highlightedX = player.getX() + 1;
				highlightedY = player.getY();	
				break;
			}
		
			//Move the player
			player.Move(movementX, movementY);
			highlightedX += movementX;
			highlightedY += movementY;

			//Check for any collisions with the wall
			if(player.getX() < 0 || player.getX() >= MAP_SIZE || player.getY() < 0 || player.getY() >= MAP_SIZE)
			{	
				player.Move(-movementX, -movementY);
				highlightedX -= movementX;
				highlightedY -= movementY;
			}	
			else if(map.at(player.getX()).at(player.getY()) != EMPTY)
			{	
				player.Move(-movementX, -movementY);
				highlightedX -= movementX;
				highlightedY -= movementY;	
			}
		
			//Explode any bombs
			bool explodedAll = false;
			while(!explodedAll)
			{
				explodedAll = true;
				for(auto it = bombs.begin(); it != bombs.end(); it++)
				{
					Bomb temp = *it;
				
					//Hit an enemy
					bool hitEnemy = false;
					for(auto enemy : enemies)
						if(temp.getX() == enemy.getX() && temp.getY() == enemy.getY())
							hitEnemy = true;

					if(temp.getX() < 0 || temp.getX() >= MAP_SIZE || temp.getY() < 0 || temp.getY() >= MAP_SIZE)
					{
						//Explode!
						for(int j = temp.getX() - 3; j <= temp.getX() + 3; j++)
							for(int k = temp.getY() - 3; k <= temp.getY() + 3; k++)
							{
								if(j < 0 || j >= MAP_SIZE || k < 0 || k >= MAP_SIZE)
									continue;	
								else if(map.at(j).at(k) == STONE)
									continue;
								else if((temp.getX() - j) * (temp.getX() - j) + (temp.getY() - k) * (temp.getY() - k) <= 9)
									map.at(j).at(k) = EMPTY;
							}	

						bombs.erase(it);
						explodedAll = false;	
						break;
					}
					else if(map.at(temp.getX()).at(temp.getY()) != EMPTY || hitEnemy)
					{
						//Explode!
						for(int j = temp.getX() - 3; j <= temp.getX() + 3; j++)
							for(int k = temp.getY() - 3; k <= temp.getY() + 3; k++)
							{
								if(j < 0 || j >= MAP_SIZE || k < 0 || k >= MAP_SIZE)
									continue;	
								else if((temp.getX() - j) * (temp.getX() - j) + (temp.getY() - k) * (temp.getY() - k) <= 9)
									map.at(j).at(k) = EMPTY;
							}

						//Kill any enemies
						for(auto &enemy : enemies)
							if((enemy.getX() - temp.getX()) * (enemy.getX() - temp.getX()) + (enemy.getY() - temp.getY()) * (enemy.getY() - temp.getY()) <= 9)
								enemy.Move(1000, 1000); //Just move them off the screen

						bombs.erase(it);
						explodedAll = false;	
						break;
					}
				}
			}
		}	
	}
}

void handleMouseClick(GLFWwindow *win, int button, int action, int mods)
{
	if(died) return;	
	
	if(button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		bool rolled = false;	
		for(int i = 0; i < DICE_COUNT; i++)
		{	
			if(diceVals[i] != 0)
				continue;
			diceRotationSpeed[i] = 12.0f;	
			diceVals[i] = rand() % 6 + 1;	
			rolled = true;	
		}	

		if(rolled)
			updateEnemies();
	}
	else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		//Click on the dice
		double mouseX, mouseY;
		glfwGetCursorPos(win, &mouseX, &mouseY);
		int winWidth, winHeight;
		glfwGetWindowSize(win, &winWidth, &winHeight);
		

		//Normalize the mouse position	
		if(winWidth < winHeight)	
		{	
			mouseY += ((double)winWidth / 2.0 - (double)winHeight / 2.0);	
			mouseX /= (double)winWidth;
			mouseY /= (double)winWidth;		
		}	
		else if(winWidth >= winHeight)
		{
			mouseX += ((double)winHeight / 2.0 - (double)winWidth / 2.0);	
			mouseX /= (double)winHeight;
			mouseY /= (double)winHeight;		
		}
		mouseX *= 2.0;
		mouseY *= 2.0;
		mouseX -= 1.0;
		mouseY -= 1.0;	
		mouseY *= -1.0;		
		
		for(int i = 0; i < DICE_COUNT; i++)
		{
			//Clicked on the dice	
			if((double)i * 0.1 * 2.0 - 0.2 - 0.08 < mouseX &&
				(double)i * 0.1 * 2.0 - 0.2 + 0.08 > mouseX &&
				-0.9 - 0.08 < mouseY &&
				-0.9 + 0.08 > mouseY)
			{
				if(diceVals[i] == 0)
					return;
			
				//Update the board
				for(auto &bombObj : bombs)
					bombObj.Move();		

				int movementX, movementY;
				switch(diceVals[i])
				{
				//Move the player
				case 1:	
					movementX = -1;
					movementY = 0;

					highlightedX = player.getX() - 1;
					highlightedY = player.getY();
					break;
				case 2:  
					movementX = 1;
					movementY = 0;
					
					highlightedX = player.getX() + 1;
					highlightedY = player.getY();
					break;
				case 3:  
					movementX = 0;
					movementY = -1;
				
					highlightedX = player.getX();
					highlightedY = player.getY() - 1;	
					break;
				case 4: 	
					movementX = 0;
					movementY = 1;
					
					highlightedX = player.getX();
					highlightedY = player.getY() + 1;	
					break;
				//Mine a block
				case 5:
					if(highlightedX < 0 || highlightedX >= MAP_SIZE || highlightedY < 0 || highlightedY >= MAP_SIZE)
						break;
					map.at(highlightedX).at(highlightedY) = EMPTY;
					//Kill any enemies with pickaxe
					for(auto &enemy : enemies)
						if(enemy.getX() == highlightedX && enemy.getY() == highlightedY)
							enemy.Move(1000, 1000); //Move enemy off the screen
					break;
				//Throw a bomb	
				case 6:
					bombs.push_back(Bomb(highlightedX, highlightedY, highlightedX - player.getX(), highlightedY - player.getY()));
					break;
				}
				
				//Move the player
				player.Move(movementX, movementY);
				highlightedX += movementX;
				highlightedY += movementY;

				//Check for any collisions with the wall
				if(player.getX() < 0 || player.getX() >= MAP_SIZE || player.getY() < 0 || player.getY() >= MAP_SIZE)
				{	
					player.Move(-movementX, -movementY);
					highlightedX -= movementX;
					highlightedY -= movementY;
				}	
				else if(map.at(player.getX()).at(player.getY()) != EMPTY)
				{	
					player.Move(-movementX, -movementY);
					highlightedX -= movementX;
					highlightedY -= movementY;	
				}
				
				//Explode any bombs
				bool explodedAll = false;
				while(!explodedAll)
				{
					explodedAll = true;
					for(auto it = bombs.begin(); it != bombs.end(); it++)
					{
						Bomb temp = *it;
						
						bool hitEnemy = false;
						for(auto enemy : enemies)
							if(temp.getX() == enemy.getX() && temp.getY() == enemy.getY())
								hitEnemy = true;

						if(temp.getX() < 0 || temp.getX() >= MAP_SIZE || temp.getY() < 0 || temp.getY() >= MAP_SIZE)
						{
							//Explode!
							for(int j = temp.getX() - 3; j <= temp.getX() + 3; j++)
								for(int k = temp.getY() - 3; k <= temp.getY() + 3; k++)
								{
									if(j < 0 || j >= MAP_SIZE || k < 0 || k >= MAP_SIZE)
										continue;	
									else if((temp.getX() - j) * (temp.getX() - j) + (temp.getY() - k) * (temp.getY() - k) <= 9)
										map.at(j).at(k) = EMPTY;
								}	

							bombs.erase(it);
							explodedAll = false;	
							break;
						}
						else if(map.at(temp.getX()).at(temp.getY()) != EMPTY || hitEnemy)
						{
							//Explode!
							for(int j = temp.getX() - 3; j <= temp.getX() + 3; j++)
								for(int k = temp.getY() - 3; k <= temp.getY() + 3; k++)
								{
									if(j < 0 || j >= MAP_SIZE || k < 0 || k >= MAP_SIZE)
										continue;	
									else if(map.at(j).at(k) == STONE)
										continue;	
									else if((temp.getX() - j) * (temp.getX() - j) + (temp.getY() - k) * (temp.getY() - k) <= 9)
										map.at(j).at(k) = EMPTY;
								}
							
							//Kill any enemies
							for(auto &enemy : enemies)
								if((enemy.getX() - temp.getX()) * (enemy.getX() - temp.getX()) + (enemy.getY() - temp.getY()) * (enemy.getY() - temp.getY()) <= 9)
									enemy.Move(1000, 1000); //Just move them off the screen

							bombs.erase(it);
							explodedAll = false;	
							break;
						}
					}
				}
				diceVals[i] = 0;
			}	
		}
	}
}

int main()
{
	srand(time(NULL));	

	//Set up glfw	
	if(!glfwInit())
		return -1;
	GLFWwindow* win = glfwCreateWindow(800, 800, "GMTK 2022", NULL, NULL);
	if(!win)
	{
		glfwTerminate();	
		return -1;
	}
	glfwMakeContextCurrent(win);
	glfwSetWindowSizeCallback(win, onWinResize);
	glfwSetKeyCallback(win, handleKeyInput);	
	glfwSetMouseButtonCallback(win, handleMouseClick);	
	//initialize glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		return -1;
	}

	//Rectangle
	float rect[] =
	{
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.0f, 1.0f,
		
		1.0f, 1.0f,
		1.0f, -1.0f,
		-1.0f, -1.0f
	};
	unsigned int id;
	glGenBuffers(1, &id);
	glBindBuffer(GL_ARRAY_BUFFER, id);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(rect), rect, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)0);	
	glEnableVertexAttribArray(0);
	//texture coordinates
	float rectTexCoords[] =
	{
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};
	unsigned int tcId;
	glGenBuffers(1, &tcId);
	glBindBuffer(GL_ARRAY_BUFFER, tcId);	
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectTexCoords), rectTexCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 2 * sizeof(float), (void*)0);	
	glEnableVertexAttribArray(1);

	//shaders
	Shader simple("res/shaders/simple-vert.glsl", "res/shaders/simple-frag.glsl");	
	//textures
	Texture dirt("res/textures/dirt.png"),
			stone("res/textures/stone.png"),
			empty("res/textures/empty.png"),
			playerTex("res/textures/player.png"),	
			slimeDice("res/textures/slime.png"),
			spiderDice("res/textures/spider.png"),
			demonDice("res/textures/demon.png"),
			diamond("res/textures/diamond.png"),
			highlight("res/textures/outline.png"),
			bomb("res/textures/bomb2.png"),
			death("res/textures/death.png"),
			title("res/textures/title.png");
	Texture diceFaces[7] = 
	{
		Texture("res/textures/dice.png"),	
		Texture("res/textures/left.png"),
		Texture("res/textures/right.png"),
		Texture("res/textures/down.png"),
		Texture("res/textures/up.png"),		
		Texture("res/textures/mine.png"),	
		Texture("res/textures/bomb.png"),	
	};

	//Generate the map	
	genMap(map);
	//Player
	player = Player(rand() % MAP_SIZE, rand() % MAP_SIZE);
	map.at(player.getX()).at(player.getY()) = EMPTY;
	highlightedX = player.getX() + 1;
	highlightedY = player.getY();
	//Place the diamond	
	int diamondX = -1, diamondY = -1;		
	//Don't have the diamond be within 2 blocks of the player	
	do
	{
		diamondX = rand() % MAP_SIZE;
		diamondY = rand() % MAP_SIZE;
	} while((diamondX - player.getX()) * (diamondX - player.getX()) <= 4 && (diamondY - player.getY()) * (diamondY - player.getY()) <= 4); 
	//Enemies		
	//Create the enemies
	for(int i = 0; i < MAP_SIZE; i++)
	{	
		for(int j = 0; j < MAP_SIZE; j++)
		{	
			//Don't put an enemy on top of the player
			if((j - player.getY()) * (j - player.getY()) <= 1 ||
				(i - player.getX()) * (i - player.getX()) <= 1 ||
				(i == diamondX && j == diamondY))
				continue;

			//If it is an empty spot, put an enemy there 10% of the time	
			if(map.at(i).at(j) == EMPTY && rand() % 10 == 0)
				enemies.push_back(Enemy(i, j, (EnemyType)(rand() % 3)));
			else if((diamondX - i) * (diamondX - i) <= 4 && (diamondY - j) * (diamondY - j) <= 4 && rand() % 3 == 0)
			{
				enemies.push_back(Enemy(i, j, (EnemyType)(rand() % 3)));
				map.at(i).at(j) = EMPTY;	
			}	
		}
	}

	//Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	simple.Use();	
	while(!glfwWindowShouldClose(win))
	{	
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		//Draw the title screen
		if(menu)
		{
			title.Activate();
			
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glUniform1f(simple.getUniformLocation("uRot"), 0.0f);
			glUniform1f(simple.getUniformLocation("uScale"), 1.0f);	
			glUniform2f(simple.getUniformLocation("uPos"), 0.0f, 0.0f);	
			glfwSwapBuffers(win);
			glfwPollEvents();
			continue;
		}
	
		//Draw the map	

		glUniform1f(simple.getUniformLocation("uRot"), 0.0f);
		glUniform1f(simple.getUniformLocation("uScale"), 0.04f);
		for(int i = 0; i < MAP_SIZE; i++)
		{	
			for(int j = 0; j < MAP_SIZE; j++)
			{ 
				switch(map.at(i).at(j))
				{
				case EMPTY: empty.Activate(); break;
				case DIRT: dirt.Activate(); break;
				case STONE: stone.Activate(); break;
				}

				glUniform2f(simple.getUniformLocation("uPos"), (float)i * 0.04f * 2.0f - 0.8f + 0.04f, (float)j * 0.04f * 2.0f - 0.8f + 0.04f);			
				glDrawArrays(GL_TRIANGLES, 0, 6);		
			}	
		}

		//Draw the player
		playerTex.Activate();	
		glUniform2f(simple.getUniformLocation("uPos"), (float)player.getX() * 0.04f * 2.0f - 0.8f + 0.04f, (float)player.getY() * 0.04f * 2.0f - 0.8f + 0.04f);	
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Draw the highlighted square
		highlight.Activate();
		glUniform2f(simple.getUniformLocation("uPos"), (float)highlightedX * 0.04f * 2.0f - 0.8f + 0.04f, (float)highlightedY * 0.04f * 2.0f - 0.8f + 0.04f); 
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Draw the enemies	
		for(auto &enemy : enemies)
		{
			switch(enemy.getType())
			{
			case SLIME: slimeDice.Activate(); break; 
			case SPIDER: spiderDice.Activate(); break;
			case DEMON: demonDice.Activate(); break;
			}

			glUniform2f(simple.getUniformLocation("uPos"), (float)enemy.getX() * 0.04f * 2.0f - 0.8f + 0.04f, (float)enemy.getY() * 0.04f * 2.0f - 0.8f + 0.04f);	
			glDrawArrays(GL_TRIANGLES, 0, 6);	
		}
		
		//Draw the bomb
		bomb.Activate();
		for(auto bombObj : bombs)
		{
			glUniform2f(simple.getUniformLocation("uPos"), (float)bombObj.getX() * 0.04f * 2.0f - 0.8f + 0.04f, (float)bombObj.getY() * 0.04f * 2.0f - 0.8f + 0.04f);	
			glDrawArrays(GL_TRIANGLES, 0, 6);	
		}

		//Draw the diamond
		diamond.Activate();	
		glUniform2f(simple.getUniformLocation("uPos"), (float)diamondX * 0.04f * 2.0f - 0.8f + 0.04f, (float)diamondY * 0.04f * 2.0f - 0.8f + 0.04f);  	
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Draw the dice			
		glUniform1f(simple.getUniformLocation("uScale"), 0.08f);	
		for(int i = 0; i < 3; i++)
		{	
			diceFaces[diceVals[i]].Activate();	

			//Rotate the dice
			diceRotation[i] += diceRotationSpeed[i];	
			if(diceRotation[i] >= 360.0f)
			{
				diceRotation[i] = 0.0f;
				diceRotationSpeed[i] = 0.0f;
			}
			glUniform1f(simple.getUniformLocation("uRot"), diceRotation[i]);

			glUniform2f(simple.getUniformLocation("uPos"), (float)i * 0.1f * 2.0f - 0.2f, -0.9f);	
			glDrawArrays(GL_TRIANGLES, 0, 6);	
		}

		//Check if the player beat the level
		if(player.getX() == diamondX && player.getY() == diamondY)
		{
			//Regenerate the map
			//Generate the map	
			genMap(map);
			//Player
			player = Player(rand() % MAP_SIZE, rand() % MAP_SIZE);
			map.at(player.getX()).at(player.getY()) = EMPTY;
			highlightedX = player.getX() + 1;
			highlightedY = player.getY();
		
			diamondX = -1, diamondY = -1;		
			//Don't have the diamond be within 2 blocks of the player	
			do
			{
				diamondX = rand() % MAP_SIZE;
				diamondY = rand() % MAP_SIZE;
			} while((diamondX - player.getX()) * (diamondX - player.getX()) <= 4 && (diamondY - player.getY()) * (diamondY - player.getY()) <= 4);
		
			enemies.clear();
			//Create the enemies
			for(int i = 0; i < MAP_SIZE; i++)
			{	
				for(int j = 0; j < MAP_SIZE; j++)
				{	
					//Don't put an enemy on top of the player
					if((j - player.getY()) * (j - player.getY()) <= 1 ||
						(i - player.getX()) * (i - player.getX()) <= 1 ||
						(i == diamondX && j == diamondY))
						continue;

					//If it is an empty spot, put an enemy there 10% of the time	
					if(map.at(i).at(j) == EMPTY && rand() % 10 == 0)
						enemies.push_back(Enemy(i, j, (EnemyType)(rand() % 3)));	
				}
			}
		}
		
		//Check if the player died
		for(auto enemy : enemies)
		{
			if(enemy.getX() == player.getX() && enemy.getY() == player.getY())
				died = true;	
		}
		if(died)
		{
			death.Activate();	
			glUniform1f(simple.getUniformLocation("uRot"), 0.0f);	
			glUniform1f(simple.getUniformLocation("uScale"), 1.0f);	
			glUniform2f(simple.getUniformLocation("uPos"), 0.0f, 0.0f);	
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		//GLFW stuff
		glfwPollEvents();
		glfwSwapBuffers(win);
	}

	glfwTerminate();
}
