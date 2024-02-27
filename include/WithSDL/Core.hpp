#pragma once

#include "Engine/Core.hpp"
#include "Engine/Logger.hpp"

namespace WithSDL{
    class Core final: public Engine::Core{
      public:
        Core() noexcept;
        ~Core();
    
      private:
        Logger::Binded logger={"Core", "SDL"};
    };
}