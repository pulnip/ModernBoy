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
Mesh::Mesh(NativePtr, const Asset::CookedMesh&){}
Mesh::~Mesh(){
    for(const auto& part: meshPtr)
        destroyMesh(part);
}
