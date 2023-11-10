#pragma once

#include <iostream>

#include <glad/glad.h>

#if !defined _STB_IMAGE_INCLUDED
#define _STB_IMAGE_INCLUDED

#include <stb/stb_image.h>
#endif // _STB_IMAGE_INCLUDED

class Texture
{
public:
	Texture(const char* filepath, const bool gammaCorrection = false);
	Texture(int width, int height, int internalFormat, int format, int type, const float* data);
	~Texture();

	void bind(int unit);
	void unbind();

private:
	unsigned int m_ID;
	int m_Width, m_Height, m_ColorChannels;
};