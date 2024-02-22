#pragma once

#include <map>

#include "TinyTraits.hpp"
#include "Observable.hpp"
#include "myfwd.hpp"

namespace Engine{
    class InputSystem: public Singleton<InputSystem>,
        public Observable<Game::Status>, Connectable
    {
        friend class Core;
      public:
        InputSystem() noexcept;
        virtual ~InputSystem();

        void connect() noexcept override final;
        virtual void update(const Game::Time& deltaTime) noexcept=0;
        void registerKey(
            const uint8_t key,
            const std::weak_ptr<Observer<Skin::Key>> subscriber
        ) noexcept;
        virtual uint8_t escapeKeycode() noexcept{ return 0x1b; }

      protected:
        std::map<uint8_t, Observable<Skin::Key>> keyMap;

      private:
        std::unique_ptr<BindedLogger> logger;
    };
}

namespace WithSTD{
    class InputSystem final: public Engine::InputSystem{
      public:
        InputSystem() noexcept;
        ~InputSystem();

        void update(const Game::Time& deltaTime) noexcept override final;
        uint8_t escapeKeycode() noexcept override final{
            return 'q';
        }

      private:
        std::unique_ptr<Engine::BindedLogger> logger;
    };
}