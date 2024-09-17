#include <print>
#include "helper.hpp"
#include "Texture.hpp"
#include "TextureAdaptor.hpp"

using namespace std;
using namespace Microsoft::WRL;
using namespace ModernBoy;

namespace ModernBoy{
    [[nodiscard]] inline ComPtr<ID3D11SamplerState> createSS(
        const ComPtr<Device>& device
    ){
        D3D11_SAMPLER_DESC sd{};
        // ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));
        sd.Filter=D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sd.AddressU=D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressV=D3D11_TEXTURE_ADDRESS_WRAP;
        sd.AddressW=D3D11_TEXTURE_ADDRESS_WRAP;
        sd.ComparisonFunc=D3D11_COMPARISON_NEVER;
        sd.MinLOD=0;
        sd.MaxLOD=D3D11_FLOAT32_MAX;

        ComPtr<ID3D11SamplerState> sampler;
        DX_throwIf(device->CreateSamplerState(&sd,
            sampler.GetAddressOf()
        ));

        return sampler;
    }
}

TextureAdaptor::TextureAdaptor(const ComPtr<Device>& device)
: sampler(createSS(device)){}

void TextureAdaptor::set(const ComPtr<ID3D11ShaderResourceView>& texView,
    const ComPtr<Context>& context
){
    vector<ID3D11ShaderResourceView*> resources;
    resources.emplace_back(texView.Get());

    context->PSSetShaderResources(0, resources.size(), resources.data());
    context->PSSetSamplers(0, 1, sampler.GetAddressOf());
}
