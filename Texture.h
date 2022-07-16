class Texture
{
	//Texture id	
	unsigned int mTexId;

	//Read in a texture from an image	
	unsigned int readTexture(const char *path);
public:
	void Activate();

	//Constructor
	Texture(const char *path);
};
