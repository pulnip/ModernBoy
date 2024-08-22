#pragma once

#include <span>
#include <wrl/client.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "helper.hpp"

namespace ModernBoy{
    using namespace std;
    using Microsoft::WRL::ComPtr;

#if defined(DEBUG) || defined(_DEBUG)
    static constexpr bool IS_DEBUG=true;
#else
    static constexpr bool IS_DEBUG=false;
#endif

    static constexpr UINT COMPILE_FLAGS=IS_DEBUG ?
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION : 0;

    [[nodiscard]] inline ComPtr<ID3DBlob> compileShader(
        const wstring& fileName, const char* target
    ){
        ComPtr<ID3DBlob> errorBlob;
        ComPtr<ID3DBlob> shaderBlob;
        HRESULT hr=D3DCompileFromFile(fileName.c_str(),
            nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "main", target, COMPILE_FLAGS, 0,
            &shaderBlob, &errorBlob
        );
        SC_throwIf(hr, errorBlob.Get());

        return shaderBlob;
    }

    [[nodiscard]] inline ComPtr<ID3D11VertexShader> createVS(
        const ComPtr<ID3DBlob>& shaderBlob,
        const ComPtr<ID3D11Device>& device
    ){
        ComPtr<ID3D11VertexShader> vs;
        DX_throwIf(device->CreateVertexShader(
            shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
            nullptr,
            &vs
        ));

        return vs;
    }

    [[nodiscard]] inline ComPtr<ID3D11InputLayout> createIL(
        const ComPtr<ID3DBlob>& shaderBlob,
        const ComPtr<ID3D11Device>& device,
        std::span<const D3D11_INPUT_ELEMENT_DESC> iedesc
    ){
        ComPtr<ID3D11InputLayout> il;
        DX_throwIf(device->CreateInputLayout(
            iedesc.data(), static_cast<UINT>(iedesc.size()),
            shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
            &il
        ));

        return il;
    }

    [[nodiscard]] inline tuple<ComPtr<ID3D11VertexShader>,
        ComPtr<ID3D11InputLayout>
    > createVSAndIL(
        const wstring& fileName,
        const ComPtr<ID3D11Device>& device,
        std::span<const D3D11_INPUT_ELEMENT_DESC> iedesc
    ){
        auto shaderBlob=compileShader(fileName, "vs_5_0");

        return {
            createVS(shaderBlob, device),
            createIL(shaderBlob, device, iedesc)
        };
    }

    [[nodiscard]] inline ComPtr<ID3D11PixelShader> createPS(
        const wstring& fileName,
        const ComPtr<ID3D11Device>& device
    ){
        auto shaderBlob=compileShader(fileName, "ps_5_0");

        ComPtr<ID3D11PixelShader> pixelShader;
        DX_throwIf(device->CreatePixelShader(
            shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
            nullptr,
            &pixelShader
        ));

        return pixelShader;
    }

    [[nodiscard]] inline ComPtr<ID3D11RasterizerState> createRS(
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_RASTERIZER_DESC rd{};
        // ZeroMemory(&rd, sizeof(D3D11_RASTERIZER_DESC));
        rd.FillMode=D3D11_FILL_MODE::D3D11_FILL_SOLID;
        rd.CullMode=D3D11_CULL_MODE::D3D11_CULL_BACK;
        rd.FrontCounterClockwise=false;
        rd.DepthClipEnable=true;

        ComPtr<ID3D11RasterizerState> rs;
        DX_throwIf(device->CreateRasterizerState(&rd, &rs));

        return rs;
    }

    [[nodiscard]] inline ComPtr<ID3D11RasterizerState> createWireState(
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_RASTERIZER_DESC rd{};
        rd.FillMode=D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
        rd.CullMode=D3D11_CULL_MODE::D3D11_CULL_NONE;
        rd.FrontCounterClockwise=false;
        rd.DepthClipEnable=true;

        ComPtr<ID3D11RasterizerState> rs;
        DX_throwIf(device->CreateRasterizerState(&rd, rs.GetAddressOf()));

        return rs;
    }

    template<typename V>
    [[nodiscard]] inline ComPtr<ID3D11Buffer> createVB(
        std::span<const V> vertices,
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_BUFFER_DESC bufferDesc{};
        // ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
        // write access access by CPU and GPU
        // bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
        bufferDesc.Usage=D3D11_USAGE_IMMUTABLE;
        bufferDesc.ByteWidth=UINT(sizeof(V) * vertices.size());
        // use as a vertex buffer
        bufferDesc.BindFlags=D3D11_BIND_VERTEX_BUFFER;
        // allow CPU to write in buffer
        // bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
        bufferDesc.CPUAccessFlags=0;
        bufferDesc.StructureByteStride=sizeof(V);

        D3D11_SUBRESOURCE_DATA vertexBufferData{
            .pSysMem=vertices.data(),
            .SysMemPitch=0,
            .SysMemSlicePitch=0
        };

        ComPtr<ID3D11Buffer> vertexBuffer;
        DX_throwIf(device->CreateBuffer(&bufferDesc,
            &vertexBufferData,
            vertexBuffer.GetAddressOf()
        ));

        return vertexBuffer;
    }

    [[nodiscard]] inline ComPtr<ID3D11Buffer> createIB(
        std::span<const uint16_t> indices,
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_BUFFER_DESC bufferDesc{};
        // write access access by CPU and GPU
        // bufferDesc.Usage=D3D11_USAGE_DYNAMIC;
        bufferDesc.Usage=D3D11_USAGE_IMMUTABLE;
        bufferDesc.ByteWidth=static_cast<UINT>(sizeof(uint16_t) * indices.size());
        // use as a index buffer
        bufferDesc.BindFlags=D3D11_BIND_INDEX_BUFFER;
        // allow CPU to write in buffer
        // bufferDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
        bufferDesc.CPUAccessFlags=0;
        bufferDesc.StructureByteStride=sizeof(uint16_t);

        D3D11_SUBRESOURCE_DATA indexBufferData{
            .pSysMem=indices.data(),
            .SysMemPitch=0,
            .SysMemSlicePitch=0
        };

        ComPtr<ID3D11Buffer> indexBuffer;
        DX_throwIf(device->CreateBuffer(&bufferDesc,
            &indexBufferData,
            indexBuffer.GetAddressOf()
        ));

        return indexBuffer;
    }

    template <typename T>
    [[nodiscard]] inline ComPtr<ID3D11Buffer> createCB(
        const T& constants,
        const ComPtr<ID3D11Device>& device
    ){
        D3D11_BUFFER_DESC cbDesc;
        cbDesc.ByteWidth=sizeof(T);
        cbDesc.Usage=D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags=D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags=D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags=0;
        cbDesc.StructureByteStride=0;

        // Fill in the subresource data.
        D3D11_SUBRESOURCE_DATA constantBufferData{
            .pSysMem=&constants,
            .SysMemPitch=0,
            .SysMemSlicePitch=0
        };

        ComPtr<ID3D11Buffer> constantBuffer;
        DX_throwIf(device->CreateBuffer(&cbDesc,
            &constantBufferData,
            constantBuffer.GetAddressOf()
        ));

        return constantBuffer;
    }

    template <typename T>
    inline void updateBuffer(ComPtr<ID3D11Buffer>& buffer,
        const T& bufferData,
        const ComPtr<ID3D11DeviceContext>& context
    ){
        D3D11_MAPPED_SUBRESOURCE ms;
        context->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        memcpy(ms.pData, &bufferData, sizeof(T));
        context->Unmap(buffer.Get(), 0);
    }
}
