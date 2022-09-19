#include "Model.h"

Model::Model(const char* filepath)
{
	loadModel(filepath);
}

Model::~Model()
{
}

void Model::draw(ShaderProgram* shaderProgram)
{
	for (Mesh& mesh : m_Meshes)
	{
		mesh.draw(shaderProgram);
	}
}

const std::vector<Mesh>& Model::getMeshes()
{
	return m_Meshes;
}

const std::vector<MeshTexture>& Model::getLoadedTextures()
{
	return m_LoadedTextures;
}

void Model::loadModel(const std::string& filepath)
{
	Assimp::Importer importer;

	// More post-processing options:
	// 
	//	aiProcess_GenNormals: creates normal vectors for each vertex if the model doesn't contain normal vectors.
	//	aiProcess_SplitLargeMeshes: splits large meshes into smaller sub-meshes (which is useful if your rendering has a maximum number of vertices allowed and can only process smaller meshes).
	//	aiProcess_OptimizeMeshes: does the reverse by trying to join several meshes into one larger mesh (reducing drawing calls for optimization).
	// 
	// http://assimp.sourceforge.net/lib_html/postprocess_8h.html
	//
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "[ERROR] MODEL: ASSIMP" << "\n" << importer.GetErrorString() << std::endl;

		return;
	}

	m_Directory = filepath.substr(0, filepath.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any).
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		m_Meshes.push_back(processMesh(mesh, scene));
	}

	// Then do the same for each of its children.
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<MeshTexture> textures;

	// Process vertex positions, normals and texture coordinates.
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.m_Position.x = mesh->mVertices[i].x;
		vertex.m_Position.y = mesh->mVertices[i].y;
		vertex.m_Position.z = mesh->mVertices[i].z;

		vertex.m_Normal.x = mesh->mNormals[i].x;
		vertex.m_Normal.y = mesh->mNormals[i].y;
		vertex.m_Normal.z = mesh->mNormals[i].z;

		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			vertex.m_TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.m_TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.m_TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertex.m_Tangent.x = mesh->mTangents[i].x;
		vertex.m_Tangent.y = mesh->mTangents[i].y;
		vertex.m_Tangent.z = mesh->mTangents[i].z;

		vertices.push_back(vertex);
	}

	// Process indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process material.
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<MeshTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "TEXTURE_DIFFUSE");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<MeshTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "TEXTURE_SPECULAR");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<MeshTexture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "TEXTURE_NORMAL");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

unsigned int Model::loadTexture(const char* filepath)
{
	stbi_set_flip_vertically_on_load(true);

	unsigned int textureID;
	int width, height, colorChannels, format = GL_RED; // Default format.
	unsigned char* data = stbi_load(filepath, &width, &height, &colorChannels, 0);

	// WARNING: We are only expecting an image with 3 or 4 color channels.
	//			Any other format may generate some OpenGL error.

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	switch (colorChannels)
	{
	case 3:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		format = GL_RGB;

		break;

	case 4:
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		format = GL_RGBA;

		break;

	default:
		std::cout << "[ERROR] MODEL: Texture format not supported." << std::endl;

		break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "[ERROR] MODEL: Failed to load texture in \"" << filepath << "\"." << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);

	return textureID;
}

std::vector<MeshTexture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<MeshTexture> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString buffer;
		material->GetTexture(type, i, &buffer);

		bool skip = false;
		std::string relativeFilepath = buffer.C_Str();

		for (unsigned int j = 0; j < m_LoadedTextures.size(); j++)
		{
			if (relativeFilepath == m_LoadedTextures[j].m_RelativeFilepath)
			{
				textures.push_back(m_LoadedTextures[j]);
				skip = true;

				break;
			}
		}

		if (!skip)
		{
			std::string completeFilepath = m_Directory + "/" + relativeFilepath;
			unsigned int textureID = loadTexture(completeFilepath.c_str());

			textures.push_back(MeshTexture (textureID, typeName, relativeFilepath));
		}
	}

	return textures;
}