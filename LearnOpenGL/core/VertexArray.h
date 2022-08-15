#pragma once

#include <glad/glad.h>

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void bind();
	void unbind();

	void setVertexAttribute(unsigned int index, int size, int type, bool normalized, unsigned int stride, void* pointer);
	static int retrieveMaxVertexAttributes();

private:
	unsigned int m_ID;
};