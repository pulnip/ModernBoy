#pragma once

#include "MeshComponent.hpp"

namespace ModernBoy{
    class CubeMapMeshComponent: public MeshComponent{
        ComPtr<ID3D11ShaderResourceView> crv;

      public:
        CubeMapMeshComponent(Actor& actor,
            const ComPtr<Device>& device,
            const std::string& textureName="assets/crate.png"
        );
        ~CubeMapMeshComponent();
    };
}
