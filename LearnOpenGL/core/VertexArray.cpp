#include "VertexArray.h"

VertexArray::VertexArray() : m_ID()
{
	glGenVertexArrays(1, &m_ID);

	// There is no reason to bind the VAO now...
	// 
	// glBindVertexArray(m_ID);
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

void VertexArray::setVertexAttribute(unsigned int index, int size, int type, bool normalized, unsigned int stride, void* pointer, int divisor)
{
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	glEnableVertexAttribArray(index);

	// By default, the attribute divisor is 0 which tells OpenGL to update the content of the vertex
	// attribute each iteration of the vertex shader. By setting this attribute to 1 we're telling
	// OpenGL that we want to update the content of the vertex attribute when we start to render a
	// new instance. By setting it to 2 we'd update the content every 2 instances and so on.
	//
	glVertexAttribDivisor(index, divisor);
}

int VertexArray::retrieveMaxVertexAttributes()
{
	int numberOfAttributes;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numberOfAttributes);

	return numberOfAttributes;
}