#pragma once
#ifndef BASIC_H
#define BASIC_H

//include third-party libraries
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

//Custom headers
#include "shader.hpp"
#include "VAO.hpp"
#include "texture.hpp"
#include "camera.hpp"

//avoid multiple including? -static will be useful here
#ifndef glCheckError()
static GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}

#define glCheckError() glCheckError_(__FILE__, __LINE__) 
#endif // !glCheckError()


class BasicApp {

private:
	
	//utilize singleton
	static BasicApp* instancePtr;

public:
#pragma region parameters
	int width = 1024;
	int height = 768;

	//cameras
	Camera camera; //(glm::vec3(0.0f, 0.0f, 3.0f));


	//time
	float deltaTime = 0.0f;  //for smooth speed
	float lastFrame = 0.0f;

	//mouse
	bool firstMouse = true;
	float lastX;
	float lastY; 

#pragma endregion

	BasicApp();
	BasicApp(int inWidth, int inHeight, Camera inCam);
	// deleting copy constructor
	BasicApp(const BasicApp& obj)
		= delete;
	~BasicApp(){delete instancePtr;}

	virtual void RunApplication();

	static BasicApp* getInstance(int inWidth=800, int inHeight=600, Camera inCam=Camera())
	{
		if (instancePtr == nullptr)
		{
			instancePtr = new BasicApp(inWidth, inHeight, inCam);
		}
		
		return instancePtr;
	}

	#pragma region utility functions
		//for the resize callback
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		}

		//check exit operation
		static void processInput(GLFWwindow* window)
		{
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, true);

			auto instance = getInstance();
			//process the camera's movement
			float cameraSpeed = static_cast<float>(2.5 * instance->deltaTime);
			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				instance->camera.ProcessKeyboard(CAMERA_MOVEMENT::Forward, instance->deltaTime);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				instance->camera.ProcessKeyboard(CAMERA_MOVEMENT::Backward, instance->deltaTime);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				instance->camera.ProcessKeyboard(CAMERA_MOVEMENT::Left, instance->deltaTime);  //obtained right vector from cross product
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				instance->camera.ProcessKeyboard(CAMERA_MOVEMENT::Right, instance->deltaTime);

		}

		static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
		{
			auto instance = getInstance();
			if (instance->firstMouse)
			{
				instance->lastX = (float)xpos;
				instance->lastY = (float)ypos;
				instance->firstMouse = false;   //for avoiding sudden jump in the beginning
			}
			//calculate the offset since the last frame
			//calculate the direction vector
			float xoffset = (float)xpos - instance->lastX;
			float yoffset = instance->lastY - (float)ypos;  //reversed since y-coordinates range from bottom-top
			instance->lastX = (float)xpos;
			instance->lastY = (float)ypos;

			instance->camera.ProcessMouseMovement(xoffset, yoffset);
		}

		static void scroll_back(GLFWwindow* window, double xoffset, double yoffset)
		{
			auto instance = getInstance();
			instance->camera.ProcessMouseScroll(static_cast<float>(yoffset));
		}

	#pragma endregion
};


#endif // !Basic_H