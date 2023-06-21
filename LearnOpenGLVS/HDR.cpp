#include "HDR.hpp"
#include "stb_image_write.h"
HDRApp* HDRApp::instancePtr = nullptr;


HDRApp::HDRApp()
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

HDRApp::HDRApp(int inWidth, int inHeight, Camera inCam)
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

	camera.Yaw = 90.0f;
	camera.UpdateCameraVectors();
	instancePtr = this;
}


void HDRApp::RunApplication()
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
	Shader SceneShader("lightHDR.vert", "lightHDR.frag");
	Shader HDRQuadShader("HDRQuadDisplay.vert", "HDRQuadDisplay.frag");

	VAO quadVAO, cubeVAO;
	VBO quadVBO, cubeVBO;

	Texture texture1("wood.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE, GL_RGBA, GL_REPEAT, true);  //true for sRGB, GL_UNSIGNED_BYTE
	texture1.texUnit(SceneShader, "diffuseTexture", 0);


	//configuring floating point frambuffer
	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);

	//create floating point color buffer to texture
	unsigned int colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	//create depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

	//attach buffers
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout<<"Framebuffer not complete!"<<std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//Enable the depth test
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	//Enable face culling
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	//init Camera window Properties
	camera.UpdateCamWindow(width, height, 0.1f, 100.0f);  //with near and far planes

	//lighting infor
	std::vector<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(0.0f, 0.0f, 49.5f));  //back light
	lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
	lightPositions.push_back(glm::vec3(0.0f, -1.8f, 4.0f));
	lightPositions.push_back(glm::vec3(0.8f, -1.7f, 6.0f));

	// colors
	std::vector<glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
	lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
	lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));


	//setup shader sampler manually
	SceneShader.use();
	SceneShader.setInt("diffuseTexture", 0);
	HDRQuadShader.use();
	HDRQuadShader.setInt("hdrBuffer", 0);

	bool hdr = true;
	float exposure = 1.2f;

	//std::cout<<"camera position "<<camera.Position[0]<<" " << camera.Position[1] <<" "<< camera.Position[2]<<std::endl;

	int cnt = 0;
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

		//1. render scene into floating point framebuffer

		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			SceneShader.use();
			camera.UpdateViewMatrix(SceneShader, "view");
			camera.UpdateProjectionMatrix(SceneShader, "projection");

			texture1.Activate();
			texture1.Bind();
			for (unsigned int i = 0; i < lightPositions.size(); i++)
			{
				SceneShader.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
				SceneShader.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
				
			}

			//render tunnel
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
			model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
			SceneShader.setMat4("model", model);
			SceneShader.setInt("inverse_normals", true);  // view in the cube
			renderCube(cubeVAO, cubeVBO);
			//save the hdr into file
			if (cnt++ == 0) {
				//save texture into images
				int data_size = width * height * 4;
				GLfloat* pixels = new GLfloat[width * height * 4];
				glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, pixels);   //as GL_RGBA while pixel format is GL_float to readout the hdr color

				stbi_write_hdr("test.hdr", width, height, 4, pixels);
				std::cout << "save the hdr" << std::endl;
			}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//2 Render floating point color buffer to 2d quad and tonemap HDR to default framebuffer's color range

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		HDRQuadShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorBuffer);   //utilize HDR texture as input
		HDRQuadShader.setBool("hdr", hdr);
		HDRQuadShader.setFloat("exposure", exposure);
		renderQuad(quadVAO, quadVBO);

		//std::cout << "hdr: " << (hdr ? "on" : "off") << "| exposure: " << exposure << std::endl;

		glfwPollEvents();  //proceed the defined updated states
		// Swap buffers  (double buffer)
		glfwSwapBuffers(window);
	}

	//de-allocate all resources	
	quadVAO.Delete(); cubeVAO.Delete();
	quadVBO.Delete(); cubeVBO.Delete();

	//ebo.Delete();
	SceneShader.Delete();
	HDRQuadShader.Delete();

	texture1.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();  //terminate GLFW

}


//Define two scene rendering functions

void renderQuad(VAO& inVAO, VBO& inVBO)
{
	if (inVAO.linked == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		//set up VAO and VBO
		inVAO.Bind();
		inVBO.BindBufferData(quadVertices, sizeof(quadVertices), GL_STATIC_DRAW);
		inVAO.linkVBO(inVBO, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
		inVAO.linkVBO(inVBO, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		inVAO.Unbind();
	}

	inVAO.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	inVAO.Unbind();
}

void renderCube(VAO& inVAO, VBO& inVBO)
{
	if (inVAO.linked == 0)
	{
		//Positions, normals and vertices
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};

		inVAO.Bind();
		inVBO.BindBufferData(vertices, sizeof(vertices), GL_STATIC_DRAW);
		inVAO.linkVBO(inVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
		inVAO.linkVBO(inVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		inVAO.linkVBO(inVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		inVAO.Unbind();
	}

	//render Cube
	inVAO.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	inVAO.Unbind();
}