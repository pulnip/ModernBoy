#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

int main1(int argc, char* argv[]){
    if(argc < 2){
        std::cout << "no file selected" << std::endl;
        return 0;
    }

    Assimp::Importer importer;

    const aiScene* scene=importer.ReadFile(
        argv[1],
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate |
        aiProcess_SortByPType
    );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
    }

    const auto& material = scene->mMaterials[0];

    aiString path;
    if(!material->GetTexture(AI_TEXTURE_TYPE_MAX, 0, &path))
        return 0;

    std::string matPath = path.C_Str();
    std::cout << "Texture: " << matPath << std::endl;

    return 0;
}