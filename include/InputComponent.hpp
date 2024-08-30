#pragma once

#include "Component.hpp"
#include "fwd.hpp"

namespace ModernBoy{
    class InputComponent: public Component{
      public:
        InputComponent(Actor& actor)
        : Component(actor){}
        ~InputComponent()=default;

        virtual void onPressed(const InputState& state)=0;
        virtual void onReleased(const InputState& state)=0;
    };
}
