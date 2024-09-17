#pragma once

#include <string>
#include <vector>
#include <gsl-lite/gsl-lite.hpp>
#include <directxtk/SimpleMath.h>
#include "fwd.hpp"
#include "Math.hpp"

namespace ModernBoy{
    class Texture{
        ComPtr<ID3D11Texture2D> texture;
      public:
        ComPtr<ID3D11ShaderResourceView> texView;

      public:
        Texture(const std::string& fileName,
            const ComPtr<Device>& device
        );
    };

    class SimpleTexture{
        ipoint2 size;
        std::vector<Channel> image;
        int channels;

      public:
        SimpleTexture(const std::string& fileName);
        SimpleTexture(const PixelPos& size,
            const std::vector<Color>& pixels
        );

        Color getRGB(const PixelPos& pixelPos) noexcept;
        Color getRGB(const UVPos& uv) noexcept;
        Color getRGB_lerp(const UVPos& uv) noexcept;

      private:
        Vector2 scaleUV(const UVPos& uv) noexcept;
        PixelPos toPixelPos(const UVPos& uv) noexcept;
        gsl::index getIndex(const PixelPos& pixelPos) noexcept;
        RGB getRGBi(const gsl::index chIndex) noexcept;
        Color getRGBf(const gsl::index chIndex) noexcept;
    };
}
