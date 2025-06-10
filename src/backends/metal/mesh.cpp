#include <utility>
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
    int numVertices = sizeof(vertices)/sizeof(float);
    // uint32_t indices[3] = { 0, 1, 2 };
    // int numIndices = sizeof(indices)/sizeof(uint32_t);

    MeshPtr meshPtr = createMesh(layerPtr,
        vertices, numVertices, nullptr, 0);
    return Mesh(meshPtr);
}