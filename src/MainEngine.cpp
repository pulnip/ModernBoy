#include <SDL2/SDL.h>
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
    SDL_Init(SDL_INIT_EVERYTHING);
    Logger::Impl::base=std::make_shared<WithSTD::Logger>();
    Logger::Impl::get()->currlevel=Logger::Level::DEBUG;

    LevelLoader::base=std::make_shared<WithRapidjson::LevelLoader>();

    auto setting=LevelLoader::get()->loadLevel(
        "Level0.json").value_or(Blueprint::Setting{}
    );

    Core::base=std::make_shared<WithSDL::Core>(setting.window);

    GameLogic::base=std::make_shared<GameLogic>();
    PhysicsSimulator::base=std::make_shared<WithModel2D::PhysicsSimulator>();
    ActorManager::base=std::make_shared<Test::ActorManager>();
    std::static_pointer_cast<Test::ActorManager>(ActorManager::get())->test();
}

MainEngine::~MainEngine(){
    ActorManager::base=nullptr;
    PhysicsSimulator::base=nullptr;
    GameLogic::base=nullptr;
    Core::base=nullptr;
    Logger::Impl::base=nullptr;

    SDL_Quit();
}

void MainEngine::start() noexcept{
    Core::get()->run();
}