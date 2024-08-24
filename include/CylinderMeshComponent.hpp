#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "MeshComponent.hpp"

namespace ModernBoy{
    class CylinderMeshComponent final: public MeshComponent{
      public:
        CylinderMeshComponent(const Actor& actor,
            const Microsoft::WRL::ComPtr<ID3D11Device>& device,
            const size_t xfrag, const size_t yfrag,
            const std::string& textureName="assets/crate.png"
        );
        ~CylinderMeshComponent();
    };
}
