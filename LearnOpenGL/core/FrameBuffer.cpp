#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(int width, int height, int colorAttachmentNumber, const BufferType& depthAndStencilBufferType, int samples)
	: m_ID(), m_ColorBuffer(), m_DepthAndStencilBuffer(), m_DepthAndStencilBufferType(depthAndStencilBufferType)
{
	glGenFramebuffers(1, &m_ID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

	// Note:
	//
	// A multisampled image contains much more information than a normal image
	// so what we need to do is downscale or resolve the image. Resolving a multisampled
	// framebuffer is generally done through glBlitFramebuffer that copies a region
	// from one framebuffer to the other while also resolving any multisampled buffers.

	// Creating color buffer.
	attachTextureAsColorBuffer(width, height, colorAttachmentNumber, samples);

	// Creating depth/stencil buffer.
	if (depthAndStencilBufferType == BufferType::TEXTURE)
	{
		if (samples != 1)
		{
			std::cout << "[ERROR] FRAMEBUFFER: Buffer type doesn't support the number of samples!" << std::endl;
		}

		attachTextureAsDepthAndStencilBuffer(width, height);
	}
	else
	{
		attachRenderBufferAsDepthAndStencilBuffer(width, height, samples);
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

void FrameBuffer::attachTextureAsColorBuffer(int width, int height, int attachmentNumber, int samples)
{
	glGenTextures(1, &m_ColorBuffer);

	if (samples == 1)
	{
		glBindTexture(GL_TEXTURE_2D, m_ColorBuffer);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentNumber, GL_TEXTURE_2D, m_ColorBuffer, 0);

		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_ColorBuffer);

		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);

		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentNumber, GL_TEXTURE_2D_MULTISAMPLE, m_ColorBuffer, 0);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}
}

void FrameBuffer::attachTextureAsDepthAndStencilBuffer(int width, int height)
{
	glGenTextures(1, &m_DepthAndStencilBuffer);
	glBindTexture(GL_TEXTURE_2D, m_DepthAndStencilBuffer);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAndStencilBuffer, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void FrameBuffer::attachRenderBufferAsDepthAndStencilBuffer(int width, int height, int samples)
{
	// We need the depth and stencil values for testing, but don't need to sample these
	// values so a renderbuffer object suits this perfectly. When we're not sampling from
	// these buffers, a renderbuffer object is generally preferred.
	//
	glGenRenderbuffers(1, &m_DepthAndStencilBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAndStencilBuffer);

	if (samples == 1)
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	}
	else
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAndStencilBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}