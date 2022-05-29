#pragma once
#include "WindowApplication.hpp"
#include "Shader.hpp"

class ShaderApp : public GLFWApplication
{
public:  //methods to override
	ShaderApp(int width, int height) { Width = width; Height = height; window = nullptr; vertices = nullptr; };
	ShaderApp() {Width = 800; Height = 600; window = nullptr; vertices = nullptr; };
    ~ShaderApp() noexcept = default;

	virtual void run() override
	{
		GLFWWindowinit();
		OpenGLInit();
		mainloop();
		cleanup();

	}

	virtual void GLFWWindowinit() override
	{
		glfwInit();

		//set up context as hints of opengl for glfw
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);  //opengl 4.6
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //use core profile

		//try creating window
		window = glfwCreateWindow(Width, Height, "DrawTriangleApp", nullptr, nullptr);
		if (window == nullptr)
		{
			std::cout << "Failed to Create GLFW Window" << std::endl;
			glfwTerminate();
			return;
		}

		glfwMakeContextCurrent(window);  //context go with the current window
		//init the rendeirng window, set up auto resize of viewport with glfw window
		glfwSetFramebufferSizeCallback(window, frambuffer_size_callback);   //first two, set the lower left corner


	}

	virtual void OpenGLInit() override
	{
		//before any calling of OpenGL functions, let's setup GLAD  managing drivers' functions
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}

		//initialize Shaders
		myShader = Shader("BasicTri.vs", "BasicTri.fs");
		
		//init vertices data - triangle
		float trivertices[] = {    //we need array stuff not simply array since we want the size of array
				0.0f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   //pos and color
				0.5f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
				-0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f
		};

		unsigned int indices[] = {   //counter-clock wise
			0, 1, 2,
			3, 1, 2
		};

		vertices = trivertices;

		//init VBO, VBO, EBO

		glGenVertexArrays(1, &VAO);    //prepare VAO
		glGenBuffers(1, &VBO);   //gen one buffer for vertices
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);    //bind VAO first 
		glBindBuffer(GL_ARRAY_BUFFER, VBO); //bind VBO to the gl buffer
		//COPY data from CPU side to the currently bound buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(trivertices), trivertices, GL_STATIC_DRAW);
		//location 0, 3 as vec3, stride becomses 6 floats
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); //(void*)0 indicates 0 offset of the data array
		glEnableVertexAttribArray(0);

		//bind the color attributes
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float))); //(void*)0 indicates 0 offset of the data array
		glEnableVertexAttribArray(1);

		//bind ebo to the gl buffer with indices
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//glBindBuffer(GL_ARRAY_BUFFER, 0);   // for later unbind safely
		//glBindVertexArray(0);   //for VAOs

		//change to the line wireframe mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	virtual void mainloop() override   //include logic loop and renderloop
	{
		double timeValue;
		while (!glfwWindowShouldClose(window))
		{
			//logic loop - input
			ProcessInput();

			//render loop part 
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			//std::cout << "here" << std::endl;
			//perform drawing
			myShader.use();
			myShader.setFloat("offset", 1.0f);
			//glUseProgram(shaderProgram);   //two VAOs for two Triangles separately totally
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 3);


			// check and call all event and swap buffer for displaying
			glfwSwapBuffers(window);   //swap chain(double) buffer for screen display
			glfwPollEvents();  //check any events
		}

	}

	virtual void cleanup() override
	{
		// DE-Allocate all recources on the GPU Side
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteProgram(myShader.ID);

		glfwDestroyWindow(window);  //processed window 
		glfwTerminate();
	}

	virtual void ProcessInput() override
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);  //set to true to close the windwo
		}
	}



private:
	//data members
	float* vertices;
	unsigned int VBO=0, VAO=0;  //excercises for two triangles
	unsigned int EBO = 0;
	uint32_t Width = 800;
	uint32_t Height = 600;
	GLFWwindow* window;
	Shader myShader;
};
