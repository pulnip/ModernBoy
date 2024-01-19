#include "TestGame.hpp"
#include "ActorManagerForTest.hpp"

void TestGame::injectDependency() noexcept{
    GameEngineWithSDL::injectDependency();
    auto self=weak_from_this();

    auto actorManager = SubEngine::make<ActorManagerForTest>(self);
}
