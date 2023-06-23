#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>

#include "Shader.h"
#include "Mesh.h"

struct Material {
    glm::vec3 Diffuse;
    glm::vec3 Specular;
    glm::vec3 Ambient;
    float Shininess;
};

class Model
{
public:
    Model(std::string path);
    void draw(Shader& shader);
private:
    // model data
    std::vector<Mesh> m_meshes;
    std::string m_directory;
    std::vector<Texture> m_texturesLoaded; // Textures that are already loaded (we use this to avoid to reload the same texture more than once)

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};
