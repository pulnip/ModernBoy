#include "backends/dx11/context.hpp"
#include "backends/dx11/mesh.hpp"

using namespace DirectX;
using namespace ModernBoy::DX11;

Mesh::Mesh(Mesh&& other){ moveFrom(std::move(other)); }
Mesh& Mesh::operator=(Mesh&& other){
    moveFrom(std::move(other));
    return *this;
}
void Mesh::moveFrom(Mesh&& other){
    vertexBuffer.Swap(other.vertexBuffer);
    numVertices = other.numVertices;
    indexBuffer.Swap(other.indexBuffer);
    numIndices = other.numIndices;
}

void Mesh::bind(RenderContext& in_context) const{
    UINT stride = sizeof(SmolVertex);
    UINT offset = 0;

    auto& context = in_context.context;
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(),
        &stride, &offset);
    context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(topology);
}

template<typename T>
static bool InitVertexBuffer(DevicePtr& in_device,
    const T& in_vertices, BufferPtr& out_buffer)
{
    D3D11_BUFFER_DESC vbDesc;
    vbDesc.ByteWidth=sizeof(in_vertices);
    vbDesc.Usage=D3D11_USAGE_IMMUTABLE;
    vbDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags=0;
    vbDesc.MiscFlags=0;
    vbDesc.StructureByteStride=sizeof(in_vertices[0]);

    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA vertexBufferData{
        .pSysMem=in_vertices,
        .SysMemPitch=0,
        .SysMemSlicePitch=0
    };

    if(in_device->CreateBuffer(&vbDesc, &vertexBufferData,
        out_buffer.GetAddressOf()) < 0) return false;

    return true;
}

template<typename T>
static bool InitIndexBuffer(DevicePtr& in_device,
    const T& in_indices, BufferPtr& out_buffer)
{
    D3D11_BUFFER_DESC ibDesc;
    ibDesc.ByteWidth=sizeof(in_indices);
    ibDesc.Usage=D3D11_USAGE_IMMUTABLE;
    ibDesc.BindFlags=D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags=0;
    ibDesc.MiscFlags=0;
    ibDesc.StructureByteStride=sizeof(in_indices[0]);

    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA indexBufferData{
        .pSysMem=in_indices,
        .SysMemPitch=0,
        .SysMemSlicePitch=0
    };

    if(in_device->CreateBuffer(&ibDesc, &indexBufferData,
        out_buffer.GetAddressOf()) < 0) return false;

    return true;
}

bool ModernBoy::DX11::makeRect(DevicePtr& in_device, Mesh& out_mesh){
    SmolVertex vertices[] = {
        {{-0.5f,  0.5f, 0.0f}, 0xFF000000},
        {{ 0.5f,  0.5f, 0.0f}, 0x00FF0055},
        {{ 0.5f, -0.5f, 0.0f}, 0x0000FFAA},
        {{-0.5f, -0.5f, 0.0f}, 0xFFFFFFFF},
    };
    UINT indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    out_mesh.numVertices = sizeof(vertices) / sizeof(vertices[0]);
    out_mesh.numIndices = sizeof(indices) / sizeof(indices[0]);
    if(!InitVertexBuffer(in_device, vertices,
        out_mesh.vertexBuffer)) return false;
    if(!InitIndexBuffer(in_device, indices,
        out_mesh.indexBuffer)) return false;

    return true;
}
static XMFLOAT3 NormalizeInit(float x, float y, float z){
    XMVECTOR v = XMVectorSet(x, y, z, 0.0f);
    XMFLOAT3 out;
    DirectX::XMStoreFloat3(&out, DirectX::XMVector3Normalize(v));
    return out;
}
bool ModernBoy::DX11::makeTetra(DevicePtr& in_device, Mesh& out_mesh){
    SmolVertex vertices[] = {
        {NormalizeInit( 1.0f,  1.0f,  1.0f), 0xFFFFFFFF},
        {NormalizeInit(-1.0f, -1.0f,  1.0f), 0xFF0000AA},
        {NormalizeInit(-1.0f,  1.0f, -1.0f), 0x00FF0055},
        {NormalizeInit( 1.0f, -1.0f, -1.0f), 0x0000FF00},
    };
    UINT indices[] = {
        0, 1, 2,
        0, 3, 1,
        0, 2, 3,
        1, 3, 2
    };

    out_mesh.numVertices = sizeof(vertices) / sizeof(vertices[0]);
    out_mesh.numIndices = sizeof(indices) / sizeof(indices[0]);
    if(!InitVertexBuffer(in_device, vertices,
        out_mesh.vertexBuffer)) return false;
    if(!InitIndexBuffer(in_device, indices,
        out_mesh.indexBuffer)) return false;

    return true;
}