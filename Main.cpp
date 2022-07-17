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
//Whether the player rolled the dice yet
static bool rolled = false;

//Map
static std::array<std::array<Tile, MAP_SIZE>, MAP_SIZE> map;
//Player
static Player player;
static int highlightedX, highlightedY;
//Bombs
static std::vector<Bomb> bombs;

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
	if(key == GLFW_KEY_SPACE && !rolled)
	{
		for(int i = 0; i < DICE_COUNT; i++)
		{	
			if(diceVals[i] != 0)
				continue;
			diceVals[i] = rand() % 6 + 1;
		}	
		rolled = true;
	}

	//Check if the player selected any of the dice
	static int keys[DICE_COUNT] = { GLFW_KEY_1, GLFW_KEY_2, GLFW_KEY_3 };
	if(action == GLFW_PRESS)
	{
		for(int i = 0; i < DICE_COUNT; i++)
		{
			if(key == keys[i])
			{
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
						if(temp.getX() < 0 || temp.getX() >= MAP_SIZE || temp.getY() < 0 || temp.getY() >= MAP_SIZE)
						{
							bombs.erase(it);
							explodedAll = false;	
							break;
						}
						else if(map.at(temp.getX()).at(temp.getY()) != EMPTY)
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
					}
				}

				//Reset the dice
				diceVals[i] = 0;	
				rolled = false;	
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
			bomb("res/textures/bomb2.png");
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
	std::vector<Enemy> enemies;
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

	//Enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//Background color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	while(!glfwWindowShouldClose(win))
	{	
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		//Draw the map
		simple.Use();
		
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
	
			glUniform2f(simple.getUniformLocation("uPos"), (float)i * 0.1f * 2.0f - 0.2f, -0.9f);	
			glDrawArrays(GL_TRIANGLES, 0, 6);	
		}
	
		//GLFW stuff
		glfwPollEvents();
		glfwSwapBuffers(win);
	}

	glfwTerminate();
}
