#include "DepthTest.hpp"
depthApp* depthApp::instancePtr = nullptr;

depthApp::depthApp()
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
depthApp::depthApp(int inWidth, int inHeight, Camera inCam)
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

void depthApp::RunApplication()
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
	Shader depthShader("depthtest.vert", "depthtest.frag");
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
	planeVao.linkVBO(planeVbo, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3*sizeof(float)));  //link to layout 1 - uv
	planeVao.Unbind();

	
	//prepare textures from texture class
	Texture texture0("marble.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE, GL_RGB, GL_REPEAT);
	Texture texture1("metal.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, GL_REPEAT);
	texture0.texUnit(depthShader, "texture1", 0);  //make sure match the sampler names in the shader
	texture1.texUnit(depthShader, "texture1", 0);  //make sure match the sampler names in the shader, once for a rendering


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

	//keep the window open
	while (!glfwWindowShouldClose(window))
	{
		//process input
		processInput(window);
		//Setup clear Color
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//process deltaTime
		float currentTime = static_cast<float>(glfwGetTime());
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;


		//draw the triangles with shaders
		depthShader.use();

		//transformations for 3D Case

		glm::mat4 view = glm::mat4(1.0f);
		camera.UpdateViewMatrix(depthShader, "view");
		camera.UpdateProjectionMatrix(depthShader, "projection");
		glm::mat4 model = glm::mat4(1.0f);
		depthShader.setMat4("model", model);

		//render the cube
		cubeVao.Bind();   //bind VAO we want to draw from vertex attributes' data
	    //Activate and Bind the above two textures with corresponding sampler
		texture0.Activate();
		texture0.Bind();
		model = glm::translate(model, glm::vec3(-1.0, 0.0, -1.0));
	    depthShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		//draw the second one
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
		depthShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//draw the plane floor
		planeVao.Bind();
		texture1.Activate();
		texture1.Bind();
		depthShader.setMat4("model", glm::mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwPollEvents();  //proceed the defined updated states
		// Swap buffers  (double buffer)
		glfwSwapBuffers(window);
	}

	//de-allocate all resources	
	cubeVao.Delete();
	planeVao.Delete();
	cubeVbo.Delete();
	planeVbo.Delete();
	depthShader.Delete();
	texture0.Delete();
	texture1.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();  //terminate GLFW
}