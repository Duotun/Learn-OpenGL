#include "FrameBuffer.hpp"
#include "stb_image_write.h"
FrameBufferApp* FrameBufferApp::instancePtr = nullptr;

FrameBufferApp::FrameBufferApp()
{
	if (instancePtr != nullptr) return;
	width = 800; height = 600;
	camera = Camera();
	deltaTime = 0.0f;  //for smooth speed
	lastFrame = 0.0f;

	//mouse
	firstMouse = true;
	lastX = width / 2.0f;
	lastY = height / 2.0f;

	instancePtr = this;
}

FrameBufferApp::FrameBufferApp(int inWidth, int inHeight, Camera inCam)
{
	if (instancePtr != nullptr) return;
	width = inWidth; height = inHeight;
	camera = inCam;
	deltaTime = 0.0f;  //for smooth speed
	lastFrame = 0.0f;

	//mouse
	firstMouse = true;
	lastX = width / 2.0f;
	lastY = height / 2.0f;

	instancePtr = this;
}

void FrameBufferApp::RunApplication()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return;
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
		return;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	//Set up viewport for the rendering size
	glViewport(0, 0, width, height);  //lower-left corner  [-1, 1] to [width, height]
	//Set up resize call-back and mouse input 
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // enable cursour but not visible
	glfwSetScrollCallback(window, scroll_back);  //for the mouse scroll

	glCheckError();  //custom functions to debug

	//setup shaders
	Shader basicShader("depthtest.vert", "depthtest.frag");
	//Shader screenShader("framebuffer_screen.vert", "framebuffer_screen.frag");
	//Shader screenShader("grayscale.vert", "grayscale.frag");
	//Shader screenShader("sharpen.vert", "sharpen.frag");
	//Shader screenShader("Boxblur.vert", "Boxblur.frag");
	Shader screenShader("edge.vert", "edge.frag");
	//Shader basicShader("texture.vert", "texture.frag");
	//Shader basicShader("basic.vert", "basic.frag");

	//setup vertex data - with more attributes
	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	float quadVertices[] =
	{
		// positions   // texCoords
	   -1.0f,  1.0f,  0.0f, 1.0f,
	   -1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

	   -1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};


	VAO cubeVao, planeVao;
	cubeVao.Bind();
	VBO cubeVbo(cubeVertices, sizeof(cubeVertices));
	//EBO ebo(indices, sizeof(indices));

	cubeVao.linkVBO(cubeVbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);  //link to layout 0 - position
	cubeVao.linkVBO(cubeVbo, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));  //link to layout 1 - uv
	cubeVao.Unbind();
	//ebo.UnBind();  //optional

	//configure the light's VAO, reuse the cube data
	planeVao.Bind();
	VBO planeVbo(planeVertices, sizeof(planeVertices));
	planeVao.linkVBO(planeVbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);  //link to layout 0 - position only
	planeVao.linkVBO(planeVbo, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));  //link to layout 1 - uv

	planeVao.Unbind();

	//screen quad VAO
	VAO quadVAO; VBO quadVBO(quadVertices, sizeof(quadVertices));
	quadVAO.Bind();
	quadVAO.linkVBO(quadVBO, 0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
	quadVAO.linkVBO(quadVBO, 1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	quadVAO.Unbind();


	//prepare textures from texture class
	Texture texture0("container.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE, GL_RGB, GL_REPEAT);
	Texture texture1("metal.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, GL_REPEAT);
	texture0.texUnit(basicShader, "texture1", 0);  //make sure match the names in the shader, set sampler
	texture1.texUnit(basicShader, "texture1", 0);  //make sure match the names in the shader, set sampler

	basicShader.use();
	basicShader.setInt("texture1", 0);  //setup sampler
	
	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	//frame buffer configuration
	unsigned int framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// create a color attachment texture
	unsigned int textureColorbuffer;
	glGenTextures(1, &textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // use a single renderbuffer object for both a depth AND stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	
	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// if use wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Enable the depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//Enable face culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	//init Camera window Properties
	camera.UpdateCamWindow(width, height, 0.01f, 100.0f);  //with near and far planes

	int cnt =0;
	//keep the window open
	while (!glfwWindowShouldClose(window))
	{
		//process input
		processInput(window);

		//process deltaTime
		float currentTime = static_cast<float>(glfwGetTime());
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;


		//bind to the frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//Setup clear Color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  //clean under the buffer bound
		glEnable(GL_DEPTH_TEST);   //Enable depth testing for scene rendering

		//draw the triangles with shaders
		basicShader.use();

		//transformations for 3D Case

		glm::mat4 view = glm::mat4(1.0f);
		camera.UpdateViewMatrix(basicShader, "view");
		camera.UpdateProjectionMatrix(basicShader, "projection");
		glm::mat4 model = glm::mat4(1.0f);
		basicShader.setMat4("model", model);

		//render the cube
		cubeVao.Bind();   //bind VAO we want to draw from vertex attributes' data
		//Activate and Bind the above two textures with corresponding sampler
		texture0.Activate();
		texture0.Bind();
		model = glm::translate(model, glm::vec3(-1.0, 0.0, -1.0));
		basicShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//draw the second one
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		basicShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//draw the plane floor
		planeVao.Bind();
		texture1.Activate();
		texture1.Bind();
		basicShader.setMat4("model", glm::mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		planeVao.Unbind(); //glBindVertexArray(0); //bind VAO back to the screen

		//now bind back to the default framebuffer (Screen)
		//and draw a quad plane with the attached frame buffer color texture
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST); // DISABLE Depth test for rendering quad only
		//clear all relevant buffers
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		screenShader.use();
		//screenShader.setFloat("time", (glm::sin(glfwGetTime()))*2.0);
		glBindVertexArray(quadVAO.ID);
		glActiveTexture(0);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);   //bind textureColorbuffer to texture sampler 0
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwPollEvents();  //proceed the defined updated states
		// Swap buffers  (double buffer)
		glfwSwapBuffers(window);
	}

	/*
	if (cnt++ == 0)
	{
		//save texture into images
		int data_size = width * height * 4;
		GLubyte* pixels = new GLubyte[width * height * 4];
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		stbi_write_bmp("test.bmp", width, height, 4, pixels);  //simple way of using bmp to process data within [0,1]
	}
	*/

	//de-allocate all resources	
	cubeVao.Delete();
	planeVao.Delete();
	cubeVbo.Delete();
	planeVbo.Delete();
	basicShader.Delete();
	screenShader.Delete();
	texture0.Delete();
	texture1.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();  //terminate GLFW
}