#include <cassert>

#include "GameEngine/GameEngine.hpp"
#include "ActorManagerForTest.hpp"

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

void ActorManagerForTest::setAttribute() noexcept{
    auto self=std::dynamic_pointer_cast<ActorManager>(shared_from_this());
    assert(self!=nullptr);

#ifdef PING_PONG
    Actor::make<Paddle>(self);
    Actor::make<Ceil>(self);
    Actor::make<Floor>(self);
    Actor::make<RightWall>(self);
    Actor::make<Ball>(self);
#endif

#ifdef SPACE_SHIP
    Actor::make<Ship>(self);
    for(int i=0; i<20; ++i){
        Actor::make<Asteroid>(self);
    }
    Actor::make<CelestialSky>(self);
#endif
}
