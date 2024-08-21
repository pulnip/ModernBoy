#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "MeshComponent.hpp"

namespace ModernBoy{
    class TriangleMeshComponent final: public MeshComponent{
      public:
        TriangleMeshComponent(const Actor& actor,
            const Microsoft::WRL::ComPtr<ID3D11Device>& device,
            const std::string& textureName="assets/crate.png"
        );
        ~TriangleMeshComponent();
    };
}