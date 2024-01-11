#include "MyWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

MyWindow::MyWindow(int screenWidth, int screenHeight, int AntiAliasing) {
	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;
	this->AntiAliasingMode = AntiAliasing;
	deltaTime = 0.0f;
	lastFrame = 0.0f;

	// initialize things
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES, AntiAliasingMode);

	window = glfwCreateWindow(screenWidth, screenHeight, "RenderingEngine", NULL, NULL); 
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
	glViewport(0, 0, screenWidth, screenHeight);

	// we enable the depht buffer
	glEnable(GL_DEPTH_TEST);

	// activate antialiasing
	glEnable(GL_MULTISAMPLE);

	// to enable the changing of the point size when rendering using GL_POINTS
	glEnable(GL_PROGRAM_POINT_SIZE);

	printf("%s\n", glGetString(GL_VERSION));
}

void MyWindow::calculateDeltaTime() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	std::cout << "FPS: " << 1.0f / deltaTime << "\r" <<std::flush;
}

void MyWindow::initializeDraw(float r, float g, float b){
	glClearColor(r,g,b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void MyWindow::finalizeDraw() const {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void MyWindow::drawMeshes(const std::vector<Object>& objects, const glm::mat4& viewMat, const glm::mat4& perspMat) const {
	for (int i = 0; i < objects.size(); i++) {
		objects[i].draw(viewMat, perspMat);
	}
}

void MyWindow::drawMesh(const Object& object, const glm::mat4& viewMat, const glm::mat4& perspMat) const {
	object.draw(viewMat, perspMat);
}

void MyWindow::drawMeshAsPoints(const Object& object, const glm::mat4& viewMat, const glm::mat4& perspMat) const {
	object.drawAsPoints(viewMat, perspMat);
}

void MyWindow::drawMeshAsPatch(const Object& object, const glm::mat4& viewMat, const glm::mat4& perspMat) const {
	object.drawAsPatch(viewMat, perspMat);
}

void MyWindow::activateWireframe(bool activate) const {
	if(activate)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void MyWindow::useThis3DFrameBuffer(const FrameBuffer3D& framebuffer) {
	glViewport(0, 0, framebuffer.getWidth(), framebuffer.getLength());
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.getFBIndex());
}

void MyWindow::useThisDepthFrameBuffer(const DepthFrameBuffer& framebuffer) {
	glViewport(0, 0, framebuffer.getWidth(), framebuffer.getHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.getFBIndex());
}

void MyWindow::useDefaultFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);
}

glm::vec3 MyWindow::CreateRay(const glm::mat4& projection, const glm::mat4& view) {
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	float x = (2.0f * mouseX) / screenWidth - 1.0f;
	float y = 1.0f - (2.0f * mouseY) / screenHeight;
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

