#pragma once

#include "Math.hpp"
#include "Hit.hpp"
#include "Ray.hpp"
#include "Texture.hpp"
#include "Attributes.hpp"

namespace ModernBoy{
    class Object{
      public:
        fRGB ambient;
        fRGB diffuse;
        fRGB specular;

        float alpha = 10.0f;
        float reflection = 0.0f;
        float transparency = 0.0f;

        std::shared_ptr<Texture> ambTexture;
        std::shared_ptr<Texture> difTexture;

        Object(
            const fRGB& ambient = fDUNE,
            const fRGB& diffuse = fBLUE,
            const fRGB& specular = fWHITE)
        :ambient(ambient), diffuse(diffuse), specular(specular){}

        virtual Hit shootRay(const Ray& ray) const=0;
    };

    class MeshObject{
        Transform transform;
        Material material;
        Mesh mesh;
    };
}
