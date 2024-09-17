#include <print>
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
#include "MeshBuffer.hpp"
#include "MeshComponent.hpp"
#include "ShaderAdaptor.inl"
#include "ShaderAdaptor.hpp"
#include "Vertex.hpp"

#include <directxtk/DDSTextureLoader.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

ShaderAdaptor::ShaderAdaptor(const ComPtr<Device>& device,
    const wstring& vsFileName, const wstring& psFileName)
: rs(createRS(device)), wireState(createWireState(device)){
    // auto [vertices, indices]=MeshBuffer{}.extract();

    // vertexBuffer=createVB<Vertex>(vertices, device);
    // indexCount=static_cast<UINT>(indices.size());
    // indexBuffer=createIB(indices, device);
    vscBuffer=createCB(vsc, device);
    pscBuffer=createCB(psc, device);
    nvcBuffer=createCB(nvc, device);

    // create shader
    constexpr D3D11_INPUT_ELEMENT_DESC inputElements[]={
        {
            .SemanticName="POSITION",
            .SemanticIndex=0,
            .Format=DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot=0,
            .AlignedByteOffset=0,
            .InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate=0
        }, {
            .SemanticName="NORMAL",
            .SemanticIndex=0,
            .Format=DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot=0,
            .AlignedByteOffset=3*sizeof(float),
            .InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate=0
        }, {
        //     .SemanticName="COLOR",
        //     .SemanticIndex=0,
        //     .Format=DXGI_FORMAT_R32G32B32A32_FLOAT,
        //     .InputSlot=0,
        //     .AlignedByteOffset=6*sizeof(float),
        //     .InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA,
        //     .InstanceDataStepRate=0
        // }, {
            .SemanticName="TEXCOORD",
            .SemanticIndex=0,
            .Format=DXGI_FORMAT_R32G32_FLOAT,
            .InputSlot=0,
            .AlignedByteOffset=6*sizeof(float),
            .InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate=0
        }
    };
    tie(vs, il)=createVSAndIL(vsFileName, device, inputElements);
    ps=createPS(psFileName, device);

    tie(nvs, nil)=createVSAndIL(L"src/NormalVS.hlsl", device, inputElements);
    nps=createPS(L"src/NormalPS.hlsl", device);

    cvcBuffer=createCB(cmc, device);

    constexpr D3D11_INPUT_ELEMENT_DESC cubeMappingIEs[]={
        {
            .SemanticName="POSITION",
            .SemanticIndex=0,
            .Format=DXGI_FORMAT_R32G32B32_FLOAT,
            .InputSlot=0,
            .AlignedByteOffset=0,
            .InputSlotClass=D3D11_INPUT_PER_VERTEX_DATA,
            .InstanceDataStepRate=0
        }
    };

    tie(cvs, cil)=createVSAndIL(L"src/CubeMappingVS.hlsl", device, cubeMappingIEs);
    cps=createPS(L"src/CubeMappingPS.hlsl", device);

    ComPtr<ID3D11Texture2D> texture;
    DX_throwIf(CreateDDSTextureFromFileEx(
        device.Get(), L"assets/skybox.dds", 0,
        D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0,
        D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_FLAGS(false),
        (ID3D11Resource**)texture.GetAddressOf(), crv.GetAddressOf(),
        nullptr
    ));

    // duplicate from CubeMeshComponent
    constexpr float cubeScale=20.0f;

    auto leftBottomBack=cubeScale*Vector3(-1.0f, -1.0f, -1.0f);
    auto leftBottomFront=cubeScale*Vector3(-1.0f, -1.0f, 1.0f);
    auto leftTopBack=cubeScale*Vector3(-1.0f, 1.0f, -1.0f);
    auto leftTopFront=cubeScale*Vector3(-1.0f, 1.0f, 1.0f);
    auto rightBottomBack=cubeScale*Vector3(1.0f, -1.0f, -1.0f);
    auto rightBottomFront=cubeScale*Vector3(1.0f, -1.0f, 1.0f);
    auto rightTopBack=cubeScale*Vector3(1.0f, 1.0f, -1.0f);
    auto rightTopFront=cubeScale*Vector3(1.0f, 1.0f, 1.0f);

    std::vector cubeVertices{
        leftTopBack,
        leftTopFront,
        rightTopFront,
        rightTopBack,
        leftBottomBack,
        leftBottomFront,
        rightBottomFront,
        rightBottomBack
    };
    std::vector<Polygon> cubePolygons{
        {0, 1, 2}, {0, 2, 3},
        {0, 4, 5}, {0, 5, 1},
        {0, 3, 7}, {0, 7, 4},
        {3, 2, 7}, {2, 6, 7},
        {1, 5, 6}, {1, 6, 2},
        {4, 6, 5}, {4, 7, 6}
    };

    // duplicate from Mesh::extract
    std::vector<uint16_t> cubeIndices;
    cubeIndices.reserve(3*cubePolygons.size());
    for(const auto& polygon: cubePolygons){
        cubeIndices.emplace_back(polygon.index[0]);
        cubeIndices.emplace_back(polygon.index[1]);
        cubeIndices.emplace_back(polygon.index[2]);
    }
    reverse(cubeIndices.begin(), cubeIndices.end());

    // duplicate from ShaderAdaptor::loadMesh
    cvb=createVB<Vector3>(cubeVertices, device);
    cic=static_cast<UINT>(cubeIndices.size());
    cib=createIB(cubeIndices, device);
}

void ShaderAdaptor::loadMesh(const Mesh<Vertex>& mesh,
    const ComPtr<Device>& device
){
    auto [vertices, indices]=mesh.extract();

    psc.useTexture=true;

    vertexBuffer=createVB<Vertex>(vertices, device);
    indexCount=static_cast<UINT>(indices.size());
    indexBuffer=createIB(indices, device);
}

void ShaderAdaptor::loadNormal(const Mesh<Vertex>& mesh,
    const ComPtr<Device>& device
){
    auto [vertices, indices]=mesh.extractNormal();

    nvb=createVB<Vertex>(vertices, device);
    nic=static_cast<UINT>(indices.size());
    nib=createIB(indices, device);
}

void ShaderAdaptor::draw(const Matrix& transform,
    const Material& material, const ComPtr<Context>& context
){
    vsc.model=transform;

    vsc.invTranspose=vsc.model;
    vsc.invTranspose.Translation(Vector3::Zero);
    vsc.invTranspose=vsc.invTranspose.Transpose().Invert();

    vsc.view=mainCamera->view();
    vsc.projection=mainCamera->projection();

    psc.eyePos=vsc.view.Translation();
    // right handed
    psc.eyePos.z = -psc.eyePos.z;

    vsc.model=vsc.model.Transpose();
    vsc.view=vsc.view.Transpose();
    vsc.projection=vsc.projection.Transpose();
    updateBuffer(vscBuffer, vsc, context);

    static float oldNormalScale=0.0f;
    if(oldNormalScale!=normalScale){
        nvc.scale=normalScale;
        updateBuffer(nvcBuffer, nvc, context);
        oldNormalScale=normalScale;
    }

    psc.material=material;
    for(size_t i=0; i<MAX_LIGHTS; ++i){
        if(i!=lightType){
            psc.lights[i].strength=Vector3::Zero;
        } else{
            psc.lights[i]=lights[i]->light;
        }
    }

    psc.rimStrength=rimStrength;
    psc.rimPower=rimPower;

    updateBuffer(pscBuffer, psc, context);

    cmc.view=vsc.view;
    cmc.projection=vsc.projection;

    updateBuffer(cvcBuffer, cmc, context); 
}

void ShaderAdaptor::setWireFrame(bool enable, const ComPtr<Context>& context){
    if(enable)
        context->RSSetState(wireState.Get());
    else
        context->RSSetState(rs.Get());
}

void ShaderAdaptor::render(const ComPtr<Context>& context){
    // IA: Input-Assembler stage
    // VS: Vertex Shader
    // PS: Pixel Shader
    // RS: Rasterizer stage
    // OM: Output-Merger stage

    // set the shader objects
    context->VSSetConstantBuffers(0, 1, vscBuffer.GetAddressOf());
    context->VSSetShader(vs.Get(), 0, 0);
    context->PSSetConstantBuffers(0, 1, pscBuffer.GetAddressOf());
    context->PSSetShader(ps.Get(), 0, 0);
    // context->RSSetState(rs.Get());

    // select which vertex buffer to display
    UINT stride=sizeof(Vertex);
    UINT offset=0;
    context->IASetInputLayout(il.Get());
    context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(),
        &stride, &offset
    );
    context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );
    context->DrawIndexed(indexCount, 0, 0);

    // render cube map
    context->VSSetConstantBuffers(0, 1, cvcBuffer.GetAddressOf());
    context->VSSetShader(cvs.Get(), 0, 0);
    context->PSSetShader(cps.Get(), 0, 0);

    stride=sizeof(Vector3);
    offset=0;
    context->IASetInputLayout(cil.Get());
    context->IASetVertexBuffers(0, 1, cvb.GetAddressOf(),
        &stride, &offset
    );
    context->IASetIndexBuffer(cib.Get(), DXGI_FORMAT_R16_UINT, 0);

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
    );

    // move to texturer?
    ID3D11ShaderResourceView* views[]={ crv.Get() };
    context->PSSetShaderResources(0, 1, views);

    context->DrawIndexed(cic, 0, 0);
}

void ShaderAdaptor::renderNormal(const ComPtr<Context>& context){
    ID3D11Buffer* vscs[]={vscBuffer.Get(), nvcBuffer.Get()};
    context->VSSetConstantBuffers(0, sizeof(vscs)/sizeof(ID3D11Buffer*), vscs);
    context->VSSetShader(nvs.Get(), 0, 0);
    context->PSSetShader(nps.Get(), 0, 0);

    UINT stride=sizeof(Vertex);
    UINT offset=0;
    context->IASetInputLayout(nil.Get());
    context->IASetVertexBuffers(0, 1, nvb.GetAddressOf(),
        &stride, &offset
    );
    context->IASetIndexBuffer(nib.Get(), DXGI_FORMAT_R16_UINT, 0);

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST
    );
    context->DrawIndexed(nic, 0, 0);
}
