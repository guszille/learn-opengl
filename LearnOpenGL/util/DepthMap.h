#pragma once

#include <iostream>

#include <glad/glad.h>

class DepthMap
{
public:
	static enum class BufferType { TEXTURE_2D, TEXTURE_CUBE_MAP };

	DepthMap(int width, int height, const BufferType& type = BufferType::TEXTURE_2D);
	~DepthMap();

	void bind();
	void unbind();

	void bindDepthBuffer(int unit);

private:
	unsigned int m_ID, m_DepthBuffer;
	BufferType m_DepthBufferType;
};