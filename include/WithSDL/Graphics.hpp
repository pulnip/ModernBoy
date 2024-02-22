#pragma once

#include "Engine/Graphics.hpp"

#include <SDL2/SDL_render.h>
#include "WithSDL/Skin.hpp"

using namespace WithSDL::Skin::Flyweight;

namespace WithSDL{
    class Graphics final: public Engine::Graphics{
        friend class Core;
      public:
        Graphics() noexcept;
        ~Graphics();

        void draw(Sprite) noexcept;

      private:
        void initialize() noexcept override final;
        void destroyAll() noexcept override final;
        void* context() noexcept override final;

        void draw(ColorRect) noexcept override final;

        void clearScreen() noexcept override final;
        void swapBuffer() noexcept override final;
    
      private:
        std::unique_ptr<Engine::BindedLogger> logger;
        SDL_Window* window=nullptr;
        SDL_Renderer* renderer=nullptr;
    };
}