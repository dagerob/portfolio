#include "CameraMovement.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

CameraMovement::CameraMovement() {
	// we create the array of quaternions represening the lookDirections stored in the array of vec3s
	initialOrientation = glm::vec3(0, 0, -1);
	for (int i = 0; i < PATH_LENGHT; i++) {
		lookDirQuaternions[i] = glm::rotation(glm::normalize(initialOrientation), glm::normalize(lookDirections[i]));
	}

	// we initialize all data for the interpolation
	t = 0.0f;
	pathIndex = -1;
	lastPoint = cameraPath[0];
	thisDistance = 0.0f;
	UpdateInterpolationData();
}

std::vector<glm::vec3> CameraMovement::calculateTangents(float t, float b, float c, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	std::vector<glm::vec3> result(2);

	float coef1, coef2, coef3, coef4;
	coef1 = (1 - t)*(1 + b)*(1 + c) / 2;
	coef2 = (1 - t)*(1 - b)*(1 - c) / 2;
	coef3 = (1 - t)*(1 + b)*(1 - c) / 2;
	coef4 = (1 - t)*(1 - b)*(1 + c) / 2;

	result[0] = coef1 * (p1 - p0) + coef2 * (p2 - p1);
	result[1] = coef3 * (p2 - p1) + coef4 * (p3 - p2);
	return result;
}

glm::vec3 CameraMovement::calculatePoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 m0, glm::vec3 m1) {
	float h00, h10, h01, h11;
	h00 = (1 + 2 * t)*(1 - t)*(1 - t);
	h10 = t * (1 - t)*(1 - t);
	h01 = t * t*(3 - 2 * t);
	h11 = t * t*(t - 1);
	return h00 * p0 + h10 * m0 + h01 * p1 + h11 * m1;
}

void CameraMovement::UpdateInterpolationData() {
	if (pathIndex < PATH_LENGHT - 2) {
		pathIndex++;
		if (pathIndex == 0) {
			tangents = calculateTangents(B, TEN, C, cameraPath[0], cameraPath[0], cameraPath[1], cameraPath[2]);
			helpQuat1 = glm::intermediate(lookDirQuaternions[0], lookDirQuaternions[0], lookDirQuaternions[1]);
			helpQuat2 = glm::intermediate(lookDirQuaternions[0], lookDirQuaternions[1], lookDirQuaternions[2]);
		}
		else if (pathIndex == PATH_LENGHT - 2) {
			tangents = calculateTangents(B, TEN, C, cameraPath[pathIndex - 1], cameraPath[pathIndex], cameraPath[pathIndex + 1], cameraPath[pathIndex + 1]);
			helpQuat1 = glm::intermediate(lookDirQuaternions[pathIndex - 1], lookDirQuaternions[pathIndex], lookDirQuaternions[pathIndex + 1]);
			helpQuat2 = glm::intermediate(lookDirQuaternions[pathIndex], lookDirQuaternions[pathIndex + 1], lookDirQuaternions[pathIndex + 1]);
		}
		else {
			tangents = calculateTangents(B, TEN, C, cameraPath[pathIndex - 1], cameraPath[pathIndex], cameraPath[pathIndex + 1], cameraPath[pathIndex + 2]);
			helpQuat1 = glm::intermediate(lookDirQuaternions[pathIndex - 1], lookDirQuaternions[pathIndex], lookDirQuaternions[pathIndex + 1]);
			helpQuat2 = glm::intermediate(lookDirQuaternions[pathIndex], lookDirQuaternions[pathIndex + 1], lookDirQuaternions[pathIndex + 2]);
		}
	}
	else {
		pathIndex = 0;
		tangents = calculateTangents(B, TEN, C, cameraPath[0], cameraPath[0], cameraPath[1], cameraPath[2]);
		helpQuat1 = glm::intermediate(lookDirQuaternions[PATH_LENGHT - 1], lookDirQuaternions[0], lookDirQuaternions[1]);
		helpQuat2 = glm::intermediate(lookDirQuaternions[0], lookDirQuaternions[1], lookDirQuaternions[2]);
	}
}

void CameraMovement::getNextStep(float deltaTime, float speed) {
	// we have to maintain a contant speed
	// t has no linear relation with how far between the two interpolated points one is
	// we use the general definition ov velocity, velocity is the rate of change of the position
	//		velocity is equal to the vector between point 1 and 2
	// we want to maintain the same speed (magnitude of velocity) 
	// we add a small vale to t until we obtain a point whose distance to the starting pint is equal to the velocity
	// we save that point and t and process the rest
	// if the next point would need a t larger than one, we rest t and calculate the tangents for the next interval
	// we multiply the speed with deltaTime to maintain a normal velocity in different systems and framerates
	while (thisDistance < speed * deltaTime) {
		if (t < 1) {
			t += 0.0001f;
		}
		else {
			t = 0;
			UpdateInterpolationData();
		}
		thisPoint = calculatePoint(t, cameraPath[pathIndex], cameraPath[pathIndex + 1], tangents[0], tangents[1]);
		thisDistance = glm::distance(lastPoint, thisPoint);
	}
	lastPoint = calculatePoint(t, cameraPath[pathIndex], cameraPath[pathIndex + 1], tangents[0], tangents[1]);
	thisDistance = 0.0f;
	// we update the rotation using the same value t
	thisRot = glm::squad(lookDirQuaternions[pathIndex], lookDirQuaternions[pathIndex + 1], helpQuat1, helpQuat1, t);

}