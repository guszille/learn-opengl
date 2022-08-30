#pragma once

#include <glad/glad.h>

class UniformBuffer
{
public:
	UniformBuffer(const int size);
	~UniformBuffer();

	void bind();
	void unbind();

	void link(const int bindingPoint);
	void link(const int bindingPoint, const int offset, const int size);

	void update(const int offset, const int size, const void* data);

private:
	unsigned int m_ID;
};
