#include <string>
#include <utility>
#include <vector>
#include "core/alias.hpp"
#include "engine/asset/mesh_importer.hpp"
#include "engine/asset/raw_resource.hpp"
#include "engine/render/metal/mesh.hpp"

extern "C"{
    extern void* createMesh(const void* rctxPtr,
        const float* vertices, int32_t numVertices,
        const uint32_t* indices, int32_t numIndices);
    extern void destroyMesh(const void* meshPtr);
}

using namespace ModernBoy;
using namespace ModernBoy::Metal;

Mesh::Mesh(Mesh&& other){ moveFrom(std::move(other)); }
Mesh& Mesh::operator=(Mesh&& other){
    moveFrom(std::move(other));
    return *this;
}
void Mesh::moveFrom(Mesh&& other){
    meshPtr = other.meshPtr;
    other.meshPtr.clear();
}

Mesh::Mesh(NativePtr rctxPtr,
    const std::string& fileName)
{
    auto rawMesh = Asset::importEmbedded<Asset::RawMesh>(fileName);
    meshPtr.reserve(rawMesh.size());

    for(const auto& part: rawMesh){
        meshPtr.emplace_back(createMesh(
            rctxPtr,
            reinterpret_cast<const float*>(part.vertices.data()), part.vertices.size(),
            part.indices.data(), part.indices.size()
        ));
    }
}
Mesh::Mesh(NativePtr rctxPtr, const Asset::CookedMesh& cooked){
    for(const auto& submesh: cooked.submeshInfoTable){
        auto vertices = &cooked.vertices[submesh.verticesSectionIndex];
        auto indices = &cooked.indices[submesh.indicesSectionIndex];

        meshPtr.emplace_back(createMesh(
            rctxPtr,
            reinterpret_cast<const float*>(vertices),
            submesh.vertexCount,
            indices, submesh.indexCount
        ));
    }
}

Mesh::~Mesh(){
    for(const auto& part: meshPtr)
        destroyMesh(part);
}

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
    std::span<Asset::Vertex> vertices,
    std::span<uint32_t> indices
): nativeMesh(createMesh(rctxPtr,
    reinterpret_cast<float*>(vertices.data()),
    vertices.size(),
    indices.data(),
    indices.size()
)){}
Submesh::~Submesh(){
    if(nativeMesh)
        destroyMesh(nativeMesh);
}


