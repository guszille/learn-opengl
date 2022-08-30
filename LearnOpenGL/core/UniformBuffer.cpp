#include "UniformBuffer.h"

UniformBuffer::UniformBuffer(const int size) : m_ID()
{
	glGenBuffers(1, &m_ID);
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &m_ID);
}

void UniformBuffer::bind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
}

void UniformBuffer::unbind()
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::link(const int bindingPoint)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_ID);
}

void UniformBuffer::link(const int bindingPoint, const int offset, const int size)
{
	glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, m_ID, offset, size);
}

void UniformBuffer::update(const int offset, const int size, const void* data)
{
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}