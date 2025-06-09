#include "backends/metal/context.hpp"
#include "backends/metal/mesh.hpp"

using namespace ModernBoy::Metal;

Mesh::Mesh(Mesh&& other){ moveFrom(std::move(other)); }
Mesh& Mesh::operator=(Mesh&& other){
    moveFrom(std::move(other));
    return *this;
}
void Mesh::moveFrom(Mesh&& other){
    vertexBuffer = other.vertexBuffer;
    other.vertexBuffer = nullptr;
    numVertices = other.numVertices;
    indexBuffer = other.indexBuffer;
    other.indexBuffer = nullptr;
    numIndices = other.numIndices;
}

void Mesh::bind(RenderContext& in_context) const{
    uint32_t stride = sizeof(6);
    uint32_t offset = 0;

    auto layer = in_context.metalLayer;
    // context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(),
    //     &stride, &offset);
    // context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    // context->IASetPrimitiveTopology(topology);
}
