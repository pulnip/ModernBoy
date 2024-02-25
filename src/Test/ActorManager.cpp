#include <cassert>

#include "Engine/Logger.hpp"
#include "Test/ActorManager.hpp"
#include "Engine/PhysicsSimulator.hpp"

#define PING_PONG

#ifdef PING_PONG
#include "Test/Ball.hpp"
#include "Test/Paddle.hpp"
#include "Test/Wall.hpp"
#endif

#ifdef SPACE_SHIP
#include "Test/Asteroid.hpp"
#include "Test/CelestialSky.hpp"
#include "Test/Ship.hpp"
#endif

using namespace Test;

ActorManager::ActorManager() noexcept:
    logger(std::make_unique<Engine::BindedLogger>("ActorManager", "Test"))
{
    logger->debug("constructed");
}

ActorManager::~ActorManager(){
    logger->debug("destructed");
}

void ActorManager::test() noexcept{
#ifdef PING_PONG
    auto paddle=std::make_shared<Test::Paddle>();
    paddle->initAbility();
    appendActor(paddle);
    auto ceil=std::make_shared<Test::Ceil>();
    ceil->initAbility();
    appendActor(ceil);
    auto floor=std::make_shared<Test::Floor>();
    floor->initAbility();
    appendActor(floor);
    auto rwall=std::make_shared<Test::RightWall>();
    rwall->initAbility();
    appendActor(rwall);
    auto ball=std::make_shared<Test::Ball>();
    ball->initAbility();
    appendActor(ball);

    using namespace WithModel2D;
    PhysicsSimulator::get()->setCollision(ball, paddle);
    PhysicsSimulator::get()->setCollision(ball, ceil);
    PhysicsSimulator::get()->setCollision(ball, floor);
    PhysicsSimulator::get()->setCollision(ball, rwall);

    PhysicsSimulator::get()->setCollision(paddle, ceil);
    PhysicsSimulator::get()->setCollision(paddle, floor);

#endif

#ifdef SPACE_SHIP
    auto ship=std::make_shared<Test::Ship>();
    ship->initAbility();
    appendActor(ship);

    for(int i=0; i<20; ++i){
        auto asteroid=std::make_shared<Test::Asteroid>();
        asteroid->initAbility();
        appendActor(asteroid);
    }
    auto sky=std::make_shared<CelestialSky>();
    sky->initAbility();
    appendActor(sky);
#endif
}
