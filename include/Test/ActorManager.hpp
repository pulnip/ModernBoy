#pragma once

#include "Engine/ActorManager.hpp"
#include "Engine/Logger.hpp"

namespace Test{
    class ActorManager: public Engine::ActorManager{
      public:
        static void make() noexcept{
            ::Engine::ActorManager::make<ActorManager>();
        }
        void test() noexcept;
      private:
        ::Logger::Binded logger={"ActorManager", "Test"};
    };
}
