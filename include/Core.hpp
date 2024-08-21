#pragma once

#include <memory>
#include "fwd.hpp"

namespace ModernBoy{
    class Core{
        std::unique_ptr<Window> window;
        std::unique_ptr<ActorManager> actors;

        bool isRunning=false;

      public:
        Core();
        ~Core();

        void run();
    };
}
