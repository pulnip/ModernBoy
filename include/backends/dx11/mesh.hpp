#ifndef __INC_DX11_MESH_HPP
#define __INC_DX11_MESH_HPP

#include "fwd.hpp"
#include "type.hpp"
#include "resource_data.hpp"

namespace ModernBoy::DX11
{
    struct SmolVertex{
        DirectX::XMFLOAT2 position;
        DirectX::XMFLOAT2 texCoord;
        DirectX::XMFLOAT4 color;
    }; ALIGN16(SmolVertex);

    constexpr D3D11_INPUT_ELEMENT_DESC smolVertexDesc[] = {
        {
            .SemanticName = "POSITION",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = offsetof(SmolVertex, position),
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        }, {
            .SemanticName = "TEXCOORD",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = offsetof(SmolVertex, texCoord),
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        }, {
            .SemanticName = "COLOR",
            .SemanticIndex = 0,
            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
            .InputSlot = 0,
            .AlignedByteOffset = offsetof(SmolVertex, color),
            .InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate = 0,
        }
    };

    struct Mesh{
    private:
    public:
        BufferPtr vertexBuffer;
        UINT numVertices;
        BufferPtr indexBuffer;
        UINT numIndices;

        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        ID3D11ShaderResourceView* texture = nullptr;

    public:
        Mesh() = default;
        Mesh(Mesh&& mesh);

        Mesh& operator=(Mesh&& mesh);

        void bind(RenderContext& in_context) const;
    };
    // static_assert(ResourceData<Mesh, RenderContext>);

    extern bool makeRect(DevicePtr& in_device, Mesh& out_mesh);
}

#endif // __INC_DX11_MESH_HPP