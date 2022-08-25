#pragma once

#include <array>
#include <iostream>

#include <glad/glad.h>

#if !defined _STB_IMAGE_INCLUDED
#define _STB_IMAGE_INCLUDED

#include <stb/stb_image.h>
#endif // _STB_IMAGE_INCLUDED

class CubeMap
{
public:
	CubeMap(const char* filepath, const std::array<const char*, 6>& faces);
	~CubeMap();

	void bind(int unit);
	void unbind();

private:
	unsigned int m_ID;
};