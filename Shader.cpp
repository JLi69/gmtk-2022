#include "Shader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/glad.h>

ShaderSrc Shader::mRead(std::string vertPath, std::string fragPath)
{
	ShaderSrc src;	
	std::stringstream vertSrcSS,
					  fragSrcSS;
	std::string line;

	//read the vertex shader
	std::ifstream vertFile(vertPath);
	if(!vertFile.is_open())
	{
		vertFile.close();	
		std::cout << "Cannot open vertex file: " << vertPath << '\n';
		return src;
	}
	while(std::getline(vertFile, line))
		vertSrcSS << line << '\n';
	src.vert = vertSrcSS.str();

	//Clean up
	vertFile.close();	

	//Read the fragment shader
	std::ifstream fragFile(fragPath);	
	if(!fragFile.is_open())
	{
		fragFile.close();	
		std::cout << "Cannot open vertex file: " << fragPath << '\n';
		return src;
	}
	while(std::getline(fragFile, line))
		fragSrcSS << line << '\n';
	src.frag = fragSrcSS.str();

	//Clean up
	fragFile.close();

	return src;
}

void Shader::mCompile(ShaderSrc &src)
{
	mProgramId = glCreateProgram();
	unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER),
				 fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	const char* vBegin = &src.vert[0];
	const char* fBegin = &src.frag[0]; 

	//Send the source code to the vertex shader and then compile it
	glShaderSource(vertShader, 1, &vBegin, nullptr);
	glCompileShader(vertShader);

	//Send the source code to the fragment shader and then compile it
	glShaderSource(fragShader, 1, &fBegin, nullptr);
	glCompileShader(fragShader);

	glAttachShader(mProgramId, vertShader);
	glAttachShader(mProgramId, fragShader);
	glLinkProgram(mProgramId);	
	glValidateProgram(mProgramId);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}

void Shader::Use()
{
	glUseProgram(mProgramId);
}

Shader::Shader(std::string vertPath, std::string fragPath)
{
	ShaderSrc src = mRead(vertPath, fragPath);	
	mCompile(src);
}

int Shader::getUniformLocation(std::string name)
{
	return glGetUniformLocation(mProgramId, name.c_str());
}
