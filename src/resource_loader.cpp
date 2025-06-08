#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "resource_loader.hpp"

using namespace ModernBoy;

static RawMesh loadMesh(aiMesh* mesh);

RawMeshes ModernBoy::loadMeshes(const std::string& fileName){
    Assimp::Importer importer;

    const aiScene* scene=importer.ReadFile(
        fileName,
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate |
        aiProcess_SortByPType
    );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        return {};
    }

    RawMeshes meshes(scene->mNumMeshes);
    for(size_t i=0; i<scene->mNumMeshes; ++i){
        meshes[i] = loadMesh(scene->mMeshes[i]);
    }

    return meshes;
}

static RawMesh loadMesh(aiMesh* mesh){
    RawMesh rawMesh(mesh->mNumVertices, 3*mesh->mNumFaces);
    aiVector3D zeros{0, 0, 0};

    for(size_t i = 0; i < mesh->mNumVertices; ++i){
        const auto& vertex = mesh->mVertices[i];
        const auto& normal = mesh->mNormals[i];
        const auto& texcoord = mesh->HasTextureCoords(0) ?
            mesh->mTextureCoords[0][i] : zeros;
        const auto& aiTangent = mesh->HasTangentsAndBitangents() ?
            mesh->mTangents[i] : zeros;

        RawVertex rawVertex{
            .position = {vertex.x, vertex.y, vertex.z},
            .normal = {normal.x, normal.y, normal.z},
            .texcoord = {texcoord.x, texcoord.y},
            .tangent = {aiTangent.x, aiTangent.y, aiTangent.z}
        };
        rawMesh.vertices.push_back(rawVertex);
    }

    for(size_t i = 0; i < mesh->mNumFaces; ++i){
        const auto& face = mesh->mFaces[i];

        assert(face.mNumIndices == 3);

        rawMesh.indices.push_back(face.mIndices[0]);
        rawMesh.indices.push_back(face.mIndices[1]);
        rawMesh.indices.push_back(face.mIndices[2]);
    }

    return rawMesh;
}

RawTexture ModernBoy::loadTexture(const std::string& fileName){
    int width, height, channels;
    auto data = stbi_load(fileName.c_str(), &width, &height, &channels, 0);

    RawTexture texture{
        .width=width,
        .height=height,
        .channels=channels,
        .pixels=std::vector<uint8_t>(data, data+width*height*channels)
    };

    stbi_image_free(data);
    return texture;
}
