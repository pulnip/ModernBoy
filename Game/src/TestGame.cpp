#include "TestGame.hpp"
#include "ActorManagerForTest.hpp"
#include "SubEngine/GameLogic.hpp"

void TestGame::injectDependency() noexcept{
    GameEngineWithSDL::injectDependency();
    auto self=weak_from_this();

    ISubEngine::make<ActorManagerForTest>(self);
    // ISubEngine::make<NullGameLogic>(self);
}
