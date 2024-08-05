#pragma once

#include <memory>
#include <directxtk/SimpleMath.h>
#include "Object.hpp"

namespace ModernBoy{
    class Hit{
      public:
        float distance;
        DirectX::SimpleMath::Vector3 point;
        DirectX::SimpleMath::Vector3 normal;
        DirectX::SimpleMath::Vector2 uv{0, 0};

        std::shared_ptr<class Object> object{};
    
      public:
        // Hit() = default;
        // Hit(const Hit&) = default;
        // virtual ~Hit() = default;
    };
}
