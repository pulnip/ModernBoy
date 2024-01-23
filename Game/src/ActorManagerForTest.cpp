#include <cassert>

#include "GameEngine/GameEngine.hpp"
#include "ActorManagerForTest.hpp"
#include "Paddle.hpp"
#include "Wall.hpp"
#include "Ship.hpp"
#include "Asteroid.hpp"

enum class GameName{
    PING_PONG,
    SPACE_SHIP
};

constexpr auto GAME_NAME=GameName::SPACE_SHIP;

void ActorManagerForTest::setAttribute() noexcept{
    auto self=std::dynamic_pointer_cast<ActorManager>(shared_from_this());
    assert(self!=nullptr);

    if(GAME_NAME==GameName::PING_PONG){
        Actor::make<Paddle>(self);
        Actor::make<Ceil>(self);
        Actor::make<Floor>(self);
        Actor::make<RightWall>(self);
    }

    if(GAME_NAME==GameName::SPACE_SHIP){
        Actor::make<Ship>(self);
        for(int i=0; i<20; ++i){
            Actor::make<Asteroid>(self);
        }
    }
}
