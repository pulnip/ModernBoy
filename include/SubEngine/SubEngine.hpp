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
            SoundEngine
        };

        class Interface: public Makable<Interface, Core::Engine>{
        public:
            Interface() noexcept=default;
            virtual ~Interface()=default;

            virtual void update(const Time& deltaTime) noexcept=0;

            virtual Type getType() const noexcept{
                return Type::Interface;
            }
        };
    }
}