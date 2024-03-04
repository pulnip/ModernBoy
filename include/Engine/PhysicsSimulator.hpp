#pragma once

#include <list>
#include <map>
#include <memory>
#include <utility>
#include "Math.hpp"
#include "TinyTraits.hpp"
#include "myfwd.hpp"
#include "Engine/Logger.hpp"

namespace Engine{
    class PhysicsSimulator:
        public MakableSingleton<PhysicsSimulator>
    {
        friend class ::MainEngine;
      protected:
        using sp=std::shared_ptr<Component::Movable>;
        using wp=std::weak_ptr<Component::Movable>;

      public:
        static void preConstruct() noexcept{}
        virtual void postConstruct() noexcept override final{}
        virtual ~PhysicsSimulator()=default;

        void update(const Game::Time& deltaTime) noexcept;

        void append(pActor) noexcept;
        void setCollision(pActor who, pActor to) noexcept;

      private:
        virtual void redoUpdateIfCollide(
            sp who, sp to, const Game::Time& dt
        ) noexcept=0;

      private:
        Logger::Binded logger={"Physics", "base"};
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
        struct AxisModel{
            enum class Axis{x, y};
            AxisModel(const Model&, Axis) noexcept;
            double position, velocity, size;
        };
      public:
        static void make() noexcept{
            ::Engine::PhysicsSimulator::make<PhysicsSimulator>();
        }
        ~PhysicsSimulator()=default;

      private:
        void redoUpdateIfCollide(
            sp who, sp to, const Game::Time& dt
        ) noexcept override final;

        // first: collision start time
        // second: collision end time
        using CollisionTime=std::pair<Game::Time, Game::Time>;
        CollisionTime axisCollisionTime(
            AxisModel, AxisModel
        ) noexcept;
        CollisionTime commonCollisionTime(
            const CollisionTime&, const CollisionTime&
        ) noexcept;
        bool isCollidable(const CollisionTime&) noexcept;
        // first value: x-axis collision
        // second value: y-axis collision        
        using Collision2D=std::pair<bool, bool>;
        // first value: estimated collision time(seconds)
        // second value: valid if collide
        using Result=std::pair<Game::Time, Collision2D>;
        bool isCloseEnough(
            const Model&, const Model&, const Game::Time&
        ) noexcept;
        Result collide(const Model&, const Model&) noexcept;
        void applyCollisionResult(
            ::Skin::Attribute_2D& target,
            const Result&, const Game::Time& totalTime,
            const Game::Vector2D& opponentVelocity
        ) noexcept;

      private:
        ::Logger::Binded logger={"Physics", "Model2D"};
    };
}