#include <SDL2/SDL.h>

#include "MainEngine.hpp"
#include "GameEngineWithSDL.hpp"
#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GameLogic.hpp"
#include "GraphicsEngineWithSDL.hpp"
#include "InputSystemWithSDL.hpp"
#include "ResourceManagerWithSDL.hpp"
#include "TimerWithSDL.hpp"

using namespace Game;
using namespace WithSDL::Core;

Engine::Engine(std::shared_ptr<Game::SubEngine::Logger>& logger) noexcept:
    Game::Core::Engine(logger)
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    }
}

Engine::~Engine(){ SDL_Quit(); }

void Engine::postConstruct() noexcept {
    Game::Core::Engine::postConstruct();

    auto self = shared_from_this();

    auto ge=Game::SubEngine::Interface::make<
        WithSDL::SubEngine::GraphicsEngine
    >(self);
    // must call after graphicsEngine
    auto rm=Game::SubEngine::Interface::make<
        WithSDL::SubEngine::ResourceManager
    >(self);

    auto gl=Game::SubEngine::Interface::make<
        Null::SubEngine::GameLogic
    >(self);
    // must call after GameLogic
    auto is=Game::SubEngine::Interface::make<
        WithSDL::SubEngine::InputSystem
    >(self);

    timer = Game::SubEngine::Timer::make<
        WithSDL::SubEngine::Timer
    >();
}
