#pragma once

#pragma region Core Headers
#include <glad/glad.h>  //need to put before glfw
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <algorithm>
#pragma endregion

// can't be class specific
static void frambuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// a very basic class for creating window application using glfw
class GLFWApplication {
public:

	virtual void run() //virtual functions for child classes to implement their own
	{
		GLFWWindowinit();
		OpenGLInit();
		mainloop();
		cleanup();
	}


	virtual void GLFWWindowinit()
	{
		glfwInit();

		//set up context as hints of opengl for glfw
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);  //opengl 4.6
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //use core profile
		
		//try creating window
		window = glfwCreateWindow(Width, Height, "WindowApp", nullptr, nullptr);
		if (window == nullptr)
		{
			std::cout << "Failed to Create GLFW Window" << std::endl;
			glfwTerminate();
			
		}
		glfwMakeContextCurrent(window);  //context go with the current window
	}

	virtual void OpenGLInit()
	{
		//before any calling of OpenGL functions, let's setup GLAD  managing drivers' functions
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return ;
		}

		//init the rendeirng window, set up auto resize of viewport with glfw window
		glfwSetFramebufferSizeCallback(window, frambuffer_size_callback);   //first two, set the lower left corner
		
	}

	virtual void mainloop()  //include logic loop and renderloop
	{
		while (!glfwWindowShouldClose(window))
		{
			//logic loop - input
			ProcessInput();

			//render loop part 
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// check and call all event and swap buffer for displaying
			glfwSwapBuffers(window);   //swap chain(double) buffer for screen display
			glfwPollEvents();  //check any events
		}
	}

	virtual void cleanup()
	{
		glfwDestroyWindow(window);  //processed window 
		glfwTerminate();
	}

	virtual void ProcessInput()
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);  //set to true to close the windwo
		}
	}

private:
	const uint32_t Width = 800;
	const uint32_t Height = 600;
	GLFWwindow* window;
};