#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "MeshComponent.hpp"

namespace ModernBoy{
    class CubeMeshComponent final: public MeshComponent{
      public:
        CubeMeshComponent(Actor& actor,
            const Microsoft::WRL::ComPtr<ID3D11Device>& device,
            const std::string& textureName="assets/crate.png"
        );
        ~CubeMeshComponent();
    };
}