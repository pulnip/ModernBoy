#pragma once

#include "fwd.hpp"

namespace ModernBoy{
    class TextureAdaptor{
        ComPtr<ID3D11SamplerState> sampler;

      public:
        TextureAdaptor(const ComPtr<Device>& device);
        void set(const ComPtr<ID3D11ShaderResourceView>& texView,
            const ComPtr<Context>& context
        );
    };
}
