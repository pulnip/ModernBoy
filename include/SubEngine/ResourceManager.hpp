#pragma once

#include "SubEngine.hpp"

namespace Game{
    namespace SubEngine{
        class ResourceManager: public Interface{
        public:
            ResourceManager() noexcept=default;
            virtual ~ResourceManager()=default;

            void update(const Time& deltaTime) noexcept override final{}

            Type getType() const noexcept override{
                return Type::ResourceManager;
            }
        };
    }
}

namespace Null{
    namespace SubEngine{
        class NullResourceManager final:
            public Game::SubEngine::ResourceManager{};
    }
}
