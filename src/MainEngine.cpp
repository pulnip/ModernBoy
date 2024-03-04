#include "MainEngine.hpp"
#include "Engine/GameLogic.hpp"
#include "Engine/Graphics.hpp"
#include "Engine/LevelLoader.hpp"
#include "Engine/PhysicsSimulator.hpp"
#include "WithSDL/Core.hpp"
#include "WithSDL/Logger.hpp"
#include "Test/ActorManager.hpp"

using namespace Engine;

MainEngine::MainEngine() noexcept{
    WithSTD::Logger::make();
    Logger::Impl::get()->currlevel=Logger::Level::DEBUG;

    WithRapidjson::LevelLoader::make();

    auto setting=LevelLoader::get()->loadLevel(
        "Level0.json").value_or(Blueprint::Setting{}
    );

    WithSDL::Core::make(setting.window);

    GameLogic::make();
    WithModel2D::PhysicsSimulator::make();
    Test::ActorManager::make();

    std::static_pointer_cast<Test::ActorManager>(ActorManager::get())->test();
}

MainEngine::~MainEngine(){
    // ActorManager::base=nullptr;
    // PhysicsSimulator::base=nullptr;
    // GameLogic::base=nullptr;
    // Core::base=nullptr;
    // Logger::Impl::base=nullptr;
}

void MainEngine::start() noexcept{
    Core::get()->run();
}