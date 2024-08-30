#pragma once

#include <memory>
#include "fwd.hpp"

namespace ModernBoy{
    class Component{
      protected:
        Actor& owner;

      public:
        Component(Actor& actor);
        virtual ~Component();
        virtual void update(float dt);
    };
}
