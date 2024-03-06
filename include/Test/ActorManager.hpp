#pragma once

#include "Engine/ActorManager.hpp"
#include "Engine/Logger.hpp"

namespace Test{
    class ActorManager: public Engine::ActorManager{
      public:
        template<typename... T>
        static void make(T... args) noexcept{
            ::Engine::ActorManager::make<ActorManager>(args...);
        }
        void test() noexcept;
      private:
        ::Logger::Binded logger={"ActorManager", "Test"};
    };
}
