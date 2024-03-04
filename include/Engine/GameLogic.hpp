#pragma once

#include <memory>
#include "TinyTraits.hpp"
#include "Observable.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    class GameLogic: public MakableSingleton<GameLogic>,
        public Observer<Game::Status>,
        public UniqueSender, public Connectable
    {
        friend class ::MainEngine;

      public:
        static void preConstruct() noexcept{}
        virtual void postConstruct() noexcept override final{}
        static void make() noexcept{
            MakableSingleton<GameLogic>::make<GameLogic>();
        }
        virtual ~GameLogic()=default;

        void connect() noexcept override final;
        void update(const Game::Time& deltaTime) noexcept;
        
        void set(const Game::Status& st) noexcept{
            onNotify(st);
        }

      private:
        void onNotify(const Game::Status&) noexcept override final;

      private:
        ::Logger::Binded logger={"GameLogic", "base"};
    };
}