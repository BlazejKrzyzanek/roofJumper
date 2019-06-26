#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <string>

#include "Mesh.h"
#include "Shader.h"

unsigned int textureFromFile(const char* path, const std::string& directory, bool gamma = false);

class Model
{
public:
	static std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	glm::vec3 collider;
	Model(const char* path, glm::vec3 collider);
	void draw(Shader shader);
private:
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);
};