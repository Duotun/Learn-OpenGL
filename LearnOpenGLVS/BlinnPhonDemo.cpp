#include "BlinnPhonDemo.hpp"
#include "stb_image_write.h"

BlinnPhonApp* BlinnPhonApp::instancePtr = nullptr;

BlinnPhonApp::BlinnPhonApp()
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

	//camera.Position = glm::vec3(0.0f, 0.0f, 5.0f);
	instancePtr = this;
}

BlinnPhonApp::BlinnPhonApp(int inWidth, int inHeight, Camera inCam)
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

	//apply camera transforms during the initialization stage
	//camera.Yaw = 90.0f;
	//camera.UpdateCameraVectors();

	instancePtr = this;
}


void BlinnPhonApp::RunApplication()
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

	GLFWwindow* window = glfwCreateWindow(width, height, "BlinnPhonDemo", NULL, NULL);
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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // enable cursour but not visible
	glfwSetScrollCallback(window, scroll_back);  //for the mouse scroll

	glCheckError();  //custom functions to debug

	//setup shaders
	//Shader lightShader("light.vert", "light.frag");
	Shader lightShader("blinn.vert", "blinn.frag");
	//Shader basicShader("texture.vert", "texture.frag");
	//Shader basicShader("basic.vert", "basic.frag");

	//setup vertex data - with more attributes
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	};


	VAO Objvao, lightsourceVAO;
	Objvao.Bind();
	VBO vbo(planeVertices, sizeof(planeVertices));
	//EBO ebo(indices, sizeof(indices));

	Objvao.linkVBO(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);  //link to layout 0 - position
	Objvao.linkVBO(vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));  //link to layout 1 - normal
	Objvao.linkVBO(vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));  //link to layout 2 -- uv
	//glBindTexture(GL_TEXTURE_2D, texture);
	Objvao.Unbind();
	//ebo.UnBind();  //optional

	// if use wireframe polygons
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Enable the depth test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
	//glDepthFunc(GL_LEQUAL);


	//init Camera window Properties
	camera.UpdateCamWindow(width, height, 0.01f, 100.0f);  //with near and far planes

	//setup light and texture properties  -> assign the sampler 
	glm::vec3 lightPos(0.f, 0.f, 0.f);

	Texture texture1("wood.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, GL_REPEAT);
	texture1.texUnit(lightShader, "floorTexture", 0);

	bool blinn = true;

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

		//Activate and Bind the above two textures with corresponding sampler
		//texture0.Activate();

		//draw the triangles with shaders
		lightShader.use();
		lightShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.3f));
		glm::vec3 lightColor = glm::vec3(1.0f);

		//for the uniform parameters for the lights
		lightShader.setVec3("viewPos", camera.Position);  //for specular light	
		lightShader.setVec3("lightPos", lightPos);
		lightShader.setInt("blinn", blinn);


		//transformations for 3D Case
		glm::mat4 view = glm::mat4(1.0f);
		camera.UpdateViewMatrix(lightShader, "view");
		camera.UpdateProjectionMatrix(lightShader, "projection");

		//render the floor
		Objvao.Bind();   //bind VAO we want to draw from vertex attributes' data
		texture1.Activate();
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//floor
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwPollEvents();  //proceed the defined updated states
		// Swap buffers  (double buffer)
		glfwSwapBuffers(window);
	}

	//de-allocate all resources	
	Objvao.Delete();
	lightsourceVAO.Delete();
	vbo.Delete();

	lightShader.Delete();

	texture1.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();  //terminate GLFW
	

}
