#include "TestGame.hpp"
#include "ActorManagerForTest.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/PhysicsSimulator.hpp"

void TestGame::setAttribute() noexcept{
    auto self=weak_from_this();

    SubEngine::make<PhysicsSimulator>(self);
    // ActorManager should call after PhysicsSimulator
    SubEngine::make<ActorManagerForTest>(self);
    // ISubEngine::make<NullGameLogic>(self);
}
