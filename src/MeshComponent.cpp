#include "Actor.hpp"
#include "MeshComponent.hpp"
#include "ShaderAdaptor.hpp"
#include "TextureAdaptor.hpp"

using namespace std;
using namespace Microsoft::WRL;
using namespace ModernBoy;

MeshComponent::MeshComponent(const Actor& actor, const string& textureName,
    const ComPtr<ID3D11Device>& device)
: Component(actor), texture(textureName, device){}

MeshComponent::~MeshComponent()=default;

void MeshComponent::draw(ShaderAdaptor& shader, TextureAdaptor& texturer,
    const ComPtr<ID3D11DeviceContext>& context
){
    const auto transform=owner.transform.transform();

    shader.draw(transform, material, context);
    texturer.set(texture.texView, context);
}

void MeshComponent::draw1(ShaderAdaptor& shader,
    const ComPtr<ID3D11DeviceContext>& context
){
    const auto transform=owner.transform.transform();

    shader.draw(transform, material, context);
}

void MeshComponent::draw2(TextureAdaptor& texturer,
    const ComPtr<ID3D11DeviceContext>& context
){
    texturer.set(texture.texView, context);
}
