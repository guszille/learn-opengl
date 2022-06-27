#pragma once

#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ShaderProgram
{
public:
	ShaderProgram(const char* vertexShaderFilepath, const char* fragmentShaderFilepath);
	~ShaderProgram();

	void bind();
	void unbind();

	void setUniform1i(const char* uniformName, const int& data);
	void setUniform1f(const char* uniformName, const float& data);
	void setUniform3f(const char* uniformName, const glm::vec3& data);
	void setUniform4f(const char* uniformName, const glm::vec4& data);
	void setUniformMatrix4fv(const char* uniformName, const glm::mat4& data);

private:
	unsigned int m_ID;

	const std::string readShaderSource(const char* filepath);
	const unsigned int createShader(const char* shaderFilepath, int shaderType);
};