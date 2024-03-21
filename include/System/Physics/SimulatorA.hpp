#pragma once

#include <map>
#include "System/Physics/Simulator.hpp"
#include "System/Physics/Types.hpp"

class SimulatorA: public Simulator{
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
    using ActorID=int;
    std::map<ActorID, Physics::ObjectA2D> objectMap;
};