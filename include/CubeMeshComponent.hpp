#pragma once

#include "MeshComponent.hpp"

namespace ModernBoy{
    class CubeMeshComponent final: public MeshComponent{
      public:
        CubeMeshComponent(Actor& actor,
            const ComPtr<Device>& device,
            const std::string& textureName="assets/crate.png"
        );
        ~CubeMeshComponent();
    };
}