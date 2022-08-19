#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int width, int height, int colorAttachmentNumber, const BufferType& depthAndStencilBufferType)
	: m_ID(), m_ColorBuffer(), m_DepthAndStencilBuffer(), m_DepthAndStencilBufferType(depthAndStencilBufferType)
{
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	attachTextureAsColorBuffer(width, height, colorAttachmentNumber);

	if (depthAndStencilBufferType == BufferType::TEXTURE)
	{
		attachTextureAsDepthAndStencilBuffer(width, height);
	}
	else
	{
		attachRenderBufferAsDepthAndStencilBuffer(width, height);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "[ERROR] FRAMEBUFFER: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_ID);

	// Delete color and deptch/stencil buffers too.
	
	glDeleteTextures(1, &m_ColorBuffer);

	if (m_DepthAndStencilBufferType == BufferType::TEXTURE)
	{
		glDeleteTextures(1, &m_DepthAndStencilBuffer);
	}
	else
	{
		glDeleteRenderbuffers(1, &m_DepthAndStencilBuffer);
	}
}

void FrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
}

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bindColorBuffer(int unit)
{
	if (unit >= 0 && unit <= 15)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, m_ColorBuffer);
	}
	else
	{
		std::cout << "[ERROR] FRAMEBUFFER: Failed to bind texture in " << unit << " unit" << std::endl;
	}
}

void FrameBuffer::attachTextureAsColorBuffer(int width, int height, int attachmentNumber)
{
	glGenTextures(1, &m_ColorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_ColorBuffer);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentNumber, GL_TEXTURE_2D, m_ColorBuffer, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::attachTextureAsDepthAndStencilBuffer(int width, int height)
{
	glGenTextures(1, &m_DepthAndStencilBuffer);
	glBindTexture(GL_TEXTURE_2D, m_DepthAndStencilBuffer);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAndStencilBuffer, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::attachRenderBufferAsDepthAndStencilBuffer(int width, int height)
{
	// We need the depth and stencil values for testing, but don't need to sample these
	// values so a renderbuffer object suits this perfectly. When we're not sampling from
	// these buffers, a renderbuffer object is generally preferred.
	//
	glGenRenderbuffers(1, &m_DepthAndStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAndStencilBuffer);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAndStencilBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}