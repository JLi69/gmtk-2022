#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

	while(!glfwWindowShouldClose(win))
	{
		glfwPollEvents();
		glfwSwapBuffers(win);
	}

	glfwTerminate();
}
