#pragma once

#include <SDL2/SDL_video.h>
#include "Attributes.hpp"
#include "fwd.hpp"

namespace ModernBoy{
    class GUIAdaptor{
      public:
        float deltaTime=0.0f;

        bool drawAsWire=false;
        float fov=70.0f;
        float normalScale=0.0f;
        size_t lightType=0;
        float rimStrength=0.0f;
        float rimPower=1.0f;

      public:
        GUIAdaptor(SDL_Window* window,
            const ComPtr<Device>& device,
            const ComPtr<Context>& context,
            const ipoint2& screenSize
        );
        ~GUIAdaptor();

        void update();
        void render();
    };
}
