#pragma once
#include "commom_header.h"
#include "Shader.h"

struct Triangle {
	glm::vec4 corners[3];
	glm::vec3 getCorner(int i) const {
		return glm::vec3(corners[i].x, corners[i].y, corners[i].z);
	};
};

class Mesh {
	unsigned int VBOindex, VAOindex;
	float* coordinates;
	int vertexCount;
	bool testIntersection(const Triangle& triangle, glm::vec3 origin, glm::vec3 direction, glm::vec3& intersection);
	float orient(const glm::vec3& a, const  glm::vec3& b, const  glm::vec3& c, const  glm::vec3& d);
public:
	Mesh(float* coordinates, int vertexCount, int dataLenght);
	Mesh(const Mesh& other) {
		VBOindex = other.VBOindex;
		VAOindex = other.VAOindex;
		coordinates = other.coordinates;
		vertexCount = other.vertexCount;
	}
	Mesh& operator=(const Mesh& other) {
		if (this == &other) {
			return *this;
		}
		VBOindex = other.VBOindex;
		VAOindex = other.VAOindex;
		coordinates = other.coordinates;
		vertexCount = other.vertexCount;
		return *this;
	}
	void draw() const;
	void drawAsPoints() const;
	void drawAsPatch() const;
	bool getIntersection(glm::vec3 origin, glm::vec3 direction, glm::mat4 modelMat, glm::vec3& hit);
};