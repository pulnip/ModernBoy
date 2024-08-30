#pragma once

#include <memory>
#include "fwd.hpp"

namespace ModernBoy{
    class Core{
        std::unique_ptr<Window> window;
        std::unique_ptr<ActorManager> actors;
        std::unique_ptr<InputSystem> inputSystem;

        bool isRunning=false;

      public:
        Core();
        ~Core();

        void run();
    };
}
