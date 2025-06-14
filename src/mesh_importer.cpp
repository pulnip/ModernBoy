#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh_importer.hpp"

using namespace ModernBoy;

static RawMesh importMesh(const aiMesh* mesh, const aiScene* scene);
RawMeshes fromFbx(const std::string& fileName);
// RawMesh for shader test
static RawMeshes createTriangle();
static RawMeshes createRectangle();
static RawMeshes createCube();

RawMeshes MeshImporter::import(const std::string& fileName){
    if(fileName.ends_with(".fbx"))
        return fromFbx(fileName);
    else if(fileName.compare("Triangle") == 0)
        return createTriangle();
    else if(fileName.compare("Rectangle") == 0)
        return createRectangle();
    else if(fileName.compare("Cube") == 0)
        return createCube();
    return {};
}

static RawMeshes createTriangle(){
    Vertices vertices = {
        {
            {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {0.5f, 0.0f}, {0.0f, -1.0f, 0.0f}
        },{
            {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}
        },{
            {1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
        }
    };
    Indices indices = {0, 2, 1};
    
    return { RawMesh(vertices, indices, {}) };

}
static RawMeshes createRectangle(){
    Vertices vertices = {
        {
            {-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f}, 
            {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
        },{
            {0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}
        },{
            {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}
        },{
            {-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, -1.0f},
            {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}
        }
    };
    Indices indices = {
        0, 1, 2,
        0, 2, 3
    };

    return { RawMesh(vertices, indices, {}) };
}
static RawMeshes createCube(){
    Vertices vertices = {
        // front
        {
            {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},
            {0.0f, 1.0f}, {},
        },{
            { 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},
            {1.0f, 1.0f}, {}
        },{
            { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},
            {1.0f, 0.0f}, {}
        },{
            {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f},
            {0.0f, 0.0f}, {}
        },
        // back
        {
            {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},
            {0.0f, 1.0f}, {}
        },{
            { 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},
            {1.0f, 1.0f}, {}
        },{
            { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f}, {}
        },{
            {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f}, {}
        },
        // left
        {
            {-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f}, {}
        },{
            {-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f}, {}
        },{
            {-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f}, {}
        },{
            {-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f}, {}
        },
        // right
        {
            { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f}, {}
        },{
            { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f}, {}
        },{
            { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},
            {1.0f, 0.0f}, {}
        },{
            { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f},
            {0.0f, 0.0f}, {}
        },
        // bottom
        {
            {-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},
            {0.0f, 1.0f}, {}
        },{
            { 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f},
            {1.0f, 1.0f}, {}
        },{
            { 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},
            {1.0f, 0.0f}, {}
        },{
            {-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f},
            {0.0f, 0.0f}, {}
        },
        // top
        {
            {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},
            {0.0f, 1.0f}, {}
        },{
            { 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f},
            {1.0f, 1.0f}, {}
        },{
            { 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},
            {1.0f, 0.0f}, {}
        },{
            {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f}, {}
        }
    };
    Indices indices = {
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

    return { RawMesh(vertices, indices, {}) };
}

RawMeshes fromFbx(const std::string& fileName){
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fileName.c_str(),
        aiProcess_Triangulate | aiProcess_CalcTangentSpace
        // Metal, DirectX
        | aiProcess_ConvertToLeftHanded
    );
    if(!scene || !scene->mRootNode || scene->mNumMeshes==0){
        return {};
    }

    size_t numMeshes = scene->mNumMeshes;
    RawMeshes meshes(numMeshes);
    for(size_t i=0; i<numMeshes; ++i){
        meshes[i] = importMesh(scene->mMeshes[i], scene);
    }

    return meshes;
}

static RawMesh importMesh(const aiMesh* mesh,
    const aiScene* scene
){
    RawMesh result(
        // Only Triangle
        mesh->mNumVertices, mesh->mNumFaces * 3,
        // one material per mesh (Assimp guarantee)
        1
    );

    auto& vertices = result.vertices;
    auto& indices = result.indices;
    for(size_t i=0; i<mesh->mNumVertices; ++i){
        const auto& aiVertex = mesh->mVertices[i];

        vertices[i] = RawVertex{
            {aiVertex.x, aiVertex.y ,aiVertex.z},
            {0, 0, 0}, {0, 0}, {0, 0, 0}
        };
        if(mesh->HasNormals()){
            const auto& aiNormal = mesh->mNormals[i];
            auto& normal = vertices[i].normal;

            normal[0] = aiNormal.x;
            normal[1] = aiNormal.y;
            normal[2] = aiNormal.z;
        }
        if(mesh->HasTextureCoords(0)){
            const auto& aiUV = mesh->mTextureCoords[0][i];
            auto& uv = vertices[i].texcoord;

            uv[0] = aiUV.x;
            uv[1] = aiUV.y;
        }
        if(mesh->HasTangentsAndBitangents()){
            const auto& aiTangent = mesh->mTangents[i];
            auto& tangent = vertices[i].tangent;

            tangent[0] = aiTangent.x;
            tangent[1] = aiTangent.y;
            tangent[2] = aiTangent.z;
        }
    }

    for(size_t i=0; i<mesh->mNumFaces; ++i){
        const auto& aiFace_ = mesh->mFaces[i];
        const auto base = 3*i;

        indices[base+0] = aiFace_.mIndices[0];
        indices[base+1] = aiFace_.mIndices[1];
        indices[base+2] = aiFace_.mIndices[2];
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiString texPath;
    if(material->GetTexture(
        aiTextureType_DIFFUSE, 0, &texPath
    )){
        printf("Diffuse Texture: %s\n", texPath.C_Str());
    }
    result.textures[0] = std::string(texPath.C_Str());

    return result;
}
