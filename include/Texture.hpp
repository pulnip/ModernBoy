#pragma once

#include <string>
#include <vector>
#include <gsl-lite/gsl-lite.hpp>
#include <directxtk/SimpleMath.h>
#include "Math.hpp"

namespace ModernBoy{
    class Texture{
      private:
        ipoint2 size;
        std::vector<Channel> image;
        int channels;
    
      public:
        Texture(const std::string& fileName);
        Texture(const PixelPos& size,
            const std::vector<DirectX::SimpleMath::Color>& pixels
        );

        DirectX::SimpleMath::Color getRGB(const PixelPos& pixelPos) noexcept;
        DirectX::SimpleMath::Color getRGB(const UVPos& uv) noexcept;
        DirectX::SimpleMath::Color getRGB_lerp(const UVPos& uv) noexcept;

      private:
        DirectX::SimpleMath::Vector2 scaleUV(const UVPos& uv) noexcept;
        PixelPos toPixelPos(const UVPos& uv) noexcept;
        gsl::index getIndex(const PixelPos& pixelPos) noexcept;
        RGB getRGBi(const gsl::index chIndex) noexcept;
        DirectX::SimpleMath::Color getRGBf(const gsl::index chIndex) noexcept;
    };
}