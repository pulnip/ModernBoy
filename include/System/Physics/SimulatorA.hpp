#pragma once

#include <map>
#include "System/Physics/Simulator.hpp"
#include "System/Physics/Types.hpp"

class SimulatorA: public Simulator{
    using Object=Physics::ObjectA2D;

    ~SimulatorA()=default;

    void update(const Time&) override;

    void request(ActorID, const Object&);
    void remove(ActorID id) noexcept;

    // helper

    // first value: x-axis collision
    // second value: y-axis collision        
    using Collision2D=std::pair<bool, bool>;
    // first value: estimated collision time(seconds)
    // second value: valid if collide
    using Result=std::pair<Game::Time, Collision2D>;

    static Result collide(const Object&, const Object&);
    static void applyResult(ActorID id, const Object& result);

    // convert Movable to ObjectA2D
    static Object convert(const Movable&) noexcept;

    // collision check in one dimension
    struct AxisModel{
        enum class Axis{x, y};
        Physics::Unit position, velocity, size;
    };

    static AxisModel downscale(Object&) noexcept;












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
    std::map<ActorID, Physics::ObjectA2D> objectMap;
};