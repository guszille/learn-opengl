#include "VertexArray.h"

VertexArray::VertexArray() : m_ID()
{
	glGenVertexArrays(1, &m_ID);
	glBindVertexArray(m_ID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_ID);
}

void VertexArray::bind()
{
	glBindVertexArray(m_ID);
}

void VertexArray::unbind()
{
	glBindVertexArray(0);
}

void VertexArray::setVertexAttribute(unsigned int index, int size, int type, bool normalized, unsigned int stride, void* pointer)
{
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	glEnableVertexAttribArray(index);
}