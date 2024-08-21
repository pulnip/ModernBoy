#pragma once

#include <memory>
#include "fwd.hpp"

namespace ModernBoy{
    class Component{
      protected:
        const Actor& owner;

      public:
        Component(const Actor& actor);
        virtual ~Component();
        virtual void update(float dt);
    };
}
