#include "MainEngine.hpp"
#include "Engine/ActorManager.hpp"
#include "Engine/GameLogic.hpp"
#include "Engine/Graphics.hpp"
#include "WithSDL/Core.hpp"
#include "WithSDL/Logger.hpp"

#include "SDL2/SDL.h"

using namespace Engine;

MainEngine::MainEngine() noexcept{
    Engine::title="GameProgramming in C++";
    Engine::screen={100, 100, 1024, 768};

    SDL_Init(SDL_INIT_EVERYTHING);
    Logger::base=std::make_shared<WithSDL::Logger>();
    Logger::get()->currlevel=Logger::Level::DEBUG;

    Core::base=std::make_shared<WithSDL::Core>();

    GameLogic::base=std::make_shared<GameLogic>();
    ActorManager::base=std::make_shared<ActorManager>();
}

MainEngine::~MainEngine(){
    SDL_Quit();
}

void MainEngine::start() noexcept{
    Core::get()->run();
}