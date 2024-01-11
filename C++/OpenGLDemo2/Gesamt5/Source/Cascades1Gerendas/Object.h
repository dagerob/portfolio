#pragma once
#include "commom_header.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class Object {
	
public:
	Mesh* mesh;
	std::shared_ptr<Shader> shader;
	glm::mat4 objectMatrix;

	Object(Mesh* mesh, std::shared_ptr<Shader> shader, const glm::mat4& objectMatrix) : mesh(mesh), shader(shader), objectMatrix(objectMatrix) {};
	Object(Mesh* mesh, std::shared_ptr<Shader> shader, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);
	Object(const Object& other) {
		mesh = other.mesh;
		shader = other.shader;
		objectMatrix = other.objectMatrix;
	}
	void draw(const glm::mat4& viewMat, const glm::mat4& perspMat) const;
	void drawAsPoints(const glm::mat4& viewMat, const glm::mat4& perspMat) const;
	void drawAsPatch(const glm::mat4& viewMat, const glm::mat4& perspMat) const;
	void setTexture3D(unsigned int texture, int textureNumber, const std::string& name);
	void setTexture2D(unsigned int texture, int textureNumber, const std::string& name);
	void setShaderFloatParameter(const std::string& name, float val) const;
	void setVec3FParameter(const std::string& name,const glm::vec3& parameter) const;
	void setBoolParameter(const std::string& name,bool parameter) const;
	void setShaderIntParameter(const std::string& name,int parameter) const;
	void move(const glm::vec3& traslation);
	bool isHitByRay(glm::vec3 origin, glm::vec3 direction, glm::vec3& hit);
	void setShader(std::shared_ptr<Shader> newShader) { shader = newShader; };
	void setMat4FParameter(const std::string& name,const glm::mat4& mat);
};