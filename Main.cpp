#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

int main()
{
	//Set up glfw	
	if(!glfwInit())
		return -1;
	GLFWwindow* win = glfwCreateWindow(800, 800, "20 rolls to night", NULL, NULL);
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

	while(!glfwWindowShouldClose(win))
	{	
		//Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		//test rectangle
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//Glfw stuff
		glfwPollEvents();
		glfwSwapBuffers(win);
	}

	glfwTerminate();
}
