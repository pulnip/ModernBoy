#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>

namespace ModernBoy{
    class Ray{
      public:
        glm::vec3 p0;
        glm::vec3 dir;
    };
}
