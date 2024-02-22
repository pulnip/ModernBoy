#pragma once

#include <memory>
#include "TinyTraits.hpp"
#include "Observable.hpp"
#include "myfwd.hpp"

namespace Engine{
    class GameLogic: public Singleton<GameLogic>,
        public Observer<Game::Status>,
        public UniqueSender, public Connectable
    {
        friend class ::MainEngine;

      public:
        GameLogic() noexcept;
        virtual ~GameLogic();

        void connect() noexcept override final;
        void update(const Game::Time& deltaTime) noexcept;

      private:
        void onNotify(const Game::Status&) noexcept override final;

      private:
        std::unique_ptr<BindedLogger> logger;
    };
}