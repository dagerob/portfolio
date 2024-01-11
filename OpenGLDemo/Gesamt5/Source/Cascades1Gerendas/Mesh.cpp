#include "commom_header.h"
#include "Mesh.h"

Mesh::Mesh(float* coordinates, int vertexCount, int dataLenght) {
	this->coordinates = coordinates;
	this->vertexCount = vertexCount;

	// generate the needed buffers
	glGenBuffers(1, &VBOindex);
	glGenVertexArrays(1, &VAOindex);

	//bind these current buffers
	glBindVertexArray(VAOindex);
	glBindBuffer(GL_ARRAY_BUFFER, VBOindex);

	// copy our vertices array in a buffer for OpenGL to use
	glBufferData(GL_ARRAY_BUFFER, dataLenght*sizeof(float), coordinates, GL_STATIC_DRAW);
	
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
}

void Mesh::draw() const {
	glBindVertexArray(VAOindex);
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void Mesh::drawAsPoints() const {
	glBindVertexArray(VAOindex);
	glDrawArrays(GL_POINTS, 0, vertexCount);
}

void Mesh::drawAsPatch() const {
	glBindVertexArray(VAOindex);
	glDrawArrays(GL_PATCHES, 0, vertexCount);
}

bool Mesh::testIntersection(const Triangle& triangle, glm::vec3 origin, glm::vec3 direction, glm::vec3& intersection) {

	const float eps = 0.0001f;
	glm::vec3 edge1 = triangle.getCorner(1) - triangle.getCorner(0);
	glm::vec3 edge2 = triangle.getCorner(2) - triangle.getCorner(0);
	glm::vec3 normal = glm::cross(edge1, edge2);
	glm::normalize(normal);
	float d = glm::dot(-normal, triangle.getCorner(0));

	float check = glm::dot(normal, direction);
	if (std::abs(check) < eps) {
		//std::cout << check << " check\n";
		return false;
	}
	float t = -(glm::dot(normal, origin) + d) / check;
	intersection = origin + t * direction;

	int sideA = 0;
	int sideB = 0;

	for (int i = 0; i < 3; i++) {
		const glm::vec3& a = triangle.getCorner(i);
		const glm::vec3& b = triangle.getCorner((i + 1) % 3);
		const glm::vec3 c = triangle.getCorner(i) + normal;
		float o = orient(a, b, c, intersection);
		if (o < -eps) {
			sideA++;
		}
		else if (o > eps) {
			sideB++;
		}
		else {
			sideA++;
			sideB++;
		}
	}

	if (sideA >= 3 || sideB >= 3) {
		return true;
	}
	return false;
}

float Mesh::orient(const glm::vec3& a, const  glm::vec3& b, const  glm::vec3& c, const  glm::vec3& d) {
	return glm::dot((a - d), glm::cross((b - d), (c - d)));
}

bool Mesh::getIntersection(glm::vec3 origin, glm::vec3 direction, glm::mat4 modelMat, glm::vec3& hit) {
	Triangle* triangles = new Triangle[vertexCount / 3];
	for (int i = 0; i < vertexCount; i+=3) {
		triangles[i/3].corners[0] = modelMat * glm::vec4(coordinates[11*i], coordinates[11*i + 1], coordinates[11*i + 2], 1);
		triangles[i/3].corners[1] = modelMat * glm::vec4(coordinates[11*(i+1)], coordinates[11*(i+1) + 1], coordinates[11*(i+1) + 2],1);
		triangles[i/3].corners[2] = modelMat * glm::vec4(coordinates[11*(i+2)], coordinates[11*(i+2) + 1], coordinates[11*(i+2) + 2],1);
	}
	for (int i = 0; i < vertexCount / 3; i++) {
		if (testIntersection(triangles[i], origin, direction, hit)) {
			return true;
		}
	}
	return false;
}