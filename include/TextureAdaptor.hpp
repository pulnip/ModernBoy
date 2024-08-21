#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "fwd.hpp"

namespace ModernBoy{
    class TextureAdaptor{
        template<typename T> using ComPtr=Microsoft::WRL::ComPtr<T>;

        ComPtr<ID3D11SamplerState> sampler;

      public:
        TextureAdaptor(const ComPtr<ID3D11Device>& device);
        void set(const ComPtr<ID3D11ShaderResourceView>& texView,
            const ComPtr<ID3D11DeviceContext>& context
        );
    };
}
