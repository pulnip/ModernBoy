#include <string>
#include <utility>
#include <vector>
#include "core/alias.hpp"
#include "engine/asset/mesh_importer.hpp"
#include "engine/asset/raw_resource.hpp"
#include "engine/render/metal/submesh.hpp"

extern "C"{
    extern void* createMesh(const void* rctxPtr,
        const float* vertices, int32_t numVertices,
        const uint32_t* indices, int32_t numIndices);
    extern void destroyMesh(const void* meshPtr);
}

using namespace ModernBoy;
using namespace ModernBoy::Metal;

Submesh::Submesh(Submesh&& other){ moveFrom(std::move(other)); }
Submesh& Submesh::operator=(Submesh&& other){
    moveFrom(std::move(other));
    return *this;
}
void Submesh::moveFrom(Submesh&& other){
    nativeMesh = other.nativeMesh;
    other.nativeMesh = nullptr;
}

Submesh::Submesh(NativePtr rctxPtr,
    std::span<const Asset::Vertex> vertices,
    std::span<const uint32_t> indices
): nativeMesh(createMesh(rctxPtr,
    reinterpret_cast<const float*>(vertices.data()),
    vertices.size(),
    indices.data(),
    indices.size()
)){}
Submesh::~Submesh(){
    if(nativeMesh)
        destroyMesh(nativeMesh);
}


