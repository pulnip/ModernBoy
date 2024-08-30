#pragma once

#include "Attributes.hpp"
#include "Component.hpp"
#include "fwd.hpp"

namespace ModernBoy{
    class LightComponent: public Component{

      public:
        Light light;

        LightComponent(Actor& actor);
    };
}
