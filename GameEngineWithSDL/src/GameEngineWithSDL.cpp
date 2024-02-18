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
    assert(not owner.expired());
    auto self = owner.lock()->getEngine();

    Game::SubEngine::Interface::make<
        WithSDL::SubEngine::GraphicsEngine
    >(self);
    // must call after graphicsEngine
    Game::SubEngine::Interface::make<
        WithSDL::SubEngine::ResourceManager
    >(self);

    Game::SubEngine::Interface::make<
        Null::SubEngine::GameLogic
    >(self);
    // must call after GameLogic
    Game::SubEngine::Interface::make<
        WithSDL::SubEngine::InputSystem
    >(self);

    timer = Game::SubEngine::Timer::make<
        WithSDL::SubEngine::Timer
    >();
}
