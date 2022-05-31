#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../core/ShaderProgram.h"
#include "../core/VertexArray.h"
#include "../core/VertexBuffer.h"
#include "../core/ElementBuffer.h"

class Object
{
public:
	Object(const ShaderProgram& shaderProgram, const VertexArray& vao, const VertexBuffer& vbo, const ElementBuffer& ebo, const glm::mat4& modelMatrix = glm::mat4(1.0f));
	~Object();

	void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

private:
	ShaderProgram m_ShaderProgram;
	VertexArray m_VAO;
	VertexBuffer m_VBO;
	ElementBuffer m_EBO;
	glm::mat4 m_ModelMatrix;
};