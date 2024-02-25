#pragma once

#include "Engine/ActorManager.hpp"

namespace Test{
    class ActorManager: public Engine::ActorManager{
      public:
        ActorManager() noexcept;
        ~ActorManager();

        void test() noexcept;
      private:
        std::unique_ptr<Engine::BindedLogger> logger;
    };
}
