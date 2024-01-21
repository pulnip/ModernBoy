#include <cassert>

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

void ActorManagerForTest::injectDependency() noexcept{
    auto self=std::dynamic_pointer_cast<ActorManager>(shared_from_this());
    assert(self!=nullptr);

    if(GAME_NAME==GameName::PING_PONG){
        IActor::make<Paddle>(self);
        IActor::make<Ceil>(self);
        IActor::make<Floor>(self);
        IActor::make<RightWall>(self);
    }

    if(GAME_NAME==GameName::SPACE_SHIP){
        IActor::make<Ship>(self);
        for(int i=0; i<1; ++i){
            IActor::make<Asteroid>(self);
        }
    }
}
