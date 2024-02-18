#include "MainEngine.hpp"
#include "TestGame.hpp"
#include "ActorManagerForTest.hpp"
#include "SubEngine/GameLogic.hpp"
#include "SubEngine/PhysicsSimulator.hpp"
#include "SubEngine/Logger.hpp"

TestGame::TestGame(
    std::shared_ptr<Game::SubEngine::Logger>& logger
) noexcept: Engine(logger){
    logger->log("GameEngine", "TestGame", "Generated.");
}

TestGame::~TestGame(){
    logger->log("GameEngine", "TestGame", "Destructed");
}

void TestGame::postConstruct() noexcept{
    Engine::postConstruct();

    auto self=owner.lock()->getEngine();

    Game::SubEngine::Interface::make<
        Game::SubEngine::PhysicsSimulator
    >(self);
    // ActorManager should call after PhysicsSimulator
    Game::SubEngine::Interface::make<
        ActorManagerForTest
    >(self);
    // ISubEngine::make<NullGameLogic>(self);
}
