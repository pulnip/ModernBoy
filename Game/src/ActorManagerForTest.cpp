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

using namespace Game;

void ActorManagerForTest::postConstruct() noexcept{
    ActorManager::postConstruct();

    auto self=shared_from_this();

    auto q=query(SubEngine::Type::PhysicsSimulator);

    assert(q.has_value());
    auto ps=std::static_pointer_cast<SubEngine::PhysicsSimulator>(q.value());

#ifdef PING_PONG
    auto paddle=Actor::Interface::make<Paddle>(self);
    auto ceil=Actor::Interface::make<Ceil>(self);
    auto floor=Actor::Interface::make<Floor>(self);
    auto rwall=Actor::Interface::make<RightWall>(self);
    auto ball=Actor::Interface::make<Ball>(self);

    ps->setCollision(ball, paddle);
    ps->setCollision(ball, ceil);
    ps->setCollision(ball, floor);
    ps->setCollision(ball, rwall);

    ps->setCollision(paddle, ceil);
    ps->setCollision(paddle, floor);

#endif

#ifdef SPACE_SHIP
    Actor::Interface::make<Ship>(self);
    for(int i=0; i<20; ++i){
        Actor::Interface::make<Asteroid>(self);
    }
    Actor::Interface::make<CelestialSky>(self);
#endif
}
