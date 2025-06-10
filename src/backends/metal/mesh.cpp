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
         0.0f, 1.0f,0.0f, 0.0f,0.0f,-1.0f, 0.0f,0.0f,
        -1.0f,-1.0f,0.0f, 0.0f,0.0f,-1.0f, 0.0f,0.0f,
         1.0f,-1.0f,0.0f, 0.0f,0.0f,-1.0f, 0.0f,0.0f,
    };
    int numVertices = sizeof(vertices) / (8*sizeof(float));

    MeshPtr meshPtr = createMesh(layerPtr,
        vertices, numVertices, nullptr, 0, nullptr);
    return Mesh(meshPtr);
}
Mesh ModernBoy::Metal::makeRectangle(NativePtr layerPtr){
    float vertices[] = {
        -0.5f,-0.5f,0.0f, 0.0f,0.0f,-1.0f, 0.0f,1.0f,
         0.5f,-0.5f,0.0f, 0.0f,0.0f,-1.0f, 1.0f,1.0f,
         0.5f, 0.5f,0.0f, 0.0f,0.0f,-1.0f, 1.0f,0.0f,
        -0.5f, 0.5f,0.0f, 0.0f,0.0f,-1.0f, 0.0f,0.0f,
    };
    int numVertices = sizeof(vertices) / (8*sizeof(float));
    uint32_t indices[] = {
        0, 1, 2,
        0, 2, 3
    };
    int numIndices = sizeof(indices)/sizeof(uint32_t);

    MeshPtr meshPtr = createMesh(layerPtr,
        vertices, numVertices, indices, numIndices,
        "metal_logo.png"
    );
    return Mesh(meshPtr);
}
Mesh ModernBoy::Metal::makeCube(NativePtr layerPtr){
    float vertices[] = {
        // front
        -0.5f,-0.5f,-0.5f, 0.0f,0.0f,0.0f, 0.0f,1.0f,
         0.5f,-0.5f,-0.5f, 0.0f,0.0f,0.0f, 1.0f,1.0f,
         0.5f, 0.5f,-0.5f, 0.0f,0.0f,0.0f, 1.0f,0.0f,
        -0.5f, 0.5f,-0.5f, 0.0f,0.0f,0.0f, 0.0f,0.0f,
        // back
        -0.5f,-0.5f, 0.5f, 0.0f,0.0f,0.0f, 0.0f,1.0f,
         0.5f,-0.5f, 0.5f, 0.0f,0.0f,0.0f, 1.0f,1.0f,
         0.5f, 0.5f, 0.5f, 0.0f,0.0f,0.0f, 1.0f,0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f,0.0f,0.0f, 0.0f,0.0f,
        // Left
        -0.5f, 0.5f,-0.5f, 0.0f,0.0f,0.0f, 0.0f,1.0f,
        -0.5f,-0.5f,-0.5f, 0.0f,0.0f,0.0f, 1.0f,1.0f,
        -0.5f,-0.5f, 0.5f, 0.0f,0.0f,0.0f, 1.0f,0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f,0.0f,0.0f, 0.0f,0.0f,
        // right
         0.5f, 0.5f,-0.5f, 0.0f,0.0f,0.0f, 0.0f, 1.0f,
         0.5f,-0.5f,-0.5f, 0.0f,0.0f,0.0f, 1.0f, 1.0f,
         0.5f,-0.5f, 0.5f, 0.0f,0.0f,0.0f, 1.0f, 0.0f,
         0.5f, 0.5f, 0.5f, 0.0f,0.0f,0.0f, 0.0f, 0.0f,
        // bottom
        -0.5f,-0.5f,-0.5f, 0.0f,0.0f,0.0f, 0.0f, 1.0f,
         0.5f,-0.5f,-0.5f, 0.0f,0.0f,0.0f, 1.0f, 1.0f,
         0.5f,-0.5f, 0.5f, 0.0f,0.0f,0.0f, 1.0f, 0.0f,
        -0.5f,-0.5f, 0.5f, 0.0f,0.0f,0.0f, 0.0f, 0.0f,
        // top
        -0.5f, 0.5f,-0.5f, 0.0f,0.0f,0.0f, 0.0f, 1.0f,
         0.5f, 0.5f,-0.5f, 0.0f,0.0f,0.0f, 1.0f, 1.0f,
         0.5f, 0.5f, 0.5f, 0.0f,0.0f,0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f,0.0f,0.0f, 0.0f, 0.0f
    };
    int numVertices = sizeof(vertices) / (8*sizeof(float));
    uint32_t indices[] = {
        // front
         0,  3,  2,
         2,  1,  0,
        // back
         4,  5,  6,
         6,  7,  4,
        // left
        11,  8,  9,
         9, 10, 11,
        // right
        12, 13, 14,
        14, 15, 12,
        // bottom
        16, 17, 18,
        18, 19, 16,
        // top
        20, 21, 22,
        22, 23, 20
    };
    int numIndices = sizeof(indices) / sizeof(uint32_t);

    MeshPtr meshPtr = createMesh(layerPtr,
        vertices, numVertices, indices, numIndices,
        "metal_logo.png"
    );
    return Mesh(meshPtr);
}

Mesh ModernBoy::Metal::makeSomething(NativePtr layerPtr){
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile("Hu tao.fbx",
        aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_CalcTangentSpace);

    if (!scene || !scene->mRootNode || scene->mNumMeshes == 0) {
        return Mesh(nullptr);
    }

    aiMesh* mesh = scene->mMeshes[0];

    std::vector<float> vertices(8*mesh->mNumVertices, 0);
    for (size_t i = 0; i < mesh->mNumVertices; ++i) {
        size_t base = 8*i;
        // position
        vertices[base+0] = mesh->mVertices[i].x;
        vertices[base+1] = mesh->mVertices[i].y-17;
        vertices[base+2] = mesh->mVertices[i].z+2;
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
    std::string manPath = "面.png";

    MeshPtr meshPtr = createMesh(layerPtr,
        vertices.data(), vertices.size() / 8,
        // nullptr, 0, texPath.C_Str()
        indices.data(), indices.size(), manPath.c_str()
    );
    return Mesh(meshPtr);
}