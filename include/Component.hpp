#pragma once

#include <memory>
#include "fwd.hpp"

namespace ModernBoy{
    class Component{
      protected:
        Actor& owner;

      public:
        Component(Actor& actor):owner(actor){}

        virtual ~Component()=default;
        virtual void update([[maybe_unused]] float dt){};
    };
}
