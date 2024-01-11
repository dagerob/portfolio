#include "Object.h"

Object::Object(Mesh* mesh, std::shared_ptr<Shader> shader, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) : mesh(mesh), shader(shader) {
	this->mesh = mesh;
	this->shader = shader;
	objectMatrix = glm::mat4(1.0f);
	objectMatrix = glm::translate(objectMatrix, position);
	glm::quat myquaternion = glm::quat(rotation);
	objectMatrix = objectMatrix * glm::toMat4(myquaternion);
	objectMatrix = glm::scale(objectMatrix, scale);
}

void Object::draw(const glm::mat4& viewMat, const glm::mat4& perspMat) const {
	shader->use();

	// we pass the matrixes
	shader->setMat4F("projection", perspMat);
	shader->setMat4F("view", viewMat);
	shader->setMat4F("model", objectMatrix);

	// we draw the mesh
	mesh->draw();
}

void Object::drawAsPoints(const glm::mat4& viewMat, const glm::mat4& perspMat) const {
	shader->use();

	// we pass the matrixes
	shader->setMat4F("projection", perspMat);
	shader->setMat4F("view", viewMat);
	shader->setMat4F("model", objectMatrix);

	// we draw the mesh
	mesh->drawAsPoints();
}

void Object::drawAsPatch(const glm::mat4& viewMat, const glm::mat4& perspMat) const {
	shader->use();

	// we pass the matrixes
	shader->setMat4F("projection", perspMat);
	shader->setMat4F("view", viewMat);
	shader->setMat4F("model", objectMatrix);

	// we draw the mesh
	mesh->drawAsPatch();
}

void Object::setTexture3D(unsigned int texture, int textureNumber, const std::string& name) {
	shader->use();
	shader->setInt(name, textureNumber);
	switch (textureNumber)
	{
	case 0:
		glActiveTexture(GL_TEXTURE0);
		break;
	case 1:
		glActiveTexture(GL_TEXTURE1);
		break;
	case 2:
		glActiveTexture(GL_TEXTURE2);
		break;
	case 3:
		glActiveTexture(GL_TEXTURE3);
		break;
	case 4:
		glActiveTexture(GL_TEXTURE4);
		break;
	case 5:
		glActiveTexture(GL_TEXTURE5);
		break;
	default:
		return;
	}
	glBindTexture(GL_TEXTURE_3D, texture);
}

void Object::setTexture2D(unsigned int texture, int textureNumber, const std::string& name) {
	shader->use();
	shader->setInt(name, textureNumber);
	switch (textureNumber)
	{
	case 0:
		glActiveTexture(GL_TEXTURE0);
		break;
	case 1:
		glActiveTexture(GL_TEXTURE1);
		break;
	case 2:
		glActiveTexture(GL_TEXTURE2);
		break;
	case 3:
		glActiveTexture(GL_TEXTURE3);
		break;
	case 4:
		glActiveTexture(GL_TEXTURE4);
		break;
	case 5:
		glActiveTexture(GL_TEXTURE5);
		break;
	default:
		return;
	}
	glBindTexture(GL_TEXTURE_2D, texture);
}

void Object::setShaderFloatParameter(const std::string& name, float val) const{
	shader->use();
	shader->setFloat(name, val);
}

void Object::setVec3FParameter(const std::string& name, const glm::vec3& parameter) const {
	shader->use();
	shader->setVec3F(name, parameter);
}

void Object::setBoolParameter(const std::string& name, bool parameter) const {
	shader->use();
	shader->setBool(name, parameter);
}

void Object::move(const glm::vec3& traslation) {
	objectMatrix = glm::translate(objectMatrix, traslation);
}

void Object::setShaderIntParameter(const std::string& name, int parameter) const {
	shader->use();
	shader->setInt(name, parameter);
}

bool Object::isHitByRay(glm::vec3 origin, glm::vec3 direction, glm::vec3& hit) {
	return mesh->getIntersection(origin, direction, objectMatrix, hit);
}

void Object::setMat4FParameter(const std::string& name,const glm::mat4& mat) {
	shader->use();
	shader->setMat4F(name, mat);
}