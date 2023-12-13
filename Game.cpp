#include <algorithm>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"

Game::Game()=default;

Game::~Game(){
    for(auto actor: mActors) delete actor;
    for(auto actor: mPendingActors) delete actor;
    
    for(auto key: textures) SDL_DestroyTexture(key.second);

    SDL_DestroyWindow(mWindow);

    SDL_Quit();
}

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

    // Init SDL Image Library
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    LoadData();

    return true;
}

void Game::LoadData(){
    // ping-pong

    // auto ceil=new Wall(this, 1024/2, 15/2, 1024, 15);
    // auto floor=new Wall(this, 1024/2, 768-15/2, 1024, 15);
    // auto rightWall=new Wall(this, 1024-15/2, 768/2, 15, 768);

    // auto paddle=new Paddle(this);
    // paddle->CollideAllow(ceil);
    // paddle->CollideAllow(floor);

    // auto ball=new Ball(this, 1024/2, 768/2, 15, 15);
    // ball->CollideAllow(ceil);
    // ball->CollideAllow(floor);
    // ball->CollideAllow(rightWall);
    // ball->CollideAllow(paddle);

    auto ship=new Ship(this);

	// Create actor for the background (this doesn't need a subclass)
	Actor* temp = new Actor(this);
	temp->SetPosition(Vector2{512.0f, 384.0f});
	// Create the "far back" background
	BGSpriteComponent* bg = new BGSpriteComponent(temp);
	bg->SetScreenSize(Vector2{1024.0f, 768.0f});
	std::vector<SDL_Texture*> bgtexs = {
		GetTexture("resource/Farback01.png"),
		GetTexture("resource/Farback02.png")
	};
	bg->SetBGTextures(bgtexs);
	bg->SetScrollSpeed(-100.0f);
	// Create the closer background
	bg = new BGSpriteComponent(temp, 50);
	bg->SetScreenSize(Vector2{1024.0f, 768.0f});
	bgtexs = {
		GetTexture("resource/Stars.png"),
		GetTexture("resource/Stars.png")
	};
	bg->SetBGTextures(bgtexs);
	bg->SetScrollSpeed(-200.0f);

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

SDL_Texture* Game::GetTexture(const char* fileName){
    auto texture=textures[fileName];
    if(texture!=nullptr) return texture;

    texture=LoadTexture(fileName);
    return textures[fileName]=texture;
}

void Game::AddDrawable(DrawComponent* drawable){
    auto it = mDrawables.cbegin();
    
    const int myDrawOrder = drawable->GetDrawOrder();
    for(; it!=mDrawables.cend(); ++it){
        // mSprites는 이미 정렬되어 있음.
        // sprite보다 그리기 순서가 큰 요소 앞에 삽입해야함.
        if((*it)->GetDrawOrder() > myDrawOrder) break;
    }

    mDrawables.insert(it, drawable);
}

void Game::RemoveDrawable(DrawComponent* drawable){
    mDrawables.erase(
        std::find(mDrawables.cbegin(), mDrawables.cend(), drawable)
    );

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
    for(auto d: mDrawables){
        d->Draw(mRenderer);
    }

    // 전면 버퍼와 후면 버퍼 교환
    SDL_RenderPresent(mRenderer);
}

SDL_Texture* Game::LoadTexture(const char* fileName){
    // 파일로부터 로딩
    SDL_Surface* surf = IMG_Load(fileName);
    if(!surf){
        SDL_Log("Failed to load texture file %s", fileName);
        return nullptr;
    }

    // 텍스쳐 생성
    auto texture = SDL_CreateTextureFromSurface(mRenderer, surf);
    SDL_FreeSurface(surf);
    if(!texture){
        SDL_Log("Failed to convert surface to texture for %s", fileName);
        return nullptr;
    }
    return texture;
}