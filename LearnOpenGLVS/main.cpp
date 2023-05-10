//include third-party libraries
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.hpp"
#include "VAO.hpp"
#include "texture.hpp"


#pragma region utility functions
//for the resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//check exit operation
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

GLenum glCheckError_(const char* file, int line)
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

#pragma endregion


#pragma region parameters

const int width = 1024;
const int height = 768;

#pragma endregion

int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	//give glfw hint to initialize opengl and window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  //3.3 opengl
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //use core profiles only
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // comment this line in a release build! 

	GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Set up viewport for the rendering size
	glViewport(0, 0, width, height);  //lower-left corner  [-1, 1] to [width, height]
	//Set up resize call-back
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glCheckError();  //custom functions to debug

	//setup shaders
	Shader basicShader("texture.vert", "texture.frag");
	//Shader basicShader("basic.vert", "basic.frag");

	//setup vertex data - with more attributes
	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};

	//for index buffer - element array, should be counter-clockwise
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2
	};

	VAO vao;
	vao.Bind();
	VBO vbo(vertices, sizeof(vertices));
	EBO ebo(indices, sizeof(indices));

	vao.linkVBO(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);  //link to layout 0 - position
	vao.linkVBO(vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));  //link to layout 1 - color
	vao.linkVBO(vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));  //link to layout 2 -- uv
	//glBindTexture(GL_TEXTURE_2D, texture);
	vao.Unbind();
	//ebo.UnBind();  //optional

	//prepare textures from texture class
	Texture texture0("container.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE, GL_RGB, GL_REPEAT);
	texture0.texUnit(basicShader, "outTexture0", 0);  //make sure match the names in the shader
	
	Texture texture1("awesomeface.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, GL_REPEAT);
	texture1.texUnit(basicShader, "outTexture1", 1);


	// if use wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//keep the window open
	while (!glfwWindowShouldClose(window))
	{
		//process input
		processInput(window);
		//Setup clear Color
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Activate and Bind the above two textures with corresponding sampler
		texture0.Activate();
		texture1.Activate();

		//draw the triangles with shaders
		basicShader.use();

		//use uniform case
		//float timeVal = glfwGetTime();
		//float greenVal = sin(timeVal)/2.0f + 0.5f;
		//int vertexColorLocation = glGetUniformLocation(basicShader.ID,"offset");
		//glUniform1f(vertexColorLocation, greenVal);

		vao.Bind();   //bind VAO we want to draw
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwPollEvents();  //proceed the defined updated states
		// Swap buffers  (double buffer)
		glfwSwapBuffers(window);
	}

	//de-allocate all resources	
	vao.Delete();
	vbo.Delete();
	ebo.Delete();
	basicShader.Delete();
	texture0.Delete();
	texture1.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();  //terminate GLFW
	return 0;
}