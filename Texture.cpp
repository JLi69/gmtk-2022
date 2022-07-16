#include "Texture.h"
#include <SOIL2/SOIL2.h>
#include <iostream>
#include <glad/glad.h>

unsigned int Texture::readTexture(const char *path)
{
	mTexId = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	//Failed to open file!
	if(mTexId == 0)
	{
		std::cout << "Failed to open file: " << path << '\n';	
		return 0;	
	}
	return mTexId;
}

void Texture::Activate()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTexId);
}

Texture::Texture(const char *path)
{
	readTexture(path);
}
