#pragma once

#include "Object.hpp"

namespace ModernBoy{
    class Sphere: public Object{
      public:
        glm::vec3 center;
        float radius;

        Sphere(const glm::vec3 &center, const float radius,
        const fRGB &color = fRGB(1.0f))
        :center(center), radius(radius), Object(color){}

        Hit shootRay(const Ray &ray) override;
    };

    class Triangle: public Object{
      public:
        glm::vec3 v0, v1, v2;
        glm::vec2 uv0, uv1, uv2;

        Triangle(
            const glm::vec3& v0=glm::vec3(0.0f),
            const glm::vec3& v1=glm::vec3(0.0f),
            const glm::vec3& v2=glm::vec3(0.0f),
            const glm::vec2& uv0=glm::vec2(0.0f),
            const glm::vec2& uv1=glm::vec2(0.0f),
            const glm::vec2& uv2=glm::vec2(0.0f))
        :v0(v0), v1(v1), v2(v2), uv0(uv0), uv1(uv1), uv2(uv2){}

        Hit shootRay(const Ray &ray) override;
    };

    class Square: public Object{
      public:
        Triangle t1, t2;

        Square(
            const glm::vec3& v0=glm::vec3(0.0f),
            const glm::vec3& v1=glm::vec3(0.0f),
            const glm::vec3& v2=glm::vec3(0.0f),
            const glm::vec3& v3=glm::vec3(0.0f),
            const glm::vec2& uv0=glm::vec2(0.0f),
            const glm::vec2& uv1=glm::vec2(0.0f),
            const glm::vec2& uv2=glm::vec2(0.0f),
            const glm::vec2& uv3=glm::vec2(0.0f))
        :t1(v0, v1, v2, uv0, uv1, uv2)
        ,t2(v0, v2, v3, uv0, uv2, uv3){
            std::cout<<glm::distance(v0, v1)<<std::endl;
            std::cout<<glm::distance(v1, v2)<<std::endl;
            std::cout<<glm::distance(v2, v3)<<std::endl;
            std::cout<<glm::distance(v3, v0)<<std::endl;
        }

        Hit shootRay(const Ray &ray) override;
    };
}
