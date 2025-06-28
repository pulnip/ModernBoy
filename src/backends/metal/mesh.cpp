#include <string>
#include <utility>
#include <vector>
#include "raw_resource.hpp"
#include "backends/metal/mesh.hpp"

#ifdef __cplusplus
extern "C"{
#endif

    extern void* createMesh(const void* layerPtr,
        const float* vertices, int numVertices,
        const uint32_t* indices, int numIndices,
        const char* texturePath);
    extern void destroyMesh(const void* meshPtr);
    extern void* makeTriangle(const void* layerPtr);

#ifdef __cplusplus
}
#endif

using namespace ModernBoy;
using namespace ModernBoy::Metal;

static std::vector<float> toFloats(const Vertices& vertices);

Mesh::Mesh(Mesh&& other){ moveFrom(std::move(other)); }
Mesh& Mesh::operator=(Mesh&& other){
    moveFrom(std::move(other));
    return *this;
}
void Mesh::moveFrom(Mesh&& other){
    meshPtr = other.meshPtr;
    other.meshPtr.clear();
}

Mesh::Mesh(const std::string& fileName,
    NativePtr metalLayer){
    auto rawMesh = import<RawMesh>(fileName);
    meshPtr.reserve(rawMesh.size());

    for(const auto& part: rawMesh){
        auto vertices = toFloats(part.vertices);
        const char* texPath = nullptr;
        if(!part.textures.empty()){
            texPath = part.textures[0].c_str();
        }

        meshPtr.emplace_back(createMesh(
            metalLayer,
            vertices.data(), vertices.size() / 8,
            part.indices.data(), part.indices.size(),
                texPath
        ));
    }
}
Mesh::~Mesh(){
    for(const auto& part: meshPtr)
        destroyMesh(part);
}

static std::vector<float> toFloats(const Vertices& vertices){
    std::vector<float> result(8*vertices.size());

    for(size_t i=0; i<vertices.size(); ++i){
        size_t base = 8*i;
        result[base+0] = vertices[i].position[0];
        result[base+1] = vertices[i].position[1];
        result[base+2] = vertices[i].position[2];
        result[base+3] = vertices[i].normal[0];
        result[base+4] = vertices[i].normal[1];
        result[base+5] = vertices[i].normal[2];
        result[base+6] = vertices[i].texcoord[0];
        result[base+7] = vertices[i].texcoord[1];
    }

    return result;
}
