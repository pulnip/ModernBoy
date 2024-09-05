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
        TRIANGLE_MESH,
      // light component
        DEFAULT_LIGHT,
      // camera component
        MAIN_CAMERA,
      // move component
        CAMERA_MOVE,
      // enum for fail test
        UNDEFINED_COMPONENT
    };

    class ComponentFactory{
        Core& core;

      public:
        ComponentFactory(Core& core):core(core){}

        std::shared_ptr<Component> make(
            ComponentType componentType, Actor& actor
        );
    };
}
