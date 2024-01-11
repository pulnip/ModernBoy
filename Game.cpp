#include <algorithm>
#include <cassert>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"
#include "SubEngine.hpp"

SDL_GameEngine::~SDL_GameEngine() {
    SDL_Quit();
}

SDL_GameEngine::SDL_GameEngine() noexcept {
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    }
}

void SDL_GameEngine::postConstruct() noexcept {
    auto self = weak_from_this();

    resourceManager = SubEngine::make<SDL_ResourceManager>(self);
    inputSystem = SubEngine::make<SDL_InputSystem>(self);
    game = SubEngine::make<GameLogic>(self);
    graphicsEngine = SubEngine::make<SDL_GraphicsEngine>(self);

    inputSystem->Observable<GameStatus>::subscribe(game);
}

// deprecated

void SDL_GameEngine::shutDown() noexcept {
    SDL_Quit();
}

void SDL_GameEngine::runLoop() noexcept {
    isRunning = true;

    while (isRunning) {
        processInput();
        updateGame();
        generateOutput();
    }
}

void SDL_GameEngine::appendActor(const std::shared_ptr<Actor> actor) noexcept {
    if (isUpdatingActors) {
        pendingActors.emplace_back(actor);
    } else {
        actors.emplace_back(actor);
    }
}

void SDL_GameEngine::updateGame() noexcept {
    // 마지막 프레임 이후로 최소한 16ms가 경과할 때까지 대기
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), ticksCount + 16))
        ;

    // 델타 시간은 현재 프레임의 틱 값과 이전 프레임 틱 값의 차다.
    float deltaTime = (SDL_GetTicks() - ticksCount) / 1000.0f; // ms -> s
    // 최대 델타 시간 값은 0.05s
    deltaTime = std::min(0.05f, deltaTime);

    // 다음 프레임을 위해 틱 값을 갱신
    ticksCount = SDL_GetTicks();

    // 모든 액터를 갱신
    isUpdatingActors = true;
    for (auto actor : actors) {
        actor->update(deltaTime);
    }
    isUpdatingActors = false;

    // 전면 버퍼와 후면 버퍼 교환
    graphicsEngine->changeColorBuffer();

    // 대기 중인 액터를 활성화
    for (auto pending : pendingActors) {
        actors.emplace_back(pending);
    }
    pendingActors.clear();

    // 죽은 액터를 제거
    for (auto it = actors.begin(); it != actors.end();) {
        if ((*it)->getState() == Actor::EDead) {
            actors.erase(it++);
        } else {
            ++it;
        }
    }
}

void p1pingpong::postConstruct(std::shared_ptr<SDL_GameEngine> self) noexcept {
    auto ceil = Actor::make<Wall>(self);
    ceil->position = {1024.0f / 2, 15.0f / 2};
    ceil->baseSize = {1024.0f, 15.0f};

    auto floor = Actor::make<Wall>(self);
    floor->position = {1024.0f / 2, 768.0f - 15.0f / 2};
    floor->baseSize = {1024.0f, 15.0f};

    auto rightWall = Actor::make<Wall>(self);
    rightWall->position = {1024.0f - 15.0f / 2, 768.0f / 2};
    rightWall->baseSize = {15.0f, 768.0f};

    auto paddle = Actor::make<Paddle>(self);
    paddle->allowCollision(ceil);
    paddle->allowCollision(floor);

    auto ball = Actor::make<Ball>(self);
    ball->allowCollision(ceil);
    ball->allowCollision(floor);
    ball->allowCollision(rightWall);
    ball->allowCollision(paddle);
}

void spaceShip::postConstruct(std::shared_ptr<SDL_GameEngine> self) noexcept {
    auto ship = Actor::make<Ship>(self);

    // Create actor for the background (this doesn't need a subclass)
    auto bgActor = Actor::make<Actor>(self);
    bgActor->position = Vector2{512.0f, 384.0f};
    // Create the "far back" background
    auto fbg = Component::Factory::make<BGSpriteComponent>(bgActor);
    fbg->setScreenSize(Vector2{1024.0f, 768.0f});
    std::vector<SDL_Texture *> fbgtexs = {
        getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Farback01.png"),
        getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Farback02.png")};
    fbg->setBGTextures(fbgtexs);
    fbg->setScrollSpeed(-100.0f);
    // Create the closer background
    auto cbg = Component::Factory::make<BGSpriteComponent>(bgActor);
    cbg->setUpdateOrder(101);
    cbg->setScreenSize(Vector2{1024.0f, 768.0f});
    std::vector<SDL_Texture *> cbgtexs = {
        getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Stars.png"),
        getTexture("C:/Users/choiw/Documents/GameEngineDevelopment/resource/Stars.png")};
    cbg->setBGTextures(cbgtexs);
    cbg->setScrollSpeed(-200.0f);

    for (int i = 0; i < 20; ++i) {
        auto asteroid = Actor::make<Asteroid>(self);
    }
}