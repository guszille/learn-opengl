#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "../../core/ShaderProgram.h"

struct Vertex
{
    glm::vec3 m_Position;
    glm::vec3 m_Normal;
    glm::vec2 m_TexCoords;
};

struct MeshTexture
{
    unsigned int m_ID;
    std::string m_Type;
    std::string m_RelativeFilepath;

    MeshTexture(const int id, const std::string& type, const std::string& filepath)
        : m_ID(id), m_Type(type), m_RelativeFilepath(filepath) {}
    ~MeshTexture() { /* TODO: Performs clean-up of all buffers allocated in GPU memory. */ }
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<MeshTexture>& textures);
    ~Mesh();

    void draw(ShaderProgram* shaderProgram);

    unsigned int getVAO() const;

    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;
    std::vector<MeshTexture> m_Textures;

private:
    unsigned int m_VAO, m_VBO, m_EBO;
};