#pragma once

#include <memory>
#include <directxtk/SimpleMath.h>
#include "Object.hpp"

namespace ModernBoy{
    class Hit{
      public:
        float distance;
        Vector3 point;
        Vector3 normal;
        Vector2 uv{0, 0};

        std::shared_ptr<class Object> object{};
    
      public:
        // Hit() = default;
        // Hit(const Hit&) = default;
        // virtual ~Hit() = default;
    };
}
