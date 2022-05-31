#pragma once

#include <iostream>

#include <glad/glad.h>

#include <stb/stb_image.h>

class Texture
{
public:
	Texture(const char* filepath);
	~Texture();

	void bind(int unit);
	void unbind();

private:
	unsigned int m_ID;
	int m_Width, m_Height, m_ColorChannels;
};