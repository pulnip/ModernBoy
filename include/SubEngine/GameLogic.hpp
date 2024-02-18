#pragma once

#include "Observer.hpp"
#include "Observable.hpp"

#include "SubEngine.hpp"

namespace Game{
    enum class Status{
        GAME_OVER,
        FORCE_QUIT,
        UNEXPECTED
    };
    namespace SubEngine{
        class GameLogic: public Interface,
            public Observer<Status>,
            public UniqueSender
        {
        public:
            GameLogic() noexcept=default;
            virtual ~GameLogic()=default;

        private:
            void onNotify(const Status& status) noexcept override final;

            Type getType() const noexcept override final{
                return Type::GameLogic;
            }
        };
    }
}

namespace Null{
    namespace SubEngine{
        class GameLogic: public Game::SubEngine::GameLogic{
        private:
            void update(const Game::Time& deltaTime
            ) noexcept override final{}
        };
    }
}
