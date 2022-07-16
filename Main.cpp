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

//This code could supply Olive Garden for 10 years

int main()
{
	srand(time(NULL));	

	//Set up glfw	
	if(!glfwInit())
		return -1;
	GLFWwindow* win = glfwCreateWindow(800, 800, "Dice Miner", NULL, NULL);
	if(!win)
	{
		glfwTerminate();	
		return -1;
	}
	glfwMakeContextCurrent(win);
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
	Texture dirt("res/textures/dirt.png");

	//Map
	std::array<std::array<Tile, MAP_SIZE>, MAP_SIZE> map;
	//Generate the map	
	genMap(map);
	//Player
	Player player = Player(rand() % MAP_SIZE, rand() % MAP_SIZE);
	map.at(player.getY()).at(player.getX()) = EMPTY;
	//Enemies	
	std::vector<Enemy> enemies;
	//Create the enemies
	for(int i = 0; i < MAP_SIZE; i++)
	{	
		for(int j = 0; j < MAP_SIZE; j++)
		{	
			//Don't put an enemy on top of the player
			if((i - player.getY()) * (i - player.getY()) <= 1 ||
				(j - player.getX()) * (j - player.getX()) <= 1)
				continue;

			//If it is an empty spot, put an enemy there 10% of the time	
			if(map.at(i).at(j) == EMPTY && rand() % 10 == 0)
				enemies.push_back(Enemy(j, i, (EnemyType)(rand() % 3)));	
		}
	}

	dirt.Activate();
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
				case EMPTY: glUniform4f(simple.getUniformLocation("uColor"), 0.5f, 0.5f, 0.5f, 0.1f); break;
				case DIRT: glUniform4f(simple.getUniformLocation("uColor"), 0.5f, 0.2f, 0.0f, 1.0f); break;
				case STONE: glUniform4f(simple.getUniformLocation("uColor"), 0.2f, 0.2f, 0.2f, 1.0f); break;
				}

				glUniform2f(simple.getUniformLocation("uPos"), (float)i * 0.04f * 2.0f - 0.8f + 0.04f, (float)j * 0.04f * 2.0f - 0.8f + 0.04f);			
				glDrawArrays(GL_TRIANGLES, 0, 6);		
			}	
		}

		//Draw the player
		glUniform2f(simple.getUniformLocation("uPos"), (float)player.getX() * 0.04f * 2.0f - 0.8f + 0.04f, (float)player.getY() * 0.04f * 2.0f - 0.8f + 0.04f);
		glUniform4f(simple.getUniformLocation("uColor"), 1.0f, 1.0f, 0.0f, 1.0f);	
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Draw the enemies
		for(auto &enemy : enemies)
		{
			glUniform2f(simple.getUniformLocation("uPos"), (float)enemy.getX() * 0.04f * 2.0f - 0.8f + 0.04f, (float)enemy.getY() * 0.04f * 2.0f - 0.8f + 0.04f);
			glUniform4f(simple.getUniformLocation("uColor"), 1.0f, 0.0f, 0.0f, 1.0f);	
			glDrawArrays(GL_TRIANGLES, 0, 6);	
		}

		//Draw the dice		
		glUniform4f(simple.getUniformLocation("uColor"), 1.0f, 1.0f, 1.0f, 1.0f);	
		glUniform1f(simple.getUniformLocation("uScale"), 0.08f);	
		for(int i = 0; i < 3; i++)
		{	
			glUniform2f(simple.getUniformLocation("uPos"), (float)i * 0.1f * 2.0f - 0.2f, -0.9f);	
			glDrawArrays(GL_TRIANGLES, 0, 6);	
		}	
		

		//GLFW stuff
		glfwPollEvents();
		glfwSwapBuffers(win);
	}

	glfwTerminate();
}
