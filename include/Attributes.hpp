#pragma once

#include <mutex>
#include <vector>
#include <gsl-lite/gsl-lite.hpp>
#include <directxtk/SimpleMath.h>
#include <directxmath/DirectXColors.h>
#include "Math.hpp"

namespace ModernBoy{
    struct Transform{
        Vector3 position=Vector3::Zero;
        DirectX::SimpleMath::Quaternion quaternion
            =DirectX::SimpleMath::Quaternion::Identity;
        Vector3 scale=Vector3::One;

        DirectX::SimpleMath::Matrix transform() const;

      private:
        mutable std::mutex mtx;
    };

    struct Material{
        Vector3 ambient{0.1f, 0.1f, 0.1f};
        float shininess=1.0f;
        Vector3 diffuse{0.5f, 0.5f, 0.5f};
        float reflection=0.0f;
        Vector3 specular{0.5f, 0.5f, 0.5f};
        float transparency=0.0f;
    };
    // check 16-byte aligned
    static_assert((sizeof(Material) & 0xf) == 0);

    struct Light{
        Vector3 strength=Vector3::One;
        float fallOffStart=0.0f;
        Vector3 dir=Vector3::UnitZ;
        float fallOffEnd=10.0f;
        Vector3 pos=-2*Vector3::UnitZ;
        float spotPower=1.0f;
    };
    // check 16-byte aligned
    static_assert((sizeof(Light) & 0xf) == 0);

    struct Polygon{
        gsl::index index[3];
    };

    template<typename V> struct Mesh{
        std::vector<V> verticies;
        std::vector<Polygon> polygons;
    };
}
