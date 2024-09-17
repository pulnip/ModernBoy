#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "MeshComponent.hpp"

namespace ModernBoy{
    class CubeMapMeshComponent: public MeshComponent{
        template<typename T> using ComPtr=Microsoft::WRL::ComPtr<T>;

        ComPtr<ID3D11ShaderResourceView> crv;

      public:
        CubeMapMeshComponent(Actor& actor,
            const ComPtr<ID3D11Device>& device,
            const std::string& textureName="assets/crate.png"
        );
        ~CubeMapMeshComponent();
    };
}
