#pragma once
#include "commom_header.h"
#include "3DFrameBuffer.h"
#include "DepthFrameBuffer.h"
#include "Object.h"

class MyWindow {
	unsigned int screenWidth, screenHeight;
	GLFWwindow* window;
	int AntiAliasingMode;
	bool error = false;
	float lastFrame = 0.0f, deltaTime = 0.0f;
public:
	MyWindow(int screenWidth, int screenHeight, int AntiAliasing);
	GLFWwindow* getWindow() const { return window; };
	int getWidth() const { return screenWidth; };
	int getHeight() const { return screenHeight; };
	void initializeDraw(float r, float g, float b);
	void finalizeDraw() const;
	void drawMeshes(const std::vector<Object>& objects, const glm::mat4& viewMat, const glm::mat4& perspMat) const;
	void drawMesh(const Object& object, const glm::mat4& viewMat, const glm::mat4& perspMat) const;
	void drawMeshAsPoints(const Object& object, const glm::mat4& viewMat, const glm::mat4& perspMat) const;
	void drawMeshAsPatch(const Object& object, const glm::mat4& viewMat, const glm::mat4& perspMat) const;
	void activateWireframe(bool activate) const;
	void calculateDeltaTime();
	float getDeltaTime() { return deltaTime; };
	void useThis3DFrameBuffer(const FrameBuffer3D& frameBuffer);
	void useThisDepthFrameBuffer(const DepthFrameBuffer& frameBuffer);
	void useDefaultFrameBuffer();
	glm::vec3 CreateRay(const glm::mat4& projection, const glm::mat4& view);
};