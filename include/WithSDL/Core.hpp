#pragma once

#include "Blueprint.hpp"
#include "Engine/Core.hpp"
#include "Engine/Logger.hpp"

namespace WithSDL{
    class Core final: public Engine::Core{
      public:
        Core(const ::Blueprint::Window&) noexcept;
        ~Core();
    
      private:
        ::Logger::Binded logger={"Core", "SDL"};
    };
}