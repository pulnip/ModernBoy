#pragma once

#include "Blueprint.hpp"
#include "Engine/Core.hpp"
#include "Engine/Logger.hpp"

namespace WithSDL{
    class Core final: public Engine::Core{
      public:
        Core(const ::Blueprint::Window&) noexcept;
        ~Core();

        static void preConstruct() noexcept;
        void postConstruct() noexcept override final;
        static void make(const ::Blueprint::Window& window) noexcept{
            ::Engine::Core::make<Core>(window);
        }

      private:
        ::Logger::Binded logger={"Core", "SDL"};
    };
}