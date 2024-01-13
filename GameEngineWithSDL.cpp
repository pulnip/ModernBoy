#include <SDL2/SDL.h>

#include "ActorManager.hpp"
#include "GameEngineWithSDL.hpp"
#include "GameLogic.hpp"
#include "GraphicsEngineWithSDL.hpp"
#include "InputSystemWithSDL.hpp"
#include "ResourceManagerWithSDL.hpp"

GameEngineWithSDL::~GameEngineWithSDL() {
    SDL_Quit();
}

GameEngineWithSDL::GameEngineWithSDL() noexcept {
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    }
}

void GameEngineWithSDL::postConstruct() noexcept {
    auto self = weak_from_this();

    // dependency injection
    resourceManager = SubEngine::make<ResourceManagerWithSDL>(self);
    inputSystem = SubEngine::make<InputSystemWithSDL>(self);
    gameLogic = SubEngine::make<GameLogic>(self);
    actorManager = SubEngine::make<ActorManager>(self);
    graphicsEngine = SubEngine::make<GraphicsEngineWithSDL>(self);

    inputSystem->Observable<GameStatus>::subscribe(gameLogic);
}