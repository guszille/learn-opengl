#include "TextRenderer.h"

TextRenderer::TextRenderer(const char* filepath)
	: m_VAO(), m_VBO()
{
	FT_Library freeTypeLib;
	FT_Face face;

	if (FT_Init_FreeType(&freeTypeLib))
	{
		std::cout << "[ERROR] TEXT RENDERER: Could not init FreeType library." << std::endl;
	}
	else
	{
		if (FT_New_Face(freeTypeLib, filepath, 0, &face))
		{
			std::cout << "[ERROR] TEXT RENDERER: Failed to load font." << std::endl;
		}
		else
		{
			FT_Set_Pixel_Sizes(face, 0, 48); // Set size to load glyphs as.

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction.

            // Load first 128 characters of ASCII set.
            for (unsigned char c = 0; c < 128; c++)
            {
                unsigned int texId;

                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                    std::cout << "[ERROR] TEXT RENDERER: Failed to load glyph." << std::endl;
                    continue;
                }

                glGenTextures(1, &texId);
                glBindTexture(GL_TEXTURE_2D, texId);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                Character character = { texId, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows), glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top), (unsigned int)(face->glyph->advance.x) };

                m_Characters.insert(std::pair<char, Character>(c, character));
            }

            glBindTexture(GL_TEXTURE_2D, 0);
		}
		
		FT_Done_Face(face);
	}

	FT_Done_FreeType(freeTypeLib);

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TextRenderer::~TextRenderer()
{
	// TODO: Delete all generated textures and buffers.
}

void TextRenderer::write(ShaderProgram& shaderProgram, std::string text, float x, float y, float scale, glm::vec3 color)
{
    bool blendIsEnabled = glIsEnabled(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shaderProgram.bind();

    shaderProgram.setUniform3f("uTextColor", color);

    glBindVertexArray(m_VAO);
    glActiveTexture(GL_TEXTURE0 + 15);

    std::string::const_iterator ci;

    for (ci = text.begin(); ci != text.end(); ci++)
    {
        Character ch = m_Characters[*ci];

        float xpos = x + ch.m_Bearing.x * scale;
        float ypos = y - (ch.m_Size.y - ch.m_Bearing.y) * scale;

        float w = ch.m_Size.x * scale;
        float h = ch.m_Size.y * scale;

        // Update VBO for each character.
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };

        // Render glyph texture over quad.
        glBindTexture(GL_TEXTURE_2D, ch.m_TexID);

        // Update content of VBO memory.
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData.
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Draw quad.
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels).
        x += (ch.m_Advance >> 6) * scale;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    shaderProgram.unbind();

    if (!blendIsEnabled)
    {
        glDisable(GL_BLEND);
    }
}