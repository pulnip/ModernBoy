#pragma once 

#include <memory>
#include <string>
#include "fwd.hpp"

namespace ModernBoy{
    enum class ActorType{
      // actor which has no component
        NIL,
      // camera actor
        CAMERA,
      // light actor
        LIGHT,
      // Mesh Object which has color surface
        COLOR_CUBE,
        COLOR_CYLINDER,
        COLOR_SPHERE,
      // Mesh Object which has texture surface
        TEXTURE_CUBE,
        TEXTURE_GRID,
        TEXTURE_CYLINDER,
        TEXTURE_SPHERE,
      // enum for fail test
        UNDEFINED
    };

    class ActorFactory{
        Core& core;

      public:
        ActorFactory(Core& core):core(core){}
        std::shared_ptr<Actor> make(ActorType actorType);
    };
}
