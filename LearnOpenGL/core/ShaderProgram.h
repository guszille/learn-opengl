#pragma once

#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

class ShaderProgram
{
public:
	ShaderProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename);
	~ShaderProgram();

	void bind();
	void unbind();

private:
	unsigned int m_ID;

	const std::string readShaderSource(const char* filename);
	const unsigned int createShader(const char* shaderFilename, int shaderType);
};