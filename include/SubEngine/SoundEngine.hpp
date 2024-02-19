#pragma once

#include "SubEngine.hpp"

namespace Game{
    namespace SubEngine{
        class SoundEngine: public Interface,
            public std::enable_shared_from_this<SoundEngine>
        {
          public:
            SoundEngine() noexcept=default;
            virtual ~SoundEngine()=default;

            SubEngine::Type getType() const noexcept override{
                return SubEngine::Type::SoundEngine;
            }
          protected:
            void postConstruct() noexcept override;
        };
    }
}

namespace Null{
    namespace SubEngine{
        class SoundEngine: public Game::SubEngine::SoundEngine{
        private:
            void update(const Game::Time& deltaTime
            ) noexcept override final{}
        };
    }
}
