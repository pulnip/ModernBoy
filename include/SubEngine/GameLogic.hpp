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
            public UniqueSender,
            std::enable_shared_from_this<GameLogic>
        {
        public:
            GameLogic() noexcept=default;
            virtual ~GameLogic()=default;

        protected:
            void postConstruct() noexcept override;
        
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
        class GameLogic final: public Game::SubEngine::GameLogic{
        public:
            GameLogic() noexcept=default;
            ~GameLogic()=default;

        protected:
            void postConstruct() noexcept override final{
                Game::SubEngine::GameLogic::postConstruct();
            }
        
        private:
            void update(const Game::Time& deltaTime
            ) noexcept override final{}
        };
    }
}
