#pragma once
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

const int PATH_LENGHT = 20;
const float B = 0.0f, TEN = 0.0f, C = 0.0f;

class CameraMovement {
private:
	const glm::vec3 lookDirections[PATH_LENGHT] = {
	glm::vec3(0, 0, -1),
	glm::vec3(0.7, 0, -1),
	glm::vec3(0.5, 0, -1),
	glm::vec3(0.3, 0, -1),
	glm::vec3(0.1, 0, -1),
	glm::vec3(0, 0, -1),
	glm::vec3(-0.1, 0, -1),
	glm::vec3(-0.4, 0, -1),
	glm::vec3(-0.6, 0, -1),
	glm::vec3(-0.9, 0, -1),
	glm::vec3(-1, 0, -1),
	glm::vec3(-1, 0, -0.8),
	glm::vec3(-1, 0, -0.5),
	glm::vec3(-1, 0, -0.2),
	glm::vec3(-1, 0, 0),
	glm::vec3(-1, 0, 0.1),
	glm::vec3(-1, 0, 0.5),
	glm::vec3(-1, 0, 1),
	glm::vec3(-0.5, 0, 1),
	glm::vec3(0, 0, 1)
	};

	const glm::vec3 cameraPath[PATH_LENGHT] = {
	glm::vec3(1.0f,  1.0f,  10.0f),
	glm::vec3(1.0f,  0.0f, 2.0f),
	glm::vec3(1.0f, 1.0f, 4.0f),
	glm::vec3(1.0f,  2.0f,  6.0f),
	glm::vec3(1.0f,  1.0f, 8.0f),
	glm::vec3(5.0f,  1.0f,  0.0f),
	glm::vec3(5.0f,  0.0f, 2.0f),
	glm::vec3(5.0f, 1.0f, 4.0f),
	glm::vec3(5.0f,  2.0f,  6.0f),
	glm::vec3(5.0f,  1.0f, 8.0f),
	glm::vec3(5.0f,  1.0f,  -10.0f),
	glm::vec3(5.0f,  0.0f, -2.0f),
	glm::vec3(5.0f, 1.0f, -4.0f),
	glm::vec3(5.0f,  2.0f,  -6.0f),
	glm::vec3(5.0f,  1.0f, -8.0f),
	glm::vec3(1.0f,  1.0f,  -10.0f),
	glm::vec3(1.0f,  0.0f, -2.0f),
	glm::vec3(1.0f, 1.0f, -4.0f),
	glm::vec3(1.0f,  2.0f,  -6.0f),
	glm::vec3(1.0f,  1.0f, -8.0f),
	};
	
	glm::quat lookDirQuaternions[PATH_LENGHT];

	std::vector<glm::vec3> tangents;
	glm::quat helpQuat1;
	glm::quat helpQuat2;

	glm::vec3 thisPoint = cameraPath[0];
	glm::quat thisRot;

	float t;
	float thisDistance;
	int pathIndex;
	glm::vec3 lastPoint;
	glm::vec3 initialOrientation;
	void UpdateInterpolationData();
	glm::vec3 calculatePoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 m0, glm::vec3 m1);
	std::vector<glm::vec3> calculateTangents(float t, float b, float c, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
public:
	CameraMovement();
	void getNextStep(float deltaTime, float speed);
	glm::vec3 getThisPostion() {
		return thisPoint;
	}
	glm::vec3 getThisRotation() {
		return thisRot * initialOrientation;
	}

};