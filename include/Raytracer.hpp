#pragma once

#include <memory>
#include <vector>
#include "Light.hpp"
#include "Object.hpp"

namespace ModernBoy{
    class Raytracer{
      public:
        const glm::ivec2 resolution;
        Light light;
        std::vector<std::shared_ptr<Object>> objects;

      public:
        Raytracer(const glm::ivec2& resulution);

        void render(std::vector<glm::vec4>& pixels);
        Hit closest(const Ray& ray);
        fRGB traceRay(const Ray& ray, const int level);
        glm::vec3 toWorld(const glm::vec2& screenPos);
    };
}
