// Code obtained from https://learnopengl.com/Getting-started/Creating-a-window tutorial series
// modified by me
#ifndef SHADER_H
#define SHADER_H
#include "commom_header.h"

class Shader
{
public:
	// the program ID
	UINT ID;

	// constructor reads and builds the shader
	Shader() : ID(0) {};
	Shader& operator=(const Shader& other);
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* tessControlPath, const char* tessEvaluationPath, int vertsPerPatch);
	Shader(const char* vertexPath, const char* geometryPAth, const char** attributes, int attributesNumber);
	// use/activate the shader
	void use() const;
	// utility uniform functions
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setVec4F(const std::string &name, glm::vec4 vector) const;
	void setVec3F(const std::string &name, glm::vec3 vector) const;
	void setMat4F(const std::string &name, glm::mat4 matrix) const;
};

#endif