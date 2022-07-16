#include <string>


struct ShaderSrc
{
	std::string vert, frag;
};

class Shader
{
	//id	
	unsigned int mProgramId;

	//Read the shaders
	ShaderSrc mRead(std::string vertPath, std::string fragPath); 
	//Compile the shader into a program	
	void mCompile(ShaderSrc &src);

public:
	Shader(std::string vertPath, std::string fragPath); //constructor

	void Use();

	int getUniformLocation(std::string name);
};
