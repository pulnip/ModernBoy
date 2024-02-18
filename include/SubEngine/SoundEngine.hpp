#pragma once

#include "SubEngine.hpp"

namespace Game{
    namespace SubEngine{
        class SoundEngine: public Interface{
        public:
            SoundEngine() noexcept=default;
            virtual ~SoundEngine()=default;

            SubEngine::Type getType() const noexcept override{
                return SubEngine::Type::SoundEngine;
            }
        };
    }
}

namespace Null{
    namespace SubEngine{
        class NullSoundEngine:
            public Game::SubEngine::SoundEngine
        {
        private:
            void update(const Game::Time& deltaTime
            ) noexcept override final{}
        };
    }
}
