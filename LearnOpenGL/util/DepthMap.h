#pragma once

#include <iostream>

#include <glad/glad.h>

class DepthMap
{
public:
	DepthMap(int width, int height);
	~DepthMap();

	void bind();
	void unbind();

	void bindDepthBuffer(int unit);

private:
	unsigned int m_ID, m_DepthBuffer;
};