#pragma once

#include <glad/glad.h>

class ElementBuffer
{
public:
	ElementBuffer(const unsigned int* indices, const int size);
	~ElementBuffer();

	void bind();
	void unbind();

private:
	unsigned int m_ID;
};