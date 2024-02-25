#pragma once

#include <list>
#include <map>
#include <memory>
#include <utility>
#include "Math.hpp"
#include "TinyTraits.hpp"
#include "myfwd.hpp"

namespace Engine{
    class PhysicsSimulator: public Singleton<PhysicsSimulator>{
        friend class ::MainEngine;
      protected:
        using sp=std::shared_ptr<Component::Movable>;
        using wp=std::weak_ptr<Component::Movable>;

      public:
        PhysicsSimulator() noexcept;
        virtual ~PhysicsSimulator();

        void update(const Game::Time& deltaTime) noexcept;

        void append(pActor) noexcept;
        void setCollision(pActor who, pActor to) noexcept;

      private:
        virtual void redoUpdateIfCollide(
            sp who, sp to, const Game::Time& dt
        ) noexcept=0;

      private:
        std::unique_ptr<BindedLogger> logger;
        std::map<wp, std::list<wp>,
            std::owner_less<wp>
        > collisionMap;
    };
}

namespace WithModel2D{
    class PhysicsSimulator final:
        public Engine::PhysicsSimulator
    {
        struct Model{
            Model(const sp&) noexcept;
            Game::Vector2D position, velocity, size;
        };
      public:
        PhysicsSimulator() noexcept;
        ~PhysicsSimulator();

      private:
        void redoUpdateIfCollide(
            sp who, sp to, const Game::Time& dt
        ) noexcept override final;

        // check only velocity
        bool isGetCloser(const Model&, const Model&) noexcept;
        // first value: x-axis collision
        // second value: y-axis collision        
        using Collision2D=std::pair<bool, bool>;
        // first value: estimated collision time(seconds)
        // second value: valid if collide
        using Result=std::pair<Game::Time, Collision2D>;
        Result collide(const Model&, const Model&) noexcept;
        void applyCollisionResult(
            ::Skin::Attribute_2D& target,
            const Result&, const Game::Time& totalTime,
            const Game::Vector2D& opponentVelocity
        ) noexcept;

      private:
        std::unique_ptr<Engine::BindedLogger> logger;
    };
}