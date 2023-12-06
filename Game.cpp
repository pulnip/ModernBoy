#include <algorithm>

#include "Components.hpp"
#include "Game.hpp"
#include "Objects.hpp"

Game::Game()=default;

bool Game::Initialize(){
    int sdlResult=SDL_Init(SDL_INIT_VIDEO);
    if(sdlResult != 0 ){
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow(
        "Game Programming in C++",
        100, 100,
        1024, 768,
        0
    );
    if(!mWindow){
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = SDL_CreateRenderer(
        mWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if(!mRenderer){
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    auto ceil=new Wall(this, 0, 0, 1024, 15);
    auto floor=new Wall(this, 0, 768-15, 1024, 15);
    auto rightWall=new Wall(this, 1024-15, 0, 15, 768);
    AddActor(ceil);
    AddActor(floor);
    AddActor(rightWall);

    auto paddle=new Paddle(this);
    paddle->cc->collideAllow(ceil->cc);
    paddle->cc->collideAllow(floor->cc);
    AddActor(paddle);

    return true;
}

void Game::ShutDown(){
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();

    ClearActors();
}

void Game::RunLoop(){
    mIsRunning=true;
    
    while(mIsRunning){
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::AddActor(Actor* actor){
    if(mUpdatingActors){
        mPendingActors.emplace_back(actor);
    }
    else{
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor){
    mActors.erase(
        std::find(mActors.cbegin(), mActors.cend(), actor)
    );
}

void Game::ClearActors(){
    while(!mActors.empty()){
        delete mActors.back();
    }
}

void Game::ProcessInput(){
    // 큐에 여전히 이벤트가 남아있는 동안
    while(SDL_PollEvent(&event)){
        switch(event.type){
        case SDL_QUIT:
            mIsRunning=false; break;
        }
    }

    state = SDL_GetKeyboardState(nullptr);

    // ESC로 게임 종료
    if(state[SDL_SCANCODE_ESCAPE]){
        mIsRunning=false;
    }
}

void Game::UpdateGame(){
    // 마지막 프레임 이후로 최소한 16ms가 경과할 때까지 대기
    while(!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount+16));

    // 델타 시간은 현재 프레임의 틱 값과 이전 프레임 틱 값의 차다.
    float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f; // ms -> s
    // 최대 델타 시간 값은 0.05s
    deltaTime = std::min(0.05f, deltaTime);

    // 다음 프레임을 위해 틱 값을 갱신
    mTicksCount=SDL_GetTicks();

    // 모든 액터를 갱신
    mUpdatingActors = true;
    for(auto actor: mActors){
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    // 대기 중인 액터를 활성화
    for(auto pending: mPendingActors){
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    // 죽은 액터를 임시 벡터에 추가
    std::vector<Actor*> deadActors;
    for(auto actor: mActors){
        if(actor->GetState() == Actor::EDead){
            deadActors.emplace_back(actor);
        }
    }

    // 죽은 액터 제거
    for(auto actor: deadActors){
        delete actor;
    }
}

void Game::GenerateOutput(){
    // 후면 버퍼를 단색으로 클리어
    SDL_SetRenderDrawColor(mRenderer,
        0, 0, 255,  // 파란 배경
        255
    );
    SDL_RenderClear(mRenderer);
    
    // 전체 게임 장면 그리기
    for(auto d: drawables){
        SDL_SetRenderDrawColor(mRenderer,
            d->r, d->g, d->b,
            d->a
        );
        SDL_Rect rect{
            static_cast<int>(d->tc->position.x), static_cast<int>(d->tc->position.y),
            static_cast<int>(d->size.x), static_cast<int>(d->size.y)
        };
        SDL_RenderFillRect(mRenderer, &rect);
    }

    // 전면 버퍼와 후면 버퍼 교환
    SDL_RenderPresent(mRenderer);
}