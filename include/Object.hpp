#pragma once

#include "Math.hpp"
#include "Hit.hpp"
#include "Ray.hpp"
#include "Texture.hpp"

namespace ModernBoy{
    class Object{
      public:
        fRGB ambient = fRGB(0.0f);
        fRGB diffuse = fRGB(0.0f);
        fRGB specular = fRGB(0.0f);
        float alpha = 10.0f;
        float reflection = 0.0;
        float transparency = 0.0;

        std::shared_ptr<Texture> ambTexture;
        std::shared_ptr<Texture> difTexture;

        Object(const fRGB& color = fRGB(1.0f))
        :ambient(color), diffuse(color), specular(color){}

        virtual Hit shootRay(const Ray &ray)=0;
    };
}
