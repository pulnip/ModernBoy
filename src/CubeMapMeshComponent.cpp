#include <directxtk/DDSTextureLoader.h>
#include "CubeMapMeshComponent.hpp"
#include "helper.hpp"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace ModernBoy;

CubeMapMeshComponent::CubeMapMeshComponent(Actor& actor,
    const ComPtr<ID3D11Device>& device,
    const string& texName)
: MeshComponent(actor, texName, device){
    ComPtr<ID3D11Texture2D> texture;
    DX_throwIf(CreateDDSTextureFromFileEx(
        device.Get(), L"assets/skybox.dds", 0,
        D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0,
        D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_FLAGS(false),
        (ID3D11Resource**)texture.GetAddressOf(), crv.GetAddressOf()
    ));
}

CubeMapMeshComponent::~CubeMapMeshComponent()=default;
