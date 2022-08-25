#include "CubeMap.h"

CubeMap::CubeMap(const char* filepath, const std::array<const char*, 6>& faces)
	: m_ID()
{
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

	stbi_set_flip_vertically_on_load(false);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		std::string buff = "";

		buff.append(filepath);
		buff.append("/");
		buff.append(faces[i]);

		int width, height, colorChannels;
		unsigned char* data = stbi_load(buff.c_str(), &width, &height, &colorChannels, 0);

		// Texture Target					Orientation
		// 
		// GL_TEXTURE_CUBE_MAP_POSITIVE_X	Right
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_X	Left
		// GL_TEXTURE_CUBE_MAP_POSITIVE_Y	Top
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	Bottom
		// GL_TEXTURE_CUBE_MAP_POSITIVE_Z	Back
		// GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	Front
		//
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "[ERROR] CUBEMAP: Failed to load texture in \"" << filepath << "\"." << std::endl;
		}

		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

CubeMap::~CubeMap()
{
	glDeleteTextures(1, &m_ID);
}

void CubeMap::bind(int unit)
{
	if (unit >= 0 && unit <= 15)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
	}
	else
	{
		std::cout << "[ERROR] CUBEMAP: Failed to bind texture in " << unit << " unit" << std::endl;
	}
}

void CubeMap::unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}