#ifndef MODERNBOY_RENDER_GUI_HPP
#define MODERNBOY_RENDER_GUI_HPP

#include <cstdint>

namespace ModernBoy{
    struct UI{
        bool drawAsWire=false;
        float fov=100.0f;
        float normalScale = 0.0f;
        int lightType=0;
        float rimPower=1.0f;
        float rimStrength=0.0f;

        void update();
    };
}

#endif // MODERNBOY_RENDER_GUI_HPP