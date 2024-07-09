#pragma once

#include <memory>
#include <vector>
#include "Object.hpp"

namespace ModernBoy{
    class Rasterizer{
      public:
        const glm::ivec2 resolution;
        std::vector<std::shared_ptr<Object>> objects;

      public:
        Rasterizer(const glm::ivec2& resolution);

        void render(std::vector<fRGBA>& pixels);
        glm::vec2 toRaster(const glm::vec3& worldPos);
    };
}
