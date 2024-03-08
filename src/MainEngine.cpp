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

    GameLogic::make();
    WithModel2D::PhysicsSimulator::make();

    Test::ActorManager::make();
    WithRapidjson::LevelLoader::make();


    WithSDL::Core::make();

    auto setting=LevelLoader::get()->loadLevel(
        "Level0.json").value_or(Blueprint::Setting{}
    );

    Graphics::get()->initialize(setting.window);

    // std::static_pointer_cast<Test::ActorManager>(ActorManager::get())->test();
}

void MainEngine::start() noexcept{
    Core::get()->run();
}