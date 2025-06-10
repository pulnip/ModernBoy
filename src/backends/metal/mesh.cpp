#include <string>
#include <utility>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "backends/metal/mesh.hpp"

using namespace ModernBoy::Metal;

Mesh::Mesh(Mesh&& other){ moveFrom(std::move(other)); }
Mesh& Mesh::operator=(Mesh&& other){
    moveFrom(std::move(other));
    return *this;
}
void Mesh::moveFrom(Mesh&& other){
    meshPtr = other.meshPtr;
    other.meshPtr = nullptr;
}

Mesh ModernBoy::Metal::makeTriangle(NativePtr layerPtr){
    float vertices[] = {
         0, 1,0, 1,0,0, 0,0,
        -1,-1,0, 0,1,0, 0,0,
         1,-1,0, 0,0,1, 0,0,
    };
    int numVertices = sizeof(vertices) / (8*sizeof(float));
    // uint32_t indices[3] = { 0, 1, 2 };
    // int numIndices = sizeof(indices)/sizeof(uint32_t);

    MeshPtr meshPtr = createMesh(layerPtr,
        vertices, numVertices, nullptr, 0, nullptr);
    return Mesh(meshPtr);
}

Mesh ModernBoy::Metal::makeSomething(NativePtr layerPtr){
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile("assets/furina/source/Furina2.fbx",
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || !scene->mRootNode || scene->mNumMeshes == 0) {
        return Mesh(nullptr);
    }

    aiMesh* mesh = scene->mMeshes[0];

    std::vector<float> vertices(8*mesh->mNumVertices, 0);
    for (size_t i = 0; i < mesh->mNumVertices; ++i) {
        size_t base = 8*i;
        // position
        vertices[base+0] = mesh->mVertices[i].x;
        vertices[base+1] = mesh->mVertices[i].y;
        vertices[base+2] = mesh->mVertices[i].z;
        // normal
        if (mesh->HasNormals()) {
            vertices[base+3] = mesh->mNormals[i].x;
            vertices[base+4] = mesh->mNormals[i].y;
            vertices[base+5] = mesh->mNormals[i].z;
        }
        // uv
        if (mesh->HasTextureCoords(0)) {
            vertices[base+6] = mesh->mTextureCoords[0][i].x;
            vertices[base+7] = mesh->mTextureCoords[0][i].y;
        }
    }

    std::vector<uint32_t> indices(3*mesh->mNumFaces, 0);
    for (size_t i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];

        if (face.mNumIndices == 3) {
            indices[3*i+0] = face.mIndices[0];
            indices[3*i+1] = face.mIndices[1];
            indices[3*i+2] = face.mIndices[2];
        }
    }

    unsigned int materialIndex = mesh->mMaterialIndex;
    aiMaterial* material = scene->mMaterials[materialIndex];

    aiString texPath;
    if(material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS) {
        printf("Diffuse texture: %s\n", texPath.C_Str());
    }
    std::string manPath = "/Users/choiw/repository/ModernBoy/assets/furina/textures/颜.png";

    MeshPtr meshPtr = createMesh(layerPtr,
        vertices.data(), vertices.size() / 8,
        nullptr, 0, manPath.c_str()
        // indices.data(), indices.size(), manPath.c_str()
    );
    return Mesh(meshPtr);
}