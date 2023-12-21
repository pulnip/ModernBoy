#include <algorithm>
#include <cassert>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Actors.hpp"
#include "Components.hpp"
#include "Game.hpp"

bool Game::initialize(const std::weak_ptr<Game> game) noexcept{
    self=game;

    int sdlResult=SDL_Init(SDL_INIT_VIDEO);
    if(sdlResult != 0 ){
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    window=SDL_CreateWindow(
        "Game Programming in C++",
        100, 100,
        1024, 768,
        0
    );
    if(!window){
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    renderer=SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if(!renderer){
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    // Init SDL Image Library
    if(IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG){
        SDL_Log("Unable to initialize SDL_image: %s", SDL_GetError());
        return false;
    }

    loadData();

    return true;
}

void Game::loadData() noexcept{
    assert(!self.expired() && "self: expired");
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

    auto ship=std::make_shared<Ship>(self);

	// Create actor for the background (this doesn't need a subclass)
	auto temp=std::make_shared<Actor>(self);
	temp->setPosition(Vector2{512.0f, 384.0f});
	// Create the "far back" background
	auto fbg=std::make_shared<BGSpriteComponent>(temp);
	fbg->setScreenSize(Vector2{1024.0f, 768.0f});
	std::vector<SDL_Texture*> fbgtexs = {
		getTexture("../resource/Farback01.png"),
		getTexture("../resource/Farback02.png")
	};
	fbg->setBGTextures(fbgtexs);
	fbg->setScrollSpeed(-100.0f);
	// Create the closer background
    auto cbg=std::make_shared<BGSpriteComponent>(temp, 101);
	cbg->setScreenSize(Vector2{1024.0f, 768.0f});
	std::vector<SDL_Texture*> cbgtexs={
		getTexture("../resource/Stars.png"),
		getTexture("../resource/Stars.png")
	};
	cbg->setBGTextures(cbgtexs);
	cbg->setScrollSpeed(-200.0f);

    for(int i=0; i<20; ++i){
        auto asteroid=std::make_shared<Asteroid>(self);
    }
}

void Game::shutDown() noexcept{
    for(auto key: textures) SDL_DestroyTexture(key.second);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::runLoop() noexcept{
    isRunning=true;
    
    while(isRunning){
        processInput();
        updateGame();
        generateOutput();
    }
}

void Game::appendActor(const std::shared_ptr<Actor> actor) noexcept{
    if(isUpdatingActors){
        pendingActors.emplace_back(actor);
    }
    else{
        actors.emplace_back(actor);
    }
}

SDL_Texture* Game::getTexture(const char* fileName) noexcept{
    auto texture=textures[fileName];
    if(texture!=nullptr) return texture;

    return textures[fileName]=loadTexture(fileName);
}

void Game::appendDrawable(const std::weak_ptr<DrawComponent> drawable) noexcept{
    assert(!drawable.expired() && "drawable: expired");
    auto it = drawables.cbegin();
    
    const int myDrawOrder = drawable.lock()->getDrawOrder();
    for(; it!=drawables.cend(); ++it){
        assert(!(*it).expired() && "it: expired");
        // mSprites는 이미 정렬되어 있음.
        // sprite보다 그리기 순서가 큰 요소 앞에 삽입해야함.
        if((*it).lock()->getDrawOrder() > myDrawOrder) break;
    }

    drawables.insert(it, drawable);
}

void Game::removeDrawable(const std::weak_ptr<DrawComponent> drawable) noexcept{
    assert(!drawable.expired() && "drawable: expired");
    
    drawables.erase(
        std::find_if(drawables.cbegin(), drawables.cend(),
            [&drawable](const auto& d){
                assert(!d.expired() && "d: expired");
                return drawable.lock().get() == d.lock().get();
            }
        )
    );
}

void Game::processInput() noexcept{
    SDL_Event event;
    // 큐에 여전히 이벤트가 남아있는 동안
    while(SDL_PollEvent(&event)){
        switch(event.type){
        case SDL_QUIT:
            isRunning=false;
            break;
        }
    }

    keyState = SDL_GetKeyboardState(nullptr);
    // ESC로 게임 종료
    if(keyState[SDL_SCANCODE_ESCAPE]){
        isRunning=false;
    }

    isUpdatingActors=true;
    for(auto actor: actors){
        actor->processInput(keyState);
    }
    isUpdatingActors=false;
}

void Game::updateGame() noexcept{
    // 마지막 프레임 이후로 최소한 16ms가 경과할 때까지 대기
    while(!SDL_TICKS_PASSED(SDL_GetTicks(), ticksCount+16));

    // 델타 시간은 현재 프레임의 틱 값과 이전 프레임 틱 값의 차다.
    float deltaTime = (SDL_GetTicks() - ticksCount) / 1000.0f; // ms -> s
    // 최대 델타 시간 값은 0.05s
    deltaTime = std::min(0.05f, deltaTime);

    // 다음 프레임을 위해 틱 값을 갱신
    ticksCount=SDL_GetTicks();

    // 모든 액터를 갱신
    isUpdatingActors = true;
    for(auto actor: actors){
        actor->update(deltaTime);
    }
    isUpdatingActors = false;

    // 대기 중인 액터를 활성화
    for(auto pending: pendingActors){
        actors.emplace_back(pending);
    }
    pendingActors.clear();

    // 죽은 액터를 제거
    for(auto it=actors.begin(); it!=actors.end();){
        if((*it)->getState() == Actor::EDead){
            actors.erase(it++);
        }
        else{
            ++it;
        }
    }
}

void Game::generateOutput() noexcept{
    // 후면 버퍼를 단색으로 클리어
    SDL_SetRenderDrawColor(renderer,
        0, 0, 255,  // 파란 배경
        255
    );
    SDL_RenderClear(renderer);
    
    // 전체 게임 장면 그리기
    for(auto d: drawables){
        assert(!d.expired() && "d: expired");
        d.lock()->draw(renderer);
    }

    // 전면 버퍼와 후면 버퍼 교환
    SDL_RenderPresent(renderer);
}

SDL_Texture* Game::loadTexture(const char* fileName) noexcept{
    // 파일로부터 로딩
    SDL_Surface* surf = IMG_Load(fileName);
    if(!surf){
        SDL_Log("Failed to load texture file %s", fileName);
        return nullptr;
    }

    // 텍스쳐 생성
    auto texture=SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if(!texture){
        SDL_Log("Failed to convert surface to texture for %s", fileName);
        return nullptr;
    }
    return texture;
}