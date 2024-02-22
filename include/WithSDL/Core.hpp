#pragma once

#include "Engine/Core.hpp"

namespace WithSDL{
    class Core final: public Engine::Core{
      public:
        Core() noexcept;
        ~Core();
    
      private:
        std::unique_ptr<Engine::BindedLogger> logger;
    };
}