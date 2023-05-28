#include "Light.hpp"
lightApp* lightApp::instancePtr = nullptr;

lightApp::lightApp()
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
lightApp::lightApp(int inWidth, int inHeight, Camera inCam)
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

void lightApp::RunApplication()
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
	//Shader lightShader("light.vert", "light.frag");
	Shader lightShader("material.vert", "material.frag");
	Shader lightSourceShader("lightsource.vert", "lightsource.frag");
	//Shader basicShader("texture.vert", "texture.frag");
	//Shader basicShader("basic.vert", "basic.frag");

	//setup vertex data - with more attributes
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};


	VAO Objvao, lightsourceVAO;
	Objvao.Bind();
	VBO vbo(vertices, sizeof(vertices));
	//EBO ebo(indices, sizeof(indices));

	Objvao.linkVBO(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);  //link to layout 0 - position
	Objvao.linkVBO(vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));  //link to layout 1 - normal
	Objvao.linkVBO(vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));  //link to layout 2 -- uv
	//glBindTexture(GL_TEXTURE_2D, texture);
	Objvao.Unbind();
	//ebo.UnBind();  //optional

	//configure the light's VAO, reuse the cube data
	lightsourceVAO.Bind();
	lightsourceVAO.linkVBO(vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);  //link to layout 0 - position only
	//vao.linkVBO(vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));  //link to layout 1 - color
	//lightsourceVAO.linkVBO(vbo, 2, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));  //link to layout 2 -- uv
	//glBindTexture(GL_TEXTURE_2D, texture);
	lightsourceVAO.Unbind();

	//prepare textures from texture class
	//Texture texture0("container.jpg", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE, GL_RGB, GL_REPEAT);
	//texture0.texUnit(basicShader, "outTexture0", 0);  //make sure match the names in the shader


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

	//setup light and texture properties  -> assign the sampler 
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
	Texture texture1("container2.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, GL_REPEAT);
	texture1.texUnit(lightShader, "material.diffuse", 0);
	Texture texture2("container2_specular.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, GL_REPEAT);
	texture2.texUnit(lightShader, "material.specular", 1);
	Texture texture3("matrix.jpg", GL_TEXTURE_2D, GL_TEXTURE2, GL_RGB, GL_UNSIGNED_BYTE, GL_RGB, GL_REPEAT);
	texture3.texUnit(lightShader, "material.emission", 2);
	
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
		//lightColor.x = sin(glfwGetTime() * 2.0f);
		//lightColor.y = sin(glfwGetTime() * 0.7f);
		//lightColor.z = sin(glfwGetTime() * 1.3f);

		lightShader.setVec3("lightColor", lightColor);  //could vary light color as well
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

		lightPos.x = 1.0f + sin(glfwGetTime())*1.8f;
		lightPos.y = cos(glfwGetTime()/2.0f)*1.0f + 0.2f;
		lightShader.setVec3("light.position", lightPos);
		lightShader.setVec3("light.ambient", ambientColor);
		lightShader.setVec3("light.diffuse", diffuseColor);
		lightShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		lightShader.setVec3("viewPos", camera.Position);  //for specular light
		//setup materials
		texture1.Activate();  //for diffuse
		texture2.Activate();  //for specular
		texture3.Activate();  //for emission
		lightShader.setFloat("material.shininess", 32.0f);

		//transformations for 3D Case
		glm::mat4 view = glm::mat4(1.0f);
		camera.UpdateViewMatrix(lightShader, "view");
		camera.UpdateProjectionMatrix(lightShader, "projection");
		glm::mat4 model = glm::mat4(1.0f);
		lightShader.setMat4("model", model);

		//render the cube
		Objvao.Bind();   //bind VAO we want to draw from vertex attributes' data
		glDrawArrays(GL_TRIANGLES, 0 ,36);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//draw the light
		lightSourceShader.use();
		camera.UpdateViewMatrix(lightSourceShader, "view");
		camera.UpdateProjectionMatrix(lightSourceShader, "projection");
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightSourceShader.setMat4("model", model);

		lightsourceVAO.Bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwPollEvents();  //proceed the defined updated states
		// Swap buffers  (double buffer)
		glfwSwapBuffers(window);
	}

	//de-allocate all resources	
	Objvao.Delete();
	lightsourceVAO.Delete();
	vbo.Delete();
	//ebo.Delete();
	lightShader.Delete();
	lightSourceShader.Delete();
	//texture0.Delete();
	//texture1.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();  //terminate GLFW
	
}