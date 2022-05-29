#pragma once
#include "WindowApplication.hpp"

//gl_Position is necessary
static const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

static const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

const char* fragmentShader2Source = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
"}\n\0";
// use \0 to mark the end of char array

class TriangleApp : public GLFWApplication {

public:
	TriangleApp(int width, int height) noexcept { Width = width; Height = height; window = nullptr; vertices = nullptr; };   //constructer is assumed noexcept implicitly
	TriangleApp() { Width = 800; Height = 600; window = nullptr; vertices = nullptr; };

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

		//prepare vertex and fragement shader
		unsigned int vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);   //1 for sourcecode string
		glCompileShader(vertexShader);  //compile and check whether it is compiled successfully

		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);   // Check the compilation of vertex shader
		if (!success)  //0 means fail
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::Fragment::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		unsigned int fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		//link shader program, link outputs of each shader to the next shader, first vertex and then fragment
		//still ID indication based
		shaderProgram_1 = glCreateProgram();
		glAttachShader(shaderProgram_1, vertexShader);    //attach the shader and link all the shaders into one program
		glAttachShader(shaderProgram_1, fragmentShader);
		glLinkProgram(shaderProgram_1);

		//check glprogram linking successfully or not
		glGetProgramiv(shaderProgram_1, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram_1, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::Llinking Program::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		//link to the second fragment shader program
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShader2Source, NULL);
		glCompileShader(fragmentShader);

		//link shader program, link outputs of each shader to the next shader, first vertex and then fragment
		//still ID indication based
		shaderProgram_2 = glCreateProgram();
		glAttachShader(shaderProgram_2, vertexShader);
		glAttachShader(shaderProgram_2, fragmentShader);
		glLinkProgram(shaderProgram_2);

		//check glprogram linking successfully or not
		glGetProgramiv(shaderProgram_2, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram_2, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::Llinking Program::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		//after linking , shaders could be deleted
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);


		//init vertices data - triangle
		float FirstTrivertices[] = {    //we need array stuff not simply array since we want the size of array
				0.0f, 0.5f, 0.0f,  //top right
				0.5f, 0.0f, 0.0f,
				-0.5f, 0.0f, 0.0f,
		};

		float SecondTrivertices[] = {
				0.5f, 0.0f, 0.0f,
				-0.5f, 0.0f, 0.0f,
				0.0f, -0.5f, 0.0f   //top left
		};

		unsigned int indices[] = {   //counter-clock wise
			0, 1, 2,
			3, 1, 2
		};

		vertices = FirstTrivertices;

		//init VBO, VBO, EBO
		
		glGenVertexArrays(2, VAOs);    //prepare VAO
		glGenBuffers(2, VBOs);   //gen one buffer for vertices
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAOs[0]);    //bind VAO first 
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); //bind VBO to the gl buffer
		//COPY data from CPU side to the currently bound buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(FirstTrivertices), FirstTrivertices, GL_STATIC_DRAW);
		//location 0, 3 as vec3, 
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //(void*)0 indicates 0 offset of the data array
		glEnableVertexAttribArray(0);

		//bind ebo to the gl buffer with indices
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


		glBindVertexArray(VAOs[1]);    //bind VAO first 
		glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]); //bind VBO to the gl buffer
		//COPY data from CPU side to the currently bound buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(SecondTrivertices), SecondTrivertices, GL_STATIC_DRAW);
		//location 0, 3 as vec3, 
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //(void*)0 indicates 0 offset of the data array
		glEnableVertexAttribArray(0);

		//glBindBuffer(GL_ARRAY_BUFFER, 0);   // for later unbind safely
		//glBindVertexArray(0);   //for VAOs
		
		//change to the line wireframe mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	virtual void mainloop() override   //include logic loop and renderloop
	{
		while (!glfwWindowShouldClose(window))
		{
			//logic loop - input
			ProcessInput();

			//render loop part 
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			//std::cout << "here" << std::endl;
			//perform drawing
			glUseProgram(shaderProgram_1);   //two VAOs for two Triangles separately totally
			glBindVertexArray(VAOs[0]);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			glUseProgram(shaderProgram_2);
			glBindVertexArray(VAOs[1]);
			glDrawArrays(GL_TRIANGLES, 0, 3);
			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);  //DRAW 6 VERTICES
			
			// check and call all event and swap buffer for displaying
			glfwSwapBuffers(window);   //swap chain(double) buffer for screen display
			glfwPollEvents();  //check any events
		}

	}

	virtual void cleanup() override
	{
		// DE-Allocate all recources on the GPU Side
		glDeleteVertexArrays(2, VAOs);
		glDeleteBuffers(2, VBOs);
		glDeleteProgram(shaderProgram_1);
		glDeleteProgram(shaderProgram_2);

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
	float* vertices;
	unsigned int VBOs[2], VAOs[2];  //excercises for two triangles
	unsigned int EBO, shaderProgram_1=0, shaderProgram_2=0;
	uint32_t Width = 800;
	uint32_t Height = 600;
	GLFWwindow* window;
};