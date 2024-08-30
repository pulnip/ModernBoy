#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "MeshComponent.hpp"

namespace ModernBoy{
    class SphereMeshComponent final: public MeshComponent{
      public:
        SphereMeshComponent(Actor& actor,
            const Microsoft::WRL::ComPtr<ID3D11Device>& device,
            const size_t xfrag, const size_t yfrag,
            const std::string& textureName="assets/crate.png"
        );
        ~SphereMeshComponent();
    };
}
