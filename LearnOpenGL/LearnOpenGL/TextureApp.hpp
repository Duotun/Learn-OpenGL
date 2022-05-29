#pragma once
#include "WindowApplication.hpp"
#include "Shader.hpp"
// for loading textures from images
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

class TextureApp : public GLFWApplication
{
public:  //methods to override
	TextureApp(int width, int height) { Width = width; Height = height; window = nullptr; vertices = nullptr; };
	TextureApp() { Width = 800; Height = 600; window = nullptr; vertices = nullptr; };
	~TextureApp() noexcept = default;

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
		window = glfwCreateWindow(Width, Height, "TextureApp", nullptr, nullptr);
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
		myShader = Shader("BasicTex.vs", "BasicTex.fs");

		//init vertices data - triangle, stride becomes 32 bytes
		float trivertices[] = {    //we need array stuff not simply array since we want the size of array
				// positions          // colors           // texture coords
				 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
				 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
				-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
				-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 

		};

		unsigned int indices[] = {   //counter-clock wise
			0, 1, 3,
			1, 2, 3
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); //(void*)0 indicates 0 offset of the data array
		glEnableVertexAttribArray(0);

		//bind the color attributes
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); //(void*)0 indicates 0 offset of the data array
		glEnableVertexAttribArray(1);

		//texture coord attributes
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));  //last one is the offset
		glEnableVertexAttribArray(2);
		
		//bind ebo to the gl buffer with indices
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//glBindBuffer(GL_ARRAY_BUFFER, 0);   // for later unbind safely
		//glBindVertexArray(0);   //for VAOs

		//change to the line wireframe mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//load and create a texture
		//read image from the external
		
		glGenTextures(1, &texture1);
		glBindTexture(GL_TEXTURE_2D, texture1);   // bind the fllowing commands with the texture ID
		
		//set the texture wrapping and filtering options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
												 
		int width, height, nchannels;
		stbi_set_flip_vertically_on_load(true);  //flip the load mage vertically
		
		unsigned char* data = stbi_load("textures/container.jpg", &width, &height, &nchannels, 0);
		if (data) {
			//mipmap level 0
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D); //generate mipmaps more than level 0
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);  //free image data after texture generations

		//load texture2
		glGenTextures(1, &texture2);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//load image, create texture and generate mipmaps
		data = stbi_load("textures/awesomeface.png", &width, &height, &nchannels, 0);
		if (data)
		{
			// the awsomeface.png has transparency
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

		//tell the opengl shader which sample for texture
		myShader.use();
		// set it manually with uniform
		glUniform1i(glGetUniformLocation(myShader.ID, "texture1"), 0);  //for ASSIGN which texture unit to sampler (GL_TEXTURE0)
		//or set it via the texture class
		myShader.setInt("texture2", 1);
		//set the mix val
		glUniform1f(glGetUniformLocation(myShader.ID, "mixval"), mixval);

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

			//active and bind Texture, Currently 2 textures
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture2);
			//std::cout << "here" << std::endl;
			//perform drawing
			myShader.use();
			//update the mix val
			glUniform1f(glGetUniformLocation(myShader.ID, "mixval"), mixval);
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES,6, GL_UNSIGNED_INT, 0);  //index ways of drawing


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

		//Update the mixval with key
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			mixval = std::clamp(mixval+0.01f, 0.0f, 1.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			mixval = std::clamp(mixval- 0.01f, 0.0f, 1.0f);
		}
	}



private:
	//data members
	float* vertices;
	unsigned int VBO = 0, VAO = 0;  //excercises for two triangles
	unsigned int EBO = 0;
	uint32_t Width = 800;
	uint32_t Height = 600;
	unsigned int texture1=0;
	unsigned int texture2=1;
	float mixval = 0.2f;
	GLFWwindow* window;
	Shader myShader;
};