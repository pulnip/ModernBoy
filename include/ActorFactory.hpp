#pragma once 

#include <memory>
#include <string>
#include "fwd.hpp"

namespace ModernBoy{
    enum class ActorType{
      // enum for fail test
        UNDEFINED,
      // actor which has no component
        NIL,
      // camera actor
        CAMERA,
      // light actor
        DIRECTIONAL_LIGHT,
        POINT_LIGHT,
        SPOT_LIGHT,
      // Mesh Object which has color surface
        COLOR_CUBE,
        COLOR_CYLINDER,
        COLOR_SPHERE,
      // Mesh Object which has texture surface
        TEXTURE_GRID,
        TEXTURE_CUBE,
        TEXTURE_CYLINDER,
        TEXTURE_SPHERE
    };

    class ActorFactory{
        static std::string textureFileName;

      public:
        static std::shared_ptr<Actor> make(
            ActorType actorType, Core& core
        );
    };
}
