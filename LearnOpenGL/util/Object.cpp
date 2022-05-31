#include "Object.h"

Object::Object(const ShaderProgram& shaderProgram, const VertexArray& vao, const VertexBuffer& vbo, const ElementBuffer& ebo, const glm::mat4& modelMatrix)
	: m_ShaderProgram(shaderProgram), m_VAO(vao), m_VBO(vbo), m_EBO(ebo), m_ModelMatrix(modelMatrix)
{
}

Object::~Object()
{
}

void Object::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	m_ShaderProgram.bind();
	m_VAO.bind();

	m_ShaderProgram.setUniformMatrix4fv("uModelMatrix", m_ModelMatrix);
	m_ShaderProgram.setUniformMatrix4fv("uViewMatrix", viewMatrix);
	m_ShaderProgram.setUniformMatrix4fv("uProjectionMatrix", projectionMatrix);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	m_VAO.unbind();
	m_ShaderProgram.unbind();
}