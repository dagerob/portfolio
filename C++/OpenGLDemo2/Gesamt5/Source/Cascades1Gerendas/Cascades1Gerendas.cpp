// Cascades1Gerendas.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "commom_header.h"
#include "MyWindow.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Object.h"
#include "3DFrameBuffer.h"
#include "Particles.h"
#include "DepthFrameBuffer.h"

#ifdef DEBUG
const int INITIAL_MESH_WIDTH = 2;
const int INITIAL_MESH_LENGTH = 2;
const int INITIAL_MESH_HEIGHT = 2;
#endif // DEBUG

#ifndef DEBUG
const int INITIAL_MESH_WIDTH = 96;
const int INITIAL_MESH_LENGTH = 96;
const int INITIAL_MESH_HEIGHT = 255;
#endif // DEBUG

const int INITIAL_MESH_SIZE = INITIAL_MESH_WIDTH * INITIAL_MESH_LENGTH * INITIAL_MESH_HEIGHT;
const int SHADOW_SIZE = 1024;

// object containing the window and the OpenGL initialization
MyWindow myWindow(800, 600, 0);

// the shadersused
Shader densityRendererShader("DensityVertexShader.vert", "DensityFragmentShader.frag");
Shader displacementShader("DisplayVertexShader.vert", "DisplayFragmentShader.frag");
Shader marchingCubesShader("MarchingCubesVertexShader.vert","MarchingCubesFragmentShader.frag", "MarchingCubesGeometryShader.geom");
Shader lightShader("DisplayVertexShader.vert", "LightFragShader.frag");
Shader shadowRendererShader("ShadowRendererShader.vert", "ShadowRendererShader.frag");
Shader tesseletionShader("TesselationVertexShader.vert", "TesseletionFragmentShader.frag", "TesselationControlShader.tesc","TesselationEvaluationShader.tese",3);

// the light position
glm::vec3 lightPosition(5, 8, 5);

// the meshes and coords used
float quadSliceCorners[] = {
	//positions				  //normals				  //uvs				//tangents
	-1, -1,  0,	  0.0f,  0.0f, 1.0f,	  0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	 1, -1,  0,	  0.0f,  0.0f, 1.0f,	  1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
	 1,  1,  0,	  0.0f,  0.0f, 1.0f,	  1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	
	 1, 1,  0,	  0.0f,  0.0f, 1.0f,	  1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	 -1, 1, 0,	  0.0f,  0.0f, 1.0f,	  0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
	 -1, -1, 0,	  0.0f,  0.0f, 1.0f,	  0.0f, 0.0f,		1.0f, 0.0f, 0.0f
};
Mesh triangleMesh(quadSliceCorners, 6, 66);

float rocksCoords[INITIAL_MESH_WIDTH][INITIAL_MESH_LENGTH][INITIAL_MESH_HEIGHT][11];

// the objects to be drawn
// just a quad to use as "canvas" to render the density to
Object triangleObject(&triangleMesh, std::make_shared<Shader>(densityRendererShader), glm::vec3(0.0f), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
// the wall with the final shader, just a Quad
Object wallObject(&triangleMesh, std::make_shared<Shader>(displacementShader), glm::vec3(0,0,-100), glm::vec3(0, 0, 0), glm::vec3(50, 50, 1));
// an occluder that casts a shadow on the wall
Object occluderObject(&triangleMesh, std::make_shared<Shader>(tesseletionShader), glm::vec3(0, 0, -80), glm::vec3(1, 0, 0), glm::vec3(10, 10, 1));
Object occluderObject2(&triangleMesh, std::make_shared<Shader>(tesseletionShader), glm::vec3(15, -30, -50), glm::vec3(1, 0, 1), glm::vec3(10, 10, 1));
// to show where the light is
Object lightObject(&triangleMesh, std::make_shared<Shader>(lightShader), lightPosition, glm::vec3(0,0, 0), glm::vec3(10, 10, 1));


Camera camera(glm::vec3(0, 0, 3));

float lastX = myWindow.getWidth() / 2.0f;
float lastY = myWindow.getHeight() / 2.0f;
bool firstMouse = true;
bool wireframe = false;

// density texture and Buffer
FrameBuffer3D densityBuffer(INITIAL_MESH_WIDTH, INITIAL_MESH_LENGTH, INITIAL_MESH_HEIGHT);

// textures for testing
Texture diffuseTexture("brickdiff.jpg");
Texture bumpTexture("brickbump.jpg");
Texture normalTexture("bricknorm.jpg");
Texture roughnessTexture("brickrough.jpg");

// Partyclesystem
ParticleSystem particles1;
ParticleSystem particles2;
ParticleSystem particles3;

// shadows
DepthFrameBuffer depthBuffer(SHADOW_SIZE, SHADOW_SIZE);

// other variables
float zHeight = 0;
bool useDisplacement = true;
int steps = 20;
bool madeClickThisFrame = false;
glm::vec3 particleGenPosition = glm::vec3(10, 0, 1);
float tl = 1;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// code to update the Viewport if the window changes size
	glViewport(0, 0, width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Here comes the code to get keyboard inputs
	// toggle the Wireframe mode
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		wireframe = !wireframe;
		myWindow.activateWireframe(wireframe);
	}
	else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(myWindow.getWindow(), true);
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
		useDisplacement = !useDisplacement;
	}else if (key == GLFW_KEY_O && action == GLFW_PRESS){
		steps++;
	}else if (key == GLFW_KEY_P && action == GLFW_PRESS && steps > 0){
		steps--;
	}

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// Here comes the code to get mouse buttons inputs
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		madeClickThisFrame = true;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void setupCallbacks() {
	glfwSetFramebufferSizeCallback(myWindow.getWindow(), framebuffer_size_callback);
	// we set the callback for the speed input
	glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
	// set click callback
	glfwSetMouseButtonCallback(myWindow.getWindow(), mouse_button_callback);
	// set the mpuse ps callback
	glfwSetCursorPosCallback(myWindow.getWindow(), mouse_callback);
	// set the mouse scroll callback
	glfwSetScrollCallback(myWindow.getWindow(), scroll_callback);
}

// this function should be called every draw cycle
void processInput()
{
	if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, myWindow.getDeltaTime());
	if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, myWindow.getDeltaTime());
	if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, myWindow.getDeltaTime());
	if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, myWindow.getDeltaTime());
	if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, myWindow.getDeltaTime());
	if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, myWindow.getDeltaTime());
	if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_UP) == GLFW_PRESS)
		zHeight += 0.1f;
	if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS)
		zHeight -= 0.1f;
	if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_O) == GLFW_PRESS)
		tl += 0.01f;
	if (glfwGetKey(myWindow.getWindow(), GLFW_KEY_P) == GLFW_PRESS)
		tl -= 0.01f;
}

int main()
{
	//----------------
	// inuitilize Particles
	particles1.InitalizeParticleSystem();
	particles1.SetGeneratorProperties(
		particleGenPosition, // Where the particles are generated
		glm::vec3(-5, 0, -5), // Minimal velocity
		glm::vec3(5, 20, 5), // Maximal velocity
		glm::vec3(0, -5, 0), // Gravity force applied to particles1
		glm::vec3(0.0f, 0.5f, 1.0f), // Color (light blue)
		1.5f, // Minimum lifetime in seconds
		3.0f, // Maximum lifetime in seconds
		0.1f, // Rendered size
		0.02f, // Spawn every 0.05 seconds
		30); // And spawn 30 particles1

	particles2.InitalizeParticleSystem();
	particles2.SetGeneratorProperties(
		particleGenPosition, // Where the particles are generated
		glm::vec3(-5, 0, -5), // Minimal velocity
		glm::vec3(5, 20, 5), // Maximal velocity
		glm::vec3(0, -2, 0), // Gravity force applied to particles1
		glm::vec3(0.9f, 0.1f, 0.0f), // Color (light blue)
		3.0f, // Minimum lifetime in seconds
		5.0f, // Maximum lifetime in seconds
		0.6f, // Rendered size
		0.1f, // Spawn every 0.05 seconds
		5); // And spawn 30 particles

	particles3.InitalizeParticleSystem();
	particles3.SetGeneratorProperties(
		particleGenPosition, // Where the particles are generated
		glm::vec3(-1, 0, -1), // Minimal velocity
		glm::vec3(1, 3, 1), // Maximal velocity
		glm::vec3(0, -1, 0), // Gravity force applied to particles1
		glm::vec3(0.8f, 0.7f, 0.2f), // Color (light blue)
		1.5f, // Minimum lifetime in seconds
		3.0f, // Maximum lifetime in seconds
		0.2f, // Rendered size
		0.02f, // Spawn every 0.05 seconds
		10); // And spawn 30 particles
	//-----------------------
	camera.MovementSpeed = 100;
	std::vector<float> vectorArrayRocksCoords;
	// setup the initial mesh for the rocks
	for (int i = 0; i < INITIAL_MESH_WIDTH; i++) {
		for (int j = 0; j < INITIAL_MESH_LENGTH; j++) {
			for (int k = 0; k < INITIAL_MESH_HEIGHT; k++) {
				vectorArrayRocksCoords.push_back(i);
				vectorArrayRocksCoords.push_back(j);
				vectorArrayRocksCoords.push_back(k);
				for (int l = 0; l < 8; l++) {
					vectorArrayRocksCoords.push_back(0.0f);
				}
			}
		}
	}

	float* rocksCoords = vectorArrayRocksCoords.data();
	Mesh rocks(rocksCoords, INITIAL_MESH_SIZE, INITIAL_MESH_SIZE * 11);
	Object rocksObject(&rocks, std::make_shared<Shader>(marchingCubesShader), glm::vec3(0, 0,0), glm::vec3(3.1415/2, 0, 0), glm::vec3(1, 1, 1));
	setupCallbacks();
	while (!glfwWindowShouldClose(myWindow.getWindow()))
	{
		// setup window
		processInput();
		myWindow.calculateDeltaTime();
		//glm::mat4 projection = glm::mat4(1.0f);
		//glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)myWindow.getWidth() / (float)myWindow.getHeight(), 0.1f, 50000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		myWindow.initializeDraw(0.2f, 0.3f, 0.3f);

		// render to the texture
		myWindow.useThis3DFrameBuffer(densityBuffer);
		triangleObject.setShaderFloatParameter("screenWidth", densityBuffer.getWidth());
		triangleObject.setShaderFloatParameter("screenHeight", densityBuffer.getLength());
		triangleObject.setShaderFloatParameter("bufferHeight", densityBuffer.getHeight());
		do {
			triangleObject.setShaderFloatParameter("zHeight", densityBuffer.getCurrentLevel());
			triangleObject.setShaderFloatParameter("cameraHeight", zHeight);
			myWindow.drawMesh(triangleObject, view, projection);
		} while (!densityBuffer.moveToNextLevel());
		myWindow.useDefaultFrameBuffer();

		// render using the marching cubes
		rocksObject.setTexture3D(densityBuffer.getTextureIndex(), 0, "density");
		rocksObject.setShaderFloatParameter("bufferlength", INITIAL_MESH_LENGTH);
		rocksObject.setShaderFloatParameter("bufferwidth", INITIAL_MESH_WIDTH);
		rocksObject.setShaderFloatParameter("bufferheight", INITIAL_MESH_HEIGHT);
		myWindow.drawMeshAsPoints(rocksObject, view, projection);

		// SHADOWS =========================================================
		// change the shader of the objects to the shadow renderer
		wallObject.setShader(std::make_shared<Shader>(shadowRendererShader));
		occluderObject.setShader(std::make_shared<Shader>(shadowRendererShader));
		occluderObject2.setShader(std::make_shared<Shader>(shadowRendererShader));
		
		// we create the lightspace view matrix
		// we use a orthographic projection to render the shadows, assuming the light is very far away
		float near_plane = 1.0f, far_plane = 200;
		glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
		// we create the transform to move to the light point of view using the LookAt function
		glm::mat4 lightView = glm::lookAt(lightPosition,
			lightPosition + glm::vec3(0.2f, 0.6f,-1),
			glm::vec3(0, 1, 0));

		// we can combine both to obtain the whole transform in order to render from the light's POV
		glm::mat4 lightCompleteMat = lightProjection * lightView;

		//// we tell the program to render to this frame buffer
		myWindow.useThisDepthFrameBuffer(depthBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		wallObject.setMat4FParameter("lightSpaceMatrix", lightCompleteMat);
		occluderObject.setMat4FParameter("lightSpaceMatrix", lightCompleteMat);
		occluderObject2.setMat4FParameter("lightSpaceMatrix", lightCompleteMat);
		myWindow.drawMesh(wallObject, view, projection);	
		myWindow.drawMesh(occluderObject, view, projection);
		myWindow.drawMesh(occluderObject2, view, projection);

		wallObject.setShader(std::make_shared<Shader>(displacementShader));
		occluderObject.setShader(std::make_shared<Shader>(tesseletionShader));
		occluderObject2.setShader(std::make_shared<Shader>(tesseletionShader));

		myWindow.useDefaultFrameBuffer();
		//======================================================================
		
		// render the wall with displacement mapping
		wallObject.setTexture2D(diffuseTexture.getTextureIndex(), 0, "albedoTexture");
		wallObject.setTexture2D(bumpTexture.getTextureIndex(), 1, "bumpTexture");
		wallObject.setTexture2D(normalTexture.getTextureIndex(), 2, "normalTexture");
		wallObject.setTexture2D(roughnessTexture.getTextureIndex(), 3, "roughTexture");
		wallObject.setTexture2D(depthBuffer.getTextureIndex(), 4, "shadowMap");
		wallObject.setMat4FParameter("lightSpaceMatrix", lightCompleteMat);
		wallObject.setVec3FParameter("lightPos", lightPosition);
		wallObject.setVec3FParameter("viewPos", camera.Position);
		wallObject.setVec3FParameter("cameraFront", camera.Front);
		wallObject.setShaderFloatParameter("bumpiness", 0.0f);
		wallObject.setShaderFloatParameter("duv", 0.001f);
		wallObject.setShaderIntParameter("steps", steps);
		wallObject.setBoolParameter("useDisplacement", useDisplacement);
		myWindow.drawMesh(wallObject, view, projection);

		// render the occluder with displacement mapping
		occluderObject.setTexture2D(diffuseTexture.getTextureIndex(), 0, "albedoTexture");
		occluderObject.setTexture2D(normalTexture.getTextureIndex(), 2, "normalTexture");
		occluderObject.setTexture2D(roughnessTexture.getTextureIndex(), 3, "roughTexture");
		occluderObject.setMat4FParameter("lightSpaceMatrix", lightCompleteMat);
		occluderObject.setVec3FParameter("lightPos", lightPosition);
		occluderObject.setVec3FParameter("viewPos", camera.Position);
		occluderObject.setShaderFloatParameter("tl", tl);
		myWindow.drawMeshAsPatch(occluderObject, view, projection);

		// render the occluder with displacement mapping
		occluderObject2.setTexture2D(diffuseTexture.getTextureIndex(), 0, "albedoTexture");
		occluderObject2.setTexture2D(normalTexture.getTextureIndex(), 2, "normalTexture");
		occluderObject2.setTexture2D(roughnessTexture.getTextureIndex(), 3, "roughTexture");
		occluderObject2.setMat4FParameter("lightSpaceMatrix", lightCompleteMat);
		occluderObject2.setVec3FParameter("lightPos", lightPosition);
		occluderObject2.setVec3FParameter("viewPos", camera.Position);
		occluderObject2.setShaderFloatParameter("tl", tl);
		myWindow.drawMeshAsPatch(occluderObject2, view, projection);

		// render light
		myWindow.drawMesh(lightObject, view, projection);
		

		// get hits if there was a click
		if (madeClickThisFrame) {
			glm::vec3 ray = myWindow.CreateRay(projection, view);
			glm::vec3 hit;
			if (wallObject.isHitByRay(camera.Position, ray, hit)) {
				printf("hit!!\n");
				particleGenPosition = hit;
				particles1.SetGenPosition(particleGenPosition);
				particles2.SetGenPosition(particleGenPosition);
				particles3.SetGenPosition(particleGenPosition);
			}
			madeClickThisFrame = false;
		}

		// render the particles1
		particles1.SetMatrices(&projection, camera);
		particles1.UpdateParticles(myWindow.getDeltaTime());
		particles1.RenderParticles();

		particles2.SetMatrices(&projection, camera);
		particles2.UpdateParticles(myWindow.getDeltaTime());
		particles2.RenderParticles();

		particles3.SetMatrices(&projection, camera);
		particles3.UpdateParticles(myWindow.getDeltaTime());
		particles3.RenderParticles();
	
		// finish and swap buffers
		myWindow.finalizeDraw();
	}
	glfwTerminate();
}