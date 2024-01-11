#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "CameraMovement.h"
#include "Box.h"
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Triangle.h"
#include "KDTree.h"
#include <string>
#include "Timing.h"
#include <fstream>
#include <sstream>
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
const unsigned int SCRN_WIDTH = 800, SCRN_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
void processInput(GLFWwindow *window);

void init();	// method to initializa OpenGL
void display();	// draw method that updates the viewport every frame
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);	// callback to get keyboard input
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);		// callback to get mouse input
glm::vec3 CreateRay(const glm::mat4& projection, const glm::mat4& view);	// method that creates a vector pointing in the direction of a ray generated from the camera position in direction of the last click
int minVal = -50, maxVal = 50; // in between these all the triangles will be generated
bool performance = false;	// if it should perfomr the timing calculations
std::string filename = "";	// the name of the file to be loaded, if empty will randomly generate triangles

GLFWwindow* window;	// wht GLFW window object
Shader* shaderProgram;	// the shader object to render using textures, normalmap and shadows
Shader* depthShaderProgram;	// the shader to generate the shadowmaps
Shader* pointShaderProgram;	// simple shader to paint polygons with a single color
float bumpiness = 0.0f;	
bool error = false;
// vertex array objects to paint in the scene
unsigned int VBOTriangle, VAOTriangle;
unsigned int VBOPoint, VAOPoint;
unsigned int VBOCube, VAOCube;
unsigned int depthMap;	// depth map texture for shadows
unsigned int depthMapFBO; // buffer for the sahdows
unsigned int textureAlbedo; // texture to save the albedo information of the objects
unsigned int textureNormal; // texture storing the normal vectors information
int aaMode = 0;	// which antialiasing mode will be used
bool resetWindow = false;

// to maintain constant time
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// the position of the camera, useful to calculate the ray
float cameraPosX = 0.0f, cameraPosZ = 0.0f, cameraPosY = 0.0f;
// amount of triangles to be generated
int triangleAmount = 100000000;
// the array to store the triangles to be rendered
std::vector<Triangle> triangles;

// coordinates of the mouse cursor
double mouseX, mouseY;
// coordinates of las click evente registered
double clickX = -10, clickY = -10;

// vertex to represent a point
float point[3] = { 0.0f, 0.0f,  0.0f};

// variable to know if the grid should be displayed
bool showGrid = false;
// the last triangle found with the raycast
Triangle* lastResult;

// timing class for measurements
Timing* timer;
// the KD Tree that stores the triangles and allos faster traversal
KDTree tree;

void init() {
	// initialize things
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES,aaMode);

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
	glfwSetKeyCallback(window, keyboardCallback);
	// set click callback
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// we load and compile the vertex and fragment shader to display the cubes in the scene
	shaderProgram = new Shader("VertexShader.vert","FragmentShader.frag");
	depthShaderProgram = new Shader("SimpleDepthShader.vert", "EmptyShader.frag");
	pointShaderProgram = new Shader("SimpleVertexShader.vert", "SimpleColorFragmentShader.frag");

	// we enable the depht buffer
	glEnable(GL_DEPTH_TEST);

	// we create the OpenGL objects to store the vertex and normal data of the cubes
	glGenVertexArrays(1, &VAOTriangle);
	glGenBuffers(1, &VBOTriangle);

	// we load the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBOTriangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle::mesh), Triangle::mesh, GL_STATIC_DRAW);
	
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
	glBindVertexArray(VAOTriangle);


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

	// we set each texture to each TEXTURE on the GPU
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

	// we configurate the point VAO of the point
	glGenVertexArrays(1, &VAOPoint);
	glGenBuffers(1, &VBOPoint);
	glBindVertexArray(VAOPoint);
	glBindBuffer(GL_ARRAY_BUFFER, VBOPoint);
	glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// we configurate the cube VAO for the boxes
	glGenVertexArrays(1, &VAOCube);
	glGenBuffers(1, &VBOCube);
	glBindVertexArray(VAOCube);
	glBindBuffer(GL_ARRAY_BUFFER, VBOCube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Box::points), Box::points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// activate antialiasing
	glEnable(GL_MULTISAMPLE);

	// to enable the changing of the point size when rendering using GL_POINTS
	glEnable(GL_PROGRAM_POINT_SIZE);
}

void display() {
	// we initialize some matrixes
	// the view matrix
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection;
	// the projection matrix has to be made only once
	projection = glm::perspective(glm::radians(-45.0f), (float) SCRN_WIDTH / SCRN_HEIGHT, 0.1f, 100.0f);

	// the position of the light
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
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// we tell OpenGL which VAO we want to draw
		glBindVertexArray(VAOTriangle);
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

		// we make the render on the shadow map
		for (auto triangle : triangles) {
			depthShaderProgram->setMat4F("model", triangle.getModelMat());
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		// Make the Normal Render, incorporating shadows
		// we clear last scene and paint the background with black
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCRN_WIDTH, SCRN_HEIGHT);
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
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
		view = glm::lookAt(glm::vec3(cameraPosX, cameraPosY, cameraPosZ),
			glm::vec3(cameraPosX, cameraPosY, cameraPosZ) + glm::vec3(0.0f, 0.0f, -1.0f),
			glm::vec3(0.0f, 1.0f, 0.0f));
		shaderProgram->setMat4F("view", view);
		shaderProgram->setVec3F("viewPos", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		// we make the render to the framebuffer
		for (auto triangle : triangles) {
			shaderProgram->setMat4F("model", triangle.getModelMat());
			glDrawArrays(GL_TRIANGLES, 0, 3);
		}

		// we test if we need to create a new raycast and if so search the tree for a hit
		if (clickX > 0 && clickY > 0 && clickX < SCRN_WIDTH && clickY < SCRN_HEIGHT) {	
			// start by creating the ray
			glm::vec3 ray = CreateRay(projection, view);

			// get the values needed
			float camPos[3] = { cameraPosX, cameraPosY, cameraPosZ };
			float rayDir[3] = { ray.x, ray.y, ray.z };
			
			// if time needs to be measured, start doing it
			if (performance) {
				timer->clearRecords();
				timer->startRecord("Traversing Time KD");
			}
			// search for a hit in the tree with the given position, direction and a tmax of 100
			Triangle* result= tree.searchHit(camPos, rayDir, 100);
			// stop the time measuremnt
			if (performance) {
				timer->stopRecord("Traversing Time KD");
			}
			// if something was found
			if (result != nullptr) {
				// display in the console and save as lastResult
				std::cout << "\nhit" << std::endl;
				lastResult = result;
			}

			// if measurement is enable look for the same triangle using brute force
			if (performance) {
				timer->startRecord("Traversing Time Brute");
				for (auto triangle : triangles) {
					glm::vec3 intersection;
					if (tree.testIntersection(triangle, glm::vec3(cameraPosX, cameraPosY, cameraPosZ), ray, intersection)) 
						break;
				}
				timer->stopRecord("Traversing Time Brute");
				timer->print();
			}

			// reset the last click
			clickX = -10;
			clickY = -10;
			// print the point found
			std::cout << tree.lastPoint.x << " " << tree.lastPoint.y << " " << tree.lastPoint.z << std::endl;
		}
		
		// if the last point is no 0, then something was hit
		if (tree.lastPoint.x != 0 && tree.lastPoint.y != 0 && tree.lastPoint.z != 0) {
			// draw a green triangle over the real one
			pointShaderProgram->use();
			pointShaderProgram->setMat4F("projection", projection);
			pointShaderProgram->setMat4F("view", view);
			pointShaderProgram->setVec3F("color", glm::vec3(0.0f, 1.0f, 0.0f));
			pointShaderProgram->setMat4F("model", lastResult->getModelMat(0.01));
			glDrawArrays(GL_TRIANGLES, 0, 3);
			pointShaderProgram->setMat4F("model", lastResult->getModelMat(-0.01));
			glDrawArrays(GL_TRIANGLES, 0, 3);

			// draw a point on the coordinates of the last found point
			glBindVertexArray(VAOPoint);
			pointShaderProgram->use();
			pointShaderProgram->setMat4F("projection", projection);
			pointShaderProgram->setMat4F("view", view);
			glm::mat4 model = glm::mat4(1.0f);
			// we draw the intersection point
			model = glm::translate(model, tree.lastPoint);
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.01f));
			pointShaderProgram->setVec3F("color", glm::vec3(1.0f, 0.0f, 0.0f));
			pointShaderProgram->setMat4F("model", model);
			glDrawArrays(GL_POINTS, 0, 1);
		}

		// if show Grid is set, draw the boxes as wireframe
		if (showGrid) {
			glBindVertexArray(VAOPoint);
			pointShaderProgram->use();
			pointShaderProgram->setMat4F("projection", projection);
			pointShaderProgram->setMat4F("view", view);
			glm::mat4 model = glm::mat4(1.0f);
			glBindVertexArray(VAOCube);
			// traverse all the boxes saved in the tree structure
			for (auto box : tree.boxes) {
				model = glm::mat4(1.0f);
				box.getTransformMatrix(model);
				pointShaderProgram->setVec3F("color", glm::vec3(0.0f, 0.0f, 1.0f));
				pointShaderProgram->setMat4F("model", model);
				glDrawArrays(GL_LINE_STRIP, 0, 16);
			}
		}

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

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPosX += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPosX -= 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPosZ -= 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPosZ += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		cameraPosY -= 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		cameraPosY += 0.1f;
	}
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
		showGrid = !showGrid;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		clickX = mouseX;
		clickY = mouseY;
	}
		
}

glm::vec3 CreateRay(const glm::mat4& projection, const glm::mat4& view) {
	float x = (2.0f * clickX) / SCRN_WIDTH - 1.0f;
	float y = 1.0f - (2.0f * clickY) / SCRN_HEIGHT;
	float z = 1.0f;
	glm::vec3 ray_nds = glm::vec3(x, y, z);

	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);

	glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;

	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	glm::vec4 result = (glm::inverse(view) * ray_eye);
	glm::vec3 ray_wor = glm::vec3(result.x, result.y, result.z);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);
	return ray_wor;
}

void printUsage() {
	std::cerr << "Usage: Aufgabe1.exe --samples [sampling mode]" << std::endl;
}

static std::vector<Triangle> loadTringlesPositions(std::string fileName)
{
	std::vector<Triangle> newTriangles;

	std::ifstream input(fileName);
	if (input.is_open() && input.good())
	{
		input >> maxVal;
		minVal = -maxVal;

		float x, y, z, roll, pitch, yaw;
		char comma;
		while (input >> x >> comma >> y >> comma >> z >> comma >> roll >> comma >> pitch >> comma >> yaw)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(x, y, z));
			model = glm::rotate(model, roll, glm::vec3(1, 0, 0));
			model = glm::rotate(model, pitch, glm::vec3(0, 1, 0));
			model = glm::rotate(model, yaw, glm::vec3(0, 0, 1));
			newTriangles.push_back(Triangle(model));
		}
	}
	else
	{
		std::cout << "Input file could not be opened. Invalid filename or destination";
		exit(0);
	}
	input.close();

	return newTriangles;
}

int main(int argc, char* argv[])
{
	// For testing
	triangleAmount = 10000;
	maxVal = 100;
	minVal = -maxVal;
	performance = true;
	filename = "";

	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "--triangles") {
			if (i + 1 < argc) {
				if (std::stoi(argv[i + 1]) >= 0) {
					triangleAmount = std::stoi(argv[i + 1]);
				}
				else {
					printUsage();
					return 1;
				}
			}
			else {
				printUsage();
				return 1;
			}
		}
		else if (std::string(argv[i]) == "--extremes") {
			if (i + 1 < argc) {
				if (std::stoi(argv[i + 1]) >= 0) {
					maxVal = std::stoi(argv[i + 1]);
					minVal = -maxVal;
				}
				else {
					printUsage();
					return 1;
				}
			}
			else {
				printUsage();
				return 1;
			}
		}
		else if (std::string(argv[i]) == "--filename") {
			if (i + 1 < argc) {
				filename = argv[i + 1];
			}
			else {
				printUsage();
				return 1;
			}
		}
		else if (std::string(argv[i]) == "--performance") {
			performance = true;
		}
	}
	

	// Initilaize Timing class
	timer = Timing::getInstance();

	if (filename != "") {
		triangles = loadTringlesPositions(filename);
	}
	else {
		// Create random Triangles
		std::default_random_engine e1(1234);
		std::uniform_int_distribution<int> uniform_dist(minVal, maxVal);

		for (int i = 0; i < triangleAmount; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(uniform_dist(e1), uniform_dist(e1), uniform_dist(e1)));
			model = glm::rotate(model, (float)uniform_dist(e1), glm::vec3(1, 0, 0));
			model = glm::rotate(model, (float)uniform_dist(e1), glm::vec3(0, 1, 0));
			model = glm::rotate(model, (float)uniform_dist(e1), glm::vec3(0, 0, 1));
			triangles.push_back(Triangle(model));
		}
	}

	// populate the kd Tree
	if (performance)
		timer->startRecord("Build Time");
	tree = KDTree(triangles, minVal, maxVal);
	if (performance) {
		timer->stopRecord("Build Time");
		timer->print();
	}

	resetWindow = true;
	while (resetWindow) {
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