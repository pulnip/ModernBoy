#include <cassert>
#include <cmath>
#include "Math.hpp"
#include "Engine/Core.hpp"
#include "Engine/PhysicsSimulator.hpp"
#include "Engine/Logger.hpp"
#include "Component/Movable.hpp"

using namespace WithModel2D;

PhysicsSimulator::Model::Model(const sp& movable) noexcept{
    this->position=movable->get().position.linear;
    this->velocity=movable->get().velocity.linear;
    this->size=movable->get().volume.size();
}

PhysicsSimulator::PhysicsSimulator() noexcept:
    logger(std::make_unique<Engine::BindedLogger>("PhysicsSimulator", "Model2D"))
{
    logger->debug("constructed");
}

PhysicsSimulator::~PhysicsSimulator(){
    logger->debug("destructed");
}

PhysicsSimulator::AxisModel::AxisModel(const Model& m, Axis axis) noexcept{
    switch(axis){
    case Axis::x:
        position=m.position.x;
        velocity=m.velocity.x;
        size=m.size.x / 2;
        break;
    case Axis::y:
        position=m.position.y;
        velocity=m.velocity.y;
        size=m.size.y / 2;
    }
}

void PhysicsSimulator::redoUpdateIfCollide(
    sp who, sp to, const Game::Time& dt
) noexcept{
    Model target=who;
    Model opponent=to;

    // if(isGetCloser(target, opponent)){
    auto result=collide(target, opponent);
    auto& [c_time, c2d]=result;


    // if collide in deltaTime
    if(0 <= c_time and c_time <= dt){
        applyCollisionResult(
            who->get(), result, dt,
                opponent.velocity
        );
    }
}

PhysicsSimulator::CollisionTime
PhysicsSimulator::axisCollisionTime(
    AxisModel target, AxisModel opponent
) noexcept{
    using namespace My::Math;

    const auto rpos = target.position - opponent.position;
    const auto rvel = target.velocity - opponent.velocity;

    const auto size0 = target.size + opponent.size;
    const auto pos0 = std::abs(rpos);
    const auto vel0 = rpos==pos0 ? rvel : -rvel;

    double start=Number::infinity, end=Number::infinity;

    if(not Number::NearZero(vel0)){
        start = (pos0 - size0) / -vel0;
        end = (pos0 + size0) / -vel0;
    } else if(pos0 <= size0){
        start = -Number::infinity;
    }

    return {start, end};
}

PhysicsSimulator::CollisionTime
PhysicsSimulator::commonCollisionTime(
    const CollisionTime& x, const CollisionTime& y
) noexcept{
    // x.first <= x.second이도록 변형.
    const auto xmin = std::min(x.first, x.second);
    const CollisionTime x0 = {xmin, (x.first+x.second)-xmin};
    const auto ymin = std::min(y.first, y.second);
    const CollisionTime y0 = {ymin, (y.first+y.second)-ymin};

    // x0과 y0의 공통 구간 (min, max)를 찾음.
    const auto min=std::max(x0.first, y0.first);
    const auto max=std::min(x0.second, y0.second);

    return {min, max};
}

bool PhysicsSimulator::isCollidable(const CollisionTime& t) noexcept{
    // ctime_min > ctime_max라면, 충돌하는 경우가 애초에 존재하지 않다는 뜻.
    // => 충돌이 가능한 시점 t가 존재하지 않음.
    // ctime_max < 0이라면, 충돌이 가능하지만 이미 pass-by한 상태.
    // => 충돌이 가능한 시점 t < 0.
    return t.first <= t.second and 0 <= t.second;
}

PhysicsSimulator::Result PhysicsSimulator::collide(
    const Model& colliding, const Model& collided
) noexcept{
    using Axis=AxisModel::Axis;
    const auto xtime=axisCollisionTime(
        {colliding, Axis::x}, {collided, Axis::x}
    );
    const auto ytime=axisCollisionTime(
        {colliding, Axis::y}, {collided, Axis::y}
    );

    const auto commonTime=commonCollisionTime(xtime, ytime);


    if(isCollidable(commonTime)){
        // 0 이상의 충돌시점 t.
        return {std::max(0.0, commonTime.first), {
            commonTime.first==xtime.first,
            commonTime.first==ytime.first
        }};
    }
    return {My::Math::Number::infinity, {false, false}};
}

// #define TOTALLY_INELASTIC_COLLISION

void PhysicsSimulator::applyCollisionResult(
    Skin::Attribute_2D& target,
    const Result& result, const Game::Time& totalTime,
    const Game::Vector2D& opponentVelocity 
) noexcept{
    using namespace My::Math;

    const auto& [c_time, c_axis]=result;
    // undo
    target.undo_update(totalTime);
    // redo until collide
    target.update(c_time);


    // flip velocity
    auto& v=target.velocity.linear;
    if(c_axis.first){
#ifdef TOTALLY_INELASTIC_COLLISION
        v.x = opponentVelocity.x;
#else
        v.x = reflect(v.x, opponentVelocity.x);
#endif
    }

    if(c_axis.second){
#ifdef TOTALLY_INELASTIC_COLLISION
        v.y = opponentVelocity.y;
#else
        v.y = reflect(v.y, opponentVelocity.y);
#endif
    }

    // redo after collide
    target.update(totalTime - result.first);
}