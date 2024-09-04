#pragma once

#include <memory>
#include "fwd.hpp"

namespace ModernBoy{
    enum class ComponentType{
      // null object
        NIL_COMPONENT,
      // mesh component
        CUBE_MESH,
        GRID_MESH,
        CYLINDER_MESH,
        SPHERE_MESH,
      // enum for fail test
        UNDEFINED_COMPONENT
    };

    class ComponentFactory{
      public:
        static std::shared_ptr<Component> make(
            ComponentType componentType, Actor& actor
        );
    };
}
