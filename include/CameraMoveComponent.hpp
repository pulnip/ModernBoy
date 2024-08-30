#pragma once

#include "InputComponent.hpp"

namespace ModernBoy{
    class CameraMoveComponent: public InputComponent{
        float xVel=0.0f, yVel=0.0f, zVel=0.0f;
        [[maybe_unused]] float yRot=0.0f;

      public:
        CameraMoveComponent(Actor& actor);
        ~CameraMoveComponent()=default;

        void onPressed(const InputState& state) override;
        void onReleased(const InputState& state) override;
        void update(float dt) override;
    };
}
