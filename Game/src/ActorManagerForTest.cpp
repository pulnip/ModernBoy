#include <cassert>

#include "GameEngine/GameEngine.hpp"
#include "ActorManagerForTest.hpp"
#include "SubEngine/PhysicsSimulator.hpp"

#define PING_PONG

#ifdef PING_PONG
#include "Ball.hpp"
#include "Paddle.hpp"
#include "Wall.hpp"
#endif

#ifdef SPACE_SHIP
#include "Asteroid.hpp"
#include "CelestialSky.hpp"
#include "Ship.hpp"
#endif

void ActorManagerForTest::postConstruct() noexcept{
    auto self=std::dynamic_pointer_cast<ActorManager>(shared_from_this());
    assert(self!=nullptr);

    auto q=query(SubEngine::Type::PhysicsSimulator);

    assert(q.has_value());
    auto ps=std::static_pointer_cast<PhysicsSimulator>(q.value());

#ifdef PING_PONG
    auto paddle=Actor::make<Paddle>(self);
    auto ceil=Actor::make<Ceil>(self);
    auto floor=Actor::make<Floor>(self);
    auto rwall=Actor::make<RightWall>(self);
    auto ball=Actor::make<Ball>(self);

    ps->setCollision(ball, paddle);
    ps->setCollision(ball, ceil);
    ps->setCollision(ball, floor);
    ps->setCollision(ball, rwall);

    ps->setCollision(paddle, ceil);
    ps->setCollision(paddle, floor);

#endif

#ifdef SPACE_SHIP
    Actor::make<Ship>(self);
    for(int i=0; i<20; ++i){
        Actor::make<Asteroid>(self);
    }
    Actor::make<CelestialSky>(self);
#endif
}
