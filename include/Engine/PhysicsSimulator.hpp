#pragma once

#include <memory>
#include "TinyTraits.hpp"
#include "myfwd.hpp"

namespace Engine{
    class PhysicsSimulator: public Singleton<PhysicsSimulator>{
        friend class MainEngine;
      public:
        void update(const Game::Time& deltaTime) noexcept;
    };
}