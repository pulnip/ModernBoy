#include <SDL2/SDL.h>

#include "GameEngineWithSDL.hpp"

#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GameLogic.hpp"
#include "GraphicsEngineWithSDL.hpp"
#include "InputSystemWithSDL.hpp"
#include "ResourceManagerWithSDL.hpp"
#include "TimerWithSDL.hpp"

GameEngineWithSDL::~GameEngineWithSDL() {
    SDL_Quit();
}

GameEngineWithSDL::GameEngineWithSDL() noexcept {
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    }
}

void GameEngineWithSDL::injectDependency() noexcept {
    auto self = weak_from_this();

    ISubEngine::make<GraphicsEngineWithSDL>(self);
    // must call after graphicsEngine
    ISubEngine::make<ResourceManagerWithSDL>(self);

    ISubEngine::make<NullGameLogic>(self);
    // must call after GameLogic
    ISubEngine::make<InputSystemWithSDL>(self);

    timer = Timer::make<TimerWithSDL>();
}
