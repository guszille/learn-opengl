#pragma once

#include <string>
#include <vector>

#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#if !defined _STB_IMAGE_INCLUDED
#define _STB_IMAGE_INCLUDED

#include <stb/stb_image.h>
#endif // _STB_IMAGE_INCLUDED

#include "Mesh.h"

#include "../../core/ShaderProgram.h"

class Model
{
public:
	Model(const char* filepath);
	~Model();

	void draw(ShaderProgram* shaderProgram);

	const std::vector<Mesh>& getMeshes();
	const std::vector<MeshTexture>& getLoadedTextures();

private:
	std::vector<Mesh> m_Meshes;
	std::vector<MeshTexture> m_LoadedTextures;
	std::string m_Directory;

	void loadModel(const std::string& filepath);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	unsigned int loadTexture(const char* filepath);
	std::vector<MeshTexture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
};