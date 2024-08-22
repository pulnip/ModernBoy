#include <print>
#include "CameraComponent.hpp"
#include "LightComponent.hpp"
#include "MeshBuffer.hpp"
#include "MeshComponent.hpp"
#include "ShaderAdaptor.inl"
#include "ShaderAdaptor.hpp"
#include "Vertex.hpp"

using namespace std;
using namespace DirectX::SimpleMath;
using namespace ModernBoy;

ShaderAdaptor::ShaderAdaptor(const ComPtr<ID3D11Device>& device,
    const wstring& vsFileName, const wstring& psFileName)
: rs(createRS(device)), wireState(createWireState(device)){
    // auto [vertices, indices]=MeshBuffer{}.extract();

    // vertexBuffer=createVB<Vertex>(vertices, device);
    // indexCount=static_cast<UINT>(indices.size());
    // indexBuffer=createIB(indices, device);
    vscBuffer=createCB(vsc, device);
    pscBuffer=createCB(psc, device);

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

void ShaderAdaptor::draw(const Matrix& transform,
    const Material& material, const ComPtr<Context>& context
){
    vsc.model=transform;

    vsc.invTranspose=vsc.model;
    vsc.invTranspose.Translation(Vector3::Zero);
    vsc.invTranspose=vsc.invTranspose.Transpose().Invert();

    vsc.view=mainCamera->view();
    vsc.projection=mainCamera->projection();

    psc.eyePos=Vector3::Transform(Vector3::Zero, vsc.view.Transpose());
    psc.material=material;
    for(size_t i=0; i<MAX_LIGHTS; ++i){
        if(i!=lightType){
            psc.lights[i].strength=Vector3::Zero;
        } else{
            psc.lights[i]=lights[i]->light;
        }
    }

    vsc.model=vsc.model.Transpose();
    vsc.view=vsc.view.Transpose();
    vsc.projection=vsc.projection.Transpose();

    updateBuffer(vscBuffer, vsc, context);
    updateBuffer(pscBuffer, psc, context);
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
}
