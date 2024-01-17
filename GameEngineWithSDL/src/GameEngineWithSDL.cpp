#include <SDL2/SDL.h>

#include "SubEngine/ActorManager.hpp"
#include "SubEngine/GameLogic.hpp"

#include "GraphicsEngineWithSDL.hpp"
#include "InputSystemWithSDL.hpp"
#include "ResourceManagerWithSDL.hpp"
#include "TimerWithSDL.hpp"

#include "GameEngineWithSDL.hpp"

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

    resourceManager = SubEngine::make<ResourceManagerWithSDL>(self);
    inputSystem = SubEngine::make<InputSystemWithSDL>(self);
    gameLogic = SubEngine::make<GameLogic>(self);
    actorManager = SubEngine::make<ActorManager>(self);
    graphicsEngine = SubEngine::make<GraphicsEngineWithSDL>(self);
    timer = Timer::make<TimerWithSDL>();

    inputSystem->Observable<GameStatus>::subscribe(gameLogic);
}