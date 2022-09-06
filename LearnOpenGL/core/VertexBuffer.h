#pragma once

#include <glad/glad.h>

class VertexBuffer
{
public:
	VertexBuffer(const void* vertices, const int size);
	~VertexBuffer();

	void bind();
	void unbind();

private:
	unsigned int m_ID;
};