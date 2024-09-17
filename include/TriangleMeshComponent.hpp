#pragma once

#include "MeshComponent.hpp"

namespace ModernBoy{
    class TriangleMeshComponent final: public MeshComponent{
      public:
        TriangleMeshComponent(Actor& actor,
            const ComPtr<Device>& device,
            const std::string& textureName="assets/crate.png"
        );
        ~TriangleMeshComponent();
    };
}