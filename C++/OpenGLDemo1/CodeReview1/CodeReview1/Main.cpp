#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "CameraMovement.h"
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
const unsigned int SCRN_WIDTH = 800, SCRN_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

void init();
void display();
void speedCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLFWwindow* window;
Shader* shaderProgram;
Shader* depthShaderProgram;
CameraMovement* cameraPath;
float cameraSpeed = 0.0f;
float bumpiness = 0.0f;
bool error = false;
unsigned int VBO, VAO, EBO;
unsigned int depthMap;	// depth map texture for shadows
unsigned int depthMapFBO; // buffer for the sahdows
unsigned int textureAlbedo; // texture to save the albedo information of the objects
unsigned int textureNormal; // texture storing the normal vectors information
int aaMode = 0;
bool resetWindow = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float vertices[] = {
	// positions          // normals           // texture coords // tangents
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,		1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,		1.0f, 0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,		0.0f, 1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,		0.0f, -1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,		1.0f, 0.0f, 0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,		1.0f, 0.0f, 0.0f
};

unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(0.0f,  -1.0f, 2.0f),
	glm::vec3(0.0f, 0.0f, 4.0f),
	glm::vec3(0.0f,  1.0f,  6.0f),
	glm::vec3(0.0f,  0.0f, 8.0f),
	glm::vec3(6.0f,  0.0f,  0.0f),
	glm::vec3(6.0f,  -1.0f, 2.0f),
	glm::vec3(6.0f, 0.0f, 4.0f),
	glm::vec3(6.0f,  1.0f,  6.0f),
	glm::vec3(6.0f,  0.0f, 8.0f),
	glm::vec3(6.0f,  0.0f,  -10.0f),
	glm::vec3(6.0f,  -1.0f, -2.0f),
	glm::vec3(6.0f, 0.0f, -4.0f),
	glm::vec3(6.0f,  1.0f,  -6.0f),
	glm::vec3(6.0f,  0.0f, -8.0f),
	glm::vec3(0.0f,  0.0f,  -10.0f),
	glm::vec3(0.0f,  -1.0f, -2.0f),
	glm::vec3(0.0f, 0.0f, -4.0f),
	glm::vec3(0.0f,  1.0f,  -6.0f),
	glm::vec3(0.0f,  0.0f, -8.0f),
};

int main()
{
	resetWindow = true;
	while(resetWindow) {
		resetWindow = false;
		init();
		if (error)
			return -1;
		display();
		glfwDestroyWindow(window);
	}

	delete shaderProgram;
	glfwTerminate();
	return 0;
}

void init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES,aaMode);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(SCRN_WIDTH, SCRN_HEIGHT, "CodeReview1", NULL, NULL);
	// create window
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		error = true;
		return;
	}
	glfwMakeContextCurrent(window);

	// initialize glad with the pointer to the corresponding functions for the drivers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		error = true;
		return;
	}
	 // we create the viewport and set the resize callback function
	glViewport(0, 0, SCRN_WIDTH, SCRN_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	 // we set the callback for the speed input
	glfwSetKeyCallback(window, speedCallback);

	// we load and compile the vertex and fragment shader to display the cubes in the scene
	shaderProgram = new Shader("VertexShader.vert","FragmentShader.frag");
	depthShaderProgram = new Shader("SimpleDepthShader.vert", "EmptyShader.frag");

	// we enable the depht buffer
	glEnable(GL_DEPTH_TEST);

	// we create the OpenGL objects to store the vertex and normal data of the cubes
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// we load the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// Shadows part
	// we create a framebuffer to store the shadow depth buffer
	glGenFramebuffers(1, &depthMapFBO);

	// We create the Texture to save the data
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// we attach the Texture to the generated shadow frame buffer
	// we set the draw and read buffers to none because we will not save colors
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// we bind the vertex data to this OpenGL object
	glBindVertexArray(VAO);


	// we create the textures that store the color information of the objects
	int width, height, nrChannels;
	unsigned char *data;

	// Load and create ALbedoTexture
	glGenTextures(1, &textureAlbedo);
	glBindTexture(GL_TEXTURE_2D, textureAlbedo);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("Albedo.png", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	// Load and create Normals Texture
	glGenTextures(1, &textureNormal);
	glBindTexture(GL_TEXTURE_2D, textureNormal);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	data = stbi_load("Normal.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureAlbedo);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureNormal);

	// we tell OpenGL which data is being sent to the shaders
	// the vertex coordinates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// the normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// the texture coordinates
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// the tangent coordinates
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);


	// we finish by bindint the buffers to tell OpenGL which buffer to use.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// activate antialiasing
	glEnable(GL_MULTISAMPLE);
}

void display() {
	cameraPath = new CameraMovement();
	// we initialize some matrixes
	// the view matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection;
	// the projection matrix has to be made only once
	projection = glm::perspective(glm::radians(-45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	glm::vec3 lightPos(10, -2, 30);

	// the main draw loop
	while (!glfwWindowShouldClose(window) && !resetWindow)
	{
		// we calculate the deltaTime to maintain a constant speed acrros frames and different machines
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// function to proccess the user's intput
		processInput(window);

		// we tell OpenGL which VAO we want to draw
		glBindVertexArray(VAO);
		// Make the ShadowsRender
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		// we use a orthographic projection to render the shadows, assuming the light is very far away
		float near_plane = 1.0f, far_plane = 100.0f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		// we create the transform to move to the light point of view using the LookAt function
		glm::mat4 lightView = glm::lookAt(lightPos,
			glm::vec3(3, 0, -15),
			glm::vec3(0, 1, 0));

		// we can combine both to obtain the whole transform in order to render from the light's POV
		glm::mat4 lightCompleteMat = lightProjection * lightView;
		
		depthShaderProgram->use();
		depthShaderProgram->setMat4F("lightSpaceMatrix", lightCompleteMat);

		for (int i = 0; i < 20; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			depthShaderProgram->setMat4F("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3,0,-15));
		model = glm::rotate(model, 45.0f, glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(10, 10, 1));
		depthShaderProgram->setMat4F("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Make the Normal Render, incorporating shadows
		// we clear last scene and paint the background with black
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCRN_WIDTH, SCRN_HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// we use our shader class to tell OpenGL which shaers we want to use
		shaderProgram->use();

		// we load some data to the shaders, like the lightsource's position and the projection matrix
		shaderProgram->setVec3F("lightPos", lightPos);
		shaderProgram->setMat4F("projection", projection);
		shaderProgram->setMat4F("lightSpaceMatrix", lightCompleteMat);
		shaderProgram->setInt("shadowMap", 0);
		shaderProgram->setInt("albedoTexture", 1);
		shaderProgram->setInt("normalTexture", 2);
		shaderProgram->setFloat("bumpiness", bumpiness);

		// we update the view matrix
		cameraPath->getNextStep(deltaTime, cameraSpeed);
		view = glm::lookAt(cameraPath->getThisPostion(),
			cameraPath->getThisPostion() + cameraPath->getThisRotation(),
			glm::vec3(0.0f, 1.0f, 0.0f));
		shaderProgram->setMat4F("view", view);
		shaderProgram->setVec3F("viewPos", cameraPath->getThisPostion());

		// we calculate the transformation matrixes of all the cubes
		for (int i = 0; i < 20; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			shaderProgram->setMat4F("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(3, 0, -15));
		model = glm::rotate(model, 45.0f, glm::vec3(0, 1, 0));
		model = glm::scale(model, glm::vec3(10, 10, 1));
		shaderProgram->setMat4F("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// code to update the Viewport if the window changes size
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	// code to quit if ESC is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void speedCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		cameraSpeed += 0.5f;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		if (cameraSpeed - 0.5f >= 0)
			cameraSpeed -= 0.5f;
	}
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
		if (bumpiness + 0.1f <= 1)
			bumpiness += 0.1f;
	}
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
		if (bumpiness - 0.1f >= 0)
			bumpiness -= 0.1f;
	}

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		aaMode = (aaMode + 1) % 8;
		resetWindow = true;
	}
}