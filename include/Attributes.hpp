#pragma once

#include <mutex>
#include <vector>
#include <gsl-lite/gsl-lite.hpp>
#include <directxtk/SimpleMath.h>
#include <directxmath/DirectXColors.h>
#include "Math.hpp"

namespace ModernBoy{
    struct Transform{
        DirectX::SimpleMath::Vector3 position
            =DirectX::SimpleMath::Vector3::Zero;
        DirectX::SimpleMath::Quaternion quaternion
            =DirectX::SimpleMath::Quaternion::Identity;
        DirectX::SimpleMath::Vector3 scale
            =DirectX::SimpleMath::Vector3::One;

        DirectX::SimpleMath::Matrix transform() const;

      private:
        mutable std::mutex mtx;
    };

    struct Material{
        DirectX::SimpleMath::Color ambient{0.2f, 0.2f, 0.2f};
        DirectX::SimpleMath::Color diffuse=DirectX::Colors::Blue.v;
        DirectX::SimpleMath::Color specular=DirectX::Colors::White.v;
        float shininess=10.0f;

        float reflection=0.0f;
        float transparency=0.0f;
    };

    struct Polygon{
        gsl::index index[3];
    };

    template<typename V> struct Mesh{
        std::vector<V> verticies;
        std::vector<Polygon> polygons;
    };
}
