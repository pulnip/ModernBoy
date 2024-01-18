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

    auto graphicsEngine = SubEngine::make<GraphicsEngineWithSDL>(self);
    // must call after graphicsEngine
    auto resourceManager = SubEngine::make<ResourceManagerWithSDL>(self);

    auto gameLogic = SubEngine::make<NullGameLogic>(self);
    // must call after GameLogic
    auto inputSystem = SubEngine::make<InputSystemWithSDL>(self);

    auto actorManager = SubEngine::make<NullActorManager>(self);
    timer = Timer::make<TimerWithSDL>();
}
