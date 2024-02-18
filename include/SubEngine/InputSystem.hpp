#pragma once

#include <cstdint>
#include <map>

#include "Observer.hpp"
#include "Observable.hpp"
#include "SubEngine.hpp"

namespace Skin{
    struct Key {
        enum class Status {
            PRESSED,
            RELEASED
        } status;
        uint8_t key;
    };
}

namespace Game{
    namespace SubEngine{
        class InputSystem: public Interface,
            public Observable<Game::Status>
        {
        public:
            InputSystem() noexcept=default;
            virtual ~InputSystem()=default;

            virtual void update(const Time& deltaTime
            ) noexcept override=0;
            SubEngine::Type getType() const noexcept override final{
                return SubEngine::Type::InputSystem;
            }
            void registerKey(
                const uint8_t key,
                const std::weak_ptr<Observer<Skin::Key>> subscriber
            ) noexcept;

        private:
            void postConstruct() noexcept override final;
            virtual uint8_t escapeKeycode() const noexcept{ return 0x1b; }

        protected:
            std::map<uint8_t, Observable<Skin::Key>> keyMap;
        };
    }
}

namespace Null{
    namespace SubEngine{
        class NullInputSystem final: public Game::SubEngine::InputSystem{
        private:
            void update(const Game::Time&) noexcept override final{}
        };
    }
}

namespace WithSTD{
    namespace SubEngine{
        class InputSystem_default final:
            public Game::SubEngine::InputSystem
        {
        private:
            void update(const Game::Time& deltaTime
            ) noexcept override final;

            uint8_t escapeKeycode() const noexcept override final{
                return 'q';
            }
        };
    }
}
