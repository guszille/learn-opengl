#pragma once

#include <vector>
#include <iostream>

#include <glad/glad.h>

struct ColorBufferConfig
{
	int m_InternalFormat = GL_RGBA;
	int m_Filter = GL_LINEAR;
	int m_ClampMode = GL_CLAMP_TO_EDGE;
};

class FrameBuffer
{
public:
	static enum class BufferType { NONE, TEXTURE, RENDER };

	FrameBuffer(int width, int height, int numberOfColorBuffers = 1, int colorInternalFormat = GL_RGBA, int filter = GL_LINEAR, int clampMode = GL_CLAMP_TO_EDGE, const BufferType& depthAndStencilBufferType = BufferType::RENDER, int samples = 1);
	FrameBuffer(int width, int height, std::vector<ColorBufferConfig> configurations, const BufferType& depthAndStencilBufferType = BufferType::RENDER, int samples = 1);
	~FrameBuffer();

	void bind();
	void unbind();

	void bindColorBuffer(int unit, int attachmentNumber = 0);

	unsigned int getID();

private:
	unsigned int m_ID, m_NumberOfColorBuffers, m_ColorBuffers[32], m_DepthAndStencilBuffer;
	BufferType m_DepthAndStencilBufferType;

	void attachTextureAsColorBuffer(int width, int height, int attachmentNumber, int internalFormat = GL_RGBA, int filter = GL_LINEAR, int clampMode = GL_CLAMP_TO_EDGE, int samples = 1);
	void attachTextureAsDepthAndStencilBuffer(int width, int height);
	void attachRenderBufferAsDepthAndStencilBuffer(int width, int height, int samples = 1);
};
