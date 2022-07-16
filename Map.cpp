#include "Map.h"
#include <cstdlib>

void genMap(std::array<std::array<Tile, MAP_SIZE>, MAP_SIZE> &map)
{
	//Start the map out as empty	
	for(int i = 0; i < MAP_SIZE; i++)
		for(int j = 0; j < MAP_SIZE; j++)
			map.at(i).at(j) = DIRT;

	//Generate a random height map
	float height[MAP_SIZE][MAP_SIZE];
	for(int i = 0; i < MAP_SIZE; i++)
		for(int j = 0; j < MAP_SIZE; j++)
			height[i][j] = (float)(rand() % 1000) / 999.0f;

	//Smooth out the heightmap	
	int radius = 1;	
	while(radius <= ITERATIONS)
	{
		for(int i = 0; i < MAP_SIZE; i++)
			for(int j = 0; j < MAP_SIZE; j++)
			{
				float total = 0.0f,
					  num = 0.0f;
				for(int i2 = i - radius; i2 <= i + radius; i2++)
				{	
					for(int j2 = j - radius; j2 <= j + radius; j2++)
					{
						//Don't go out of bounds	
						if(i2 < 0 || i2 >= MAP_SIZE ||
							j2 < 0 || j2 >= MAP_SIZE)
							continue;	
							
						total += height[i2][j2];
						num++;	
					}
				}

				height[i][j] = total / num;
			}
		radius++;	
	}

	//Apply the features to the map			
	for(int i = 0; i < MAP_SIZE; i++)
	{	
		for(int j = 0; j < MAP_SIZE; j++)
		{	
			if(height[i][j] <= 0.48f)
				map.at(i).at(j) = EMPTY;
			if(height[i][j] > 0.5f && height[i][j] < 0.53f && rand() % 3 == 0)
				map.at(i).at(j) = STONE;	
		}	
	}
}
