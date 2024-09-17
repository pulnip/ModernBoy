#pragma once

#include <memory>
#include <vector>
#include "Object.hpp"

namespace ModernBoy{
    class Raytracer{
      public:
        const ipoint2 resolution;
        Light light;
        std::vector<std::shared_ptr<Object>> objects;

      public:
        Raytracer(const ipoint2& resolution);

        void render(std::vector<RGBA>& pixels);
        Hit closest(const Ray& ray);
        Color traceRay(const Ray& ray, const int level);
        Vector3 toWorld(const Vector2& screenPos) noexcept;
    };
}
