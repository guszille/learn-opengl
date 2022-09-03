#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<MeshTexture>& textures)
	: m_Vertices(vertices), m_Indices(indices), m_Textures(textures)
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_Normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_TexCoords)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind the VAO before any other buffer.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Mesh::~Mesh()
{
	// TODO: Performs clean-up of all buffers allocated in GPU memory.
}

void Mesh::draw(ShaderProgram* shaderProgram)
{
	unsigned int texNumber[] = { 0, 0 }; // Buffer to carry the diffuse and specular positions.

	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		std::string uniformName = "";

		if (m_Textures[i].m_Type == "TEXTURE_DIFFUSE")
		{
			uniformName += "uMaterial.diffuseMaps[" + std::to_string(texNumber[0]++) + "]";
		}
		else if (m_Textures[i].m_Type == "TEXTURE_SPECULAR")
		{
			uniformName += "uMaterial.specularMaps[" + std::to_string(texNumber[1]++) + "]";
		}

		shaderProgram->setUniform1i(uniformName.c_str(), i);

		// Activating and binding texture.
		if (i >= 0 && i <= 15)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_Textures[i].m_ID);
		}
		else
		{
			std::cout << "[ERROR] MESH: Failed to bind texture in " << i << " unit." << std::endl;

			return;
		}
	}

	shaderProgram->bind();

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	
	shaderProgram->unbind();
}