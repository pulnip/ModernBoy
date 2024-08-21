#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <wrl/client.h>
#include <d3d11.h>
#include "Attributes.hpp"
#include "Component.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

namespace ModernBoy{
    class MeshComponent: public Component{
      protected:
        Material material;
        Texture texture;

      public:
        Mesh<Vertex> mesh;

      public:
        MeshComponent(const Actor& actor, const std::string& textureName,
            const Microsoft::WRL::ComPtr<ID3D11Device>& device
        );
        virtual ~MeshComponent();

        void draw(ShaderAdaptor& shader, TextureAdaptor& texturer,
            const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context
        );
        void draw1(ShaderAdaptor& shader,
            const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context
        );
        void draw2(TextureAdaptor& texturer,
            const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context
        );
    };
}