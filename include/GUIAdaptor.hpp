#pragma once

#include <SDL2/SDL_video.h>
#include <wrl/client.h>
#include <d3d11.h>
#include "Attributes.hpp"

namespace ModernBoy{
    class GUIAdaptor{
        template<class T> using ComPtr=Microsoft::WRL::ComPtr<T>;

      public:
        float deltaTime=0.0f;

        bool drawAsWire=false;
        float fov=70.0f;
        float normalScale=0.0f;
        size_t lightType=0;

      public:
        GUIAdaptor(SDL_Window* window,
            const ComPtr<ID3D11Device>& device,
            const ComPtr<ID3D11DeviceContext>& context,
            const ipoint2& screenSize
        );
        ~GUIAdaptor();

        void update();
        void render();
    };
}