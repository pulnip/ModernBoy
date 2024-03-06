#include <SDL2/SDL.h>
#include "WithSDL/Core.hpp"
#include "WithSDL/Timer.hpp"
#include "WithSDL/Logger.hpp"
#include "WithSDL/Graphics.hpp"
#include "WithSDL/InputSystem.hpp"
#include "WithSDL/ResourceManager.hpp"

using namespace WithSDL;

Core::Core() noexcept:
    Engine::Core(std::make_unique<Timer>())
{
    Graphics::make();
    ResourceManager::make();
    InputSystem::make();
}

void Core::preConstruct() noexcept{
    SDL_Init(SDL_INIT_EVERYTHING);
    Logger::make();
}

Core::~Core(){
    WithSTD::Logger::make();
    Graphics::get()->destroyAll();
    SDL_Quit();
}