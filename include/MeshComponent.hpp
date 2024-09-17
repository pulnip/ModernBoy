#pragma once

#include <memory>
#include <string>
#include <tuple>
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
        MeshComponent(Actor& actor, const std::string& textureName,
            const ComPtr<Device>& device
        );
        virtual ~MeshComponent();

        void draw(ShaderAdaptor& shader, TextureAdaptor& texturer,
            const ComPtr<Context>& context
        );
        void draw1(ShaderAdaptor& shader,
            const ComPtr<Context>& context
        );
        void draw2(TextureAdaptor& texturer,
            const ComPtr<Context>& context
        );
    };
}