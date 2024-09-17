#pragma once

#include "MeshComponent.hpp"

namespace ModernBoy{
    class GridMeshComponent final: public MeshComponent{
      public:
        GridMeshComponent(Actor& actor,
            const ComPtr<Device>& device,
            const size_t xfrag, const size_t yfrag,
            const std::string& textureName="assets/crate.png"
        );
        ~GridMeshComponent();
    };
}