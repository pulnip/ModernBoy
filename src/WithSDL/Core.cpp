#include <SDL2/SDL.h>
#include "WithSDL/Core.hpp"
#include "WithSDL/Timer.hpp"
#include "WithSDL/Logger.hpp"
#include "WithSDL/Graphics.hpp"
#include "WithSDL/InputSystem.hpp"
#include "WithSDL/ResourceManager.hpp"

using namespace WithSDL;

Core::Core(const Blueprint::Window& window) noexcept:
    Engine::Core(std::make_unique<Timer>(), window)
{
    Graphics::make();
    ResourceManager::make();
    InputSystem::make();
}

void Core::preConstruct() noexcept{
    SDL_Init(SDL_INIT_EVERYTHING);
    Logger::make();
}

void Core::postConstruct() noexcept{
    Graphics::get()->initialize(window);
}

Core::~Core(){
    WithSTD::Logger::make();
    Graphics::get()->destroyAll();
    SDL_Quit();

    // InputSystem::base=nullptr;
    // ResourceManager::base=nullptr;
    // Graphics::base=nullptr;
}