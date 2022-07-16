#include <array>

#define MAP_SIZE 20
#define ITERATIONS 2

enum Tile
{
	DIRT,
	STONE,	
	EMPTY	
};

void genMap(std::array<std::array<Tile, MAP_SIZE>, MAP_SIZE> &map);
