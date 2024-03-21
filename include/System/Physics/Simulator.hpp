#pragma once

#include <map>
#include <memory>
#include "Engine/Types.hpp"
#include "System/Logging/Bind.hpp"
#include "System/Physics/Types.hpp"
#include "FWD.hpp"

class Simulator{
    using Time=Game::Time;

  public:
    Simulator() noexcept;
    virtual ~Simulator()=default;

    void update(const Time& deltaTime) noexcept;

    bool updateTarget(wp, const Actor::Role&, const Game::Time&) noexcept;
    void updateTarget_aux(sp, const std::vector<wp>&, const Game::Time&) noexcept;

    void append(pActor) noexcept;

  private:
    virtual void redoUpdateIfCollide(
        sp who, sp to, const Game::Time& dt
    ) noexcept=0;

  public:
    static std::shared_ptr<Simulator> null;

  private:
    Logging::Bind logger={"Physics", "base"};
    using ID=int;
    std::map<ID, >;
};

namespace WithModel2D{
    class Simulator final:
        public Engine::Simulator
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
            ::Engine::Simulator::make<Simulator>();
        }
        ~Simulator()=default;

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
        Logging::Bind logger={"Physics", "Model2D"};
    };
}