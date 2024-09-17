#include "Actor.hpp"
#include "MeshComponent.hpp"
#include "ShaderAdaptor.hpp"
#include "TextureAdaptor.hpp"

using namespace std;
using namespace Microsoft::WRL;
using namespace ModernBoy;

MeshComponent::MeshComponent(Actor& actor, const string& textureName,
    const ComPtr<Device>& device)
: Component(actor), texture(textureName, device){}

MeshComponent::~MeshComponent()=default;

void MeshComponent::draw(ShaderAdaptor& shader, TextureAdaptor& texturer,
    const ComPtr<Context>& context
){
    const auto transform=owner.transform.transform();

    shader.draw(transform, material, context);
    texturer.set(texture.texView, context);
}

void MeshComponent::draw1(ShaderAdaptor& shader,
    const ComPtr<Context>& context
){
    const auto transform=owner.transform.transform();

    shader.draw(transform, material, context);
}

void MeshComponent::draw2(TextureAdaptor& texturer,
    const ComPtr<Context>& context
){
    texturer.set(texture.texView, context);
}
