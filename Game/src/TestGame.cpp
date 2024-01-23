#include "TestGame.hpp"
#include "ActorManagerForTest.hpp"
#include "SubEngine/GameLogic.hpp"

void TestGame::setAttribute() noexcept{
    auto self=weak_from_this();

    SubEngine::make<ActorManagerForTest>(self);
    // ISubEngine::make<NullGameLogic>(self);
}
