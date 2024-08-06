#pragma once

#include "Math.hpp"

namespace ModernBoy{
    class Ray{
      public:
        Vector3 p0=Vector3::Zero;
        Vector3 dir=Vector3::UnitZ;
    };
}
