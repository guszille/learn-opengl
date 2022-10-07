#pragma once

#include <iostream>

#include <glad/glad.h>

class FrameBuffer
{
public:
	static enum class BufferType { TEXTURE, RENDER };

	FrameBuffer(int width, int height, int numberOfColorBuffers = 1, int colorInternalFormat = GL_RGBA, const BufferType& depthAndStencilBufferType = BufferType::RENDER, int samples = 1);
	~FrameBuffer();

	void bind();
	void unbind();

	void bindColorBuffer(int unit, int attachmentNumber = 0);

	unsigned int getID();

private:
	unsigned int m_ID, m_NumberOfColorBuffers, m_ColorBuffers[32], m_DepthAndStencilBuffer;
	BufferType m_DepthAndStencilBufferType;

	void attachTextureAsColorBuffer(int width, int height, int attachmentNumber, int internalFormat = GL_RGBA, int samples = 1);
	void attachTextureAsDepthAndStencilBuffer(int width, int height);
	void attachRenderBufferAsDepthAndStencilBuffer(int width, int height, int samples = 1);
};
