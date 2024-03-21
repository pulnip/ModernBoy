#pragma once

#include "Blueprint.hpp"
#include "Engine/Core.hpp"
#include "Engine/Logger.hpp"

namespace WithSDL{
    class Core final: public Engine::Core{
      public:
        static void preConstruct() noexcept;
        Core() noexcept;
        ~Core();

        static void make() noexcept{
            ::Engine::Core::make<Core>();
        }

      private:
        Logging::Bind logger={"Core", "SDL"};
    };
}