#pragma once

#include "Engine/Graphics.hpp"

#include <SDL2/SDL_render.h>
#include "WithSDL/Skin.hpp"

namespace WithSDL{
    class Graphics final: public Engine::Graphics{
        friend class Core;
      public:
        Graphics() noexcept;
        ~Graphics();

        void draw(WithSDL::Skin::Flyweight::Sprite) noexcept;
        void* context() noexcept override final;

      private:
        void initialize() noexcept override final;
        void destroyAll() noexcept override final;

        void draw(::Skin::Flyweight::ColorRect) noexcept override final;

        void clearScreen() noexcept override final;
        void swapBuffer() noexcept override final;
    
      private:
        std::unique_ptr<Engine::BindedLogger> logger;
        SDL_Window* window=nullptr;
        SDL_Renderer* renderer=nullptr;
    };
}