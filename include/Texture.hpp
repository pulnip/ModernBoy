 #pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <string>
#include <iostream>
#include <vector>
#include <gsl-lite/gsl-lite.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace ModernBoy{
    class Texture{
      private:
        using Channel = uint8_t;
        using PixelPos = glm::ivec2;
        using UVPos = glm::vec2;

        PixelPos size;
        int channels;
        std::vector<Channel> image;
    
      public:
        Texture(const std::string& fileName);
        Texture(const PixelPos& size,
            const std::vector<fRGB>& pixels
        );

        fRGB getRGB(const PixelPos& pixelPos) noexcept;
        fRGB getRGB(const UVPos& uv) noexcept;
        fRGB getRGB_lerp(const UVPos& uv) noexcept;

      private:
        glm::vec2 scaleUV(const UVPos& uv) noexcept;
        PixelPos toPixelPos(const UVPos& uv) noexcept;
        gsl::index getIndex(const PixelPos& pixelPos) noexcept;
        iRGB getRGBi(const gsl::index chIndex) noexcept;
        fRGB getRGBf(const gsl::index chIndex) noexcept;
    };
}