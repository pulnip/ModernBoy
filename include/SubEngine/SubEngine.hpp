#pragma once

#include "Makable.hpp"

#include "gefwd.hpp"

namespace Game{
    namespace SubEngine{
        enum class Type{
            Interface,
            ActorManager,
            GameLogic,
            GraphicsEngine,
            InputSystem,
            Logger,
            PhysicsSimulator,
            ResourceManager,
            SoundEngine,
            Timer
        };

        class Interface: public Makable<Interface, Core::Engine>{
        public:
            Interface() noexcept=default;
            virtual ~Interface()=default;

        private:
            virtual Type getType() const noexcept{
                return Type::Interface;
            }
            virtual void update(const Time& deltaTime) noexcept=0;
        };
    }
}