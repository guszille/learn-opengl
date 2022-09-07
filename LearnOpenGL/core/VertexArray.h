#pragma once

#include <glad/glad.h>

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void bind();
	void unbind();

	void setVertexAttribute(unsigned int index, int size, int type, bool normalized, unsigned int stride, void* pointer, int divisor = 0);
	static int retrieveMaxVertexAttributes();

private:
	unsigned int m_ID;
};