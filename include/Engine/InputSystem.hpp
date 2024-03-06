#pragma once

#include <map>

#include "TinyTraits.hpp"
#include "Observable.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    class InputSystem: public MakableSingleton<InputSystem>,
        public Observable<Game::Status>, Connectable
    {
        friend class Core;
      public:
        static void preConstruct() noexcept{}
        void postConstruct() noexcept override final{}
        virtual ~InputSystem()=default;

        void connect() noexcept override final;
        virtual void update(const Game::Time& deltaTime) noexcept=0;
        void registerKey(
            const uint8_t key,
            const std::weak_ptr<Observer<::Skin::Key>> subscriber
        ) noexcept;
        virtual uint8_t escapeKeycode() noexcept{ return 0x1b; }

      protected:
        std::map<uint8_t, Observable<::Skin::Key>> keyMap;

      private:
        ::Logger::Binded logger={"Input", "base"};
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
        ::Logger::Binded logger={"Input", "STD"};
    };
}