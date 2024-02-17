#include "TestGame.hpp"
#include "ActorManagerForTest.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/PhysicsSimulator.hpp"
#include "Logger.hpp"

TestGame::TestGame(
    std::shared_ptr<Game::Plugin::Logger>& logger
) noexcept: GameEngineWithSDL(logger){
    logger->log("GameEngine", "TestGame", "Generated.");
}

TestGame::~TestGame(){
    logger->log("GameEngine", "TestGame", "Destructed");
}

void TestGame::setAttribute() noexcept{
    auto self=weak_from_this();

    SubEngine::make<PhysicsSimulator>(self);
    // ActorManager should call after PhysicsSimulator
    SubEngine::make<ActorManagerForTest>(self);
    // ISubEngine::make<NullGameLogic>(self);
}
