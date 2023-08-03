#include "ModelTest.hpp"
#include "model.hpp"

#include <string>
#include <cstdlib>

modelApp* modelApp::instancePtr = nullptr;

modelApp::modelApp()
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

modelApp::modelApp(int inWidth, int inHeight, Camera inCam)
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

void modelApp::RunApplication()
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

	GLFWwindow* window = glfwCreateWindow(width, height, "ModelTest", NULL, NULL);
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

	stbi_set_flip_vertically_on_load(true);   //flip loaded texture before loading model

	//setup shaders
	Shader modelShader("modelTest.vert", "modelTest.frag");

	//load models
	std::string path = "./backpack/backpack.obj";
	Model testModel(path.c_str());

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
		modelShader.use();

		//transformations for 3D Case

		glm::mat4 view = glm::mat4(1.0f);
		camera.UpdateViewMatrix(modelShader, "view");
		camera.UpdateProjectionMatrix(modelShader, "projection");
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f ,0.1f));
		modelShader.setMat4("model", model);

		//renderl the loaded model
		testModel.Draw(modelShader);

		glfwPollEvents();  //proceed the defined updated states
		// Swap buffers  (double buffer)
		glfwSwapBuffers(window);
	}

	//de-allocate all resources	

	glfwDestroyWindow(window);
	glfwTerminate();  //terminate GLFW
}