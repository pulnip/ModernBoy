#pragma once

#include "Attributes.hpp"
#include "Hit.hpp"
#include "Math.hpp"
#include "Ray.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

namespace ModernBoy{
    class Object{
      public:
        Color ambient;
        Color diffuse;
        Color specular;

        float alpha = 10.0f;
        float reflection = 0.0f;
        float transparency = 0.0f;

        std::shared_ptr<Texture> ambTexture;
        std::shared_ptr<Texture> difTexture;

        Object(
            const Color& ambient={0.2f, 0.2f, 0.2f},
            const Color& diffuse=DirectX::Colors::Blue.v,
            const Color& specular=DirectX::Colors::White.v)
        :ambient(ambient), diffuse(diffuse), specular(specular){}
        virtual ~Object()=default;

        virtual Hit shootRay(const Ray& ray) const=0;
    };

    class MeshObject{
      public:
        Transform transform;
        Material material;
        Mesh<Vertex> mesh;
    };
}
