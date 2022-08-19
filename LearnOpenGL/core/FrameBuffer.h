#pragma once

#include <iostream>

#include <glad/glad.h>

class FrameBuffer
{
public:
	static enum class BufferType { TEXTURE, RENDER };

	FrameBuffer(int width, int height, int colorAttachmentNumber = 0, const BufferType& depthAndStencilBufferType = BufferType::RENDER);
	~FrameBuffer();

	void bind();
	void unbind();

	void bindColorBuffer(int unit);

private:
	unsigned int m_ID, m_ColorBuffer, m_DepthAndStencilBuffer;
	BufferType m_DepthAndStencilBufferType;

	void attachTextureAsColorBuffer(int width, int height, int attachmentNumber);
	void attachTextureAsDepthAndStencilBuffer(int width, int height);
	void attachRenderBufferAsDepthAndStencilBuffer(int width, int height);
};
