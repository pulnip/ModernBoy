#pragma once

#include "gefwd.hpp"
#include "SubEngine/GraphicsEngine.hpp"

using namespace Skin::Flyweight;
using namespace WithSDL::Skin::Flyweight;

namespace WithSDL{
    namespace SubEngine{
        class GraphicsEngine final: public Game::SubEngine::GraphicsEngine{
        public:
            GraphicsEngine() noexcept=default;
            ~GraphicsEngine();

            void draw(const ColorRect& rect) noexcept override final;
            void draw(const SpriteForSDL& sprite) noexcept;

            class SDL_Renderer* getContext() noexcept{
                return renderer;
            }

        private:
            void postConstruct() noexcept override final;
            void prepareRendering() noexcept override final{
                initBackground();
            }
            void finalizeRendering() noexcept override final{   
                changeColorBuffer();
            }

            void initBackground() noexcept;
            void changeColorBuffer() noexcept;

        private:
            // SDL이 생성한 윈도우
            class SDL_Window* window;
            class SDL_Renderer* renderer;
        };
    }
}
