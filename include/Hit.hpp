#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <memory>
#include <glm/glm.hpp>
#include "Object.hpp"

namespace ModernBoy{
    class Hit{
      public:
        float distance;
        glm::vec3 point;
        glm::vec3 normal;
        glm::vec2 uv{0.0f};

        std::shared_ptr<class Object> object{};
    
      public:
        // Hit() = default;
        // Hit(const Hit&) = default;
        // virtual ~Hit() = default;
    };
}
