#include "Game.h"

#include <algorithm>

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

    mPaddlePos.x = thickness;
    mPaddlePos.y = 768/2;

    mBallPos.x = 1024/2;
    mBallPos.y = 768/2;

    return true;
}

void Game::ShutDown(){
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void Game::RunLoop(){
    mIsRunning=true;
    
    while(mIsRunning){
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}



void Game::ProcessInput(){
    SDL_Event event;
    // 큐에 여전히 이벤트가 남아있는 동안
    while(SDL_PollEvent(&event)){
        switch(event.type){
        case SDL_QUIT:
            mIsRunning=false; break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);
    // ESC로 게임 종료
    if(state[SDL_SCANCODE_ESCAPE]){
        mIsRunning=false;
    }
    // 패들 조종
    mPaddleDir=0;
    if(state[SDL_SCANCODE_W]){
        mPaddleDir -= 1;
    }
    if(state[SDL_SCANCODE_S]){
        mPaddleDir += 1;
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

    // 패들 갱신
    if(mPaddleDir != 0){
        // 패들의 속도는 300pixel / s;
        mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
        // 패들이 화면 위로 넘어가지 않도록
        mPaddlePos.y = std::max(thickness+PaddleHeight/2.0f, mPaddlePos.y);
        // 패들이 화면 아래로 넘어가지 않도록
        mPaddlePos.y = std::min(768-thickness-PaddleHeight/2.0f, mPaddlePos.y);
    }

    // 공 위치 갱신
    mBallPos.x += mBallVel.x * deltaTime;
    mBallPos.y += mBallVel.y * deltaTime;

    // 상단 벽과 충돌
    if(mBallPos.y <= thickness + thickness/2){
        // 상단 벽으로 이동 중
        if(mBallVel.y < 0.0f){
            // 그렇다면, 이동 방향을 반전
            mBallVel.y = -mBallVel.y;
        }
    }
    // 하단 벽과 충돌
    else if(mBallPos.y >= 768 - thickness - thickness/2){
        if(mBallVel.y > 0.0f){
            mBallVel.y = -mBallVel.y;
        }
    }
    
    // 패들과 공의 위치 차
    Vector2 diff{
        std::abs(mPaddlePos.x - mBallPos.x),
        std::abs(mPaddlePos.y - mBallPos.y)
    };
    // 패들과 충돌
    if(
        // x축에 대해
        (diff.x <= thickness/2 + thickness/2 ) // 패들과 공의 너비 합의 절반
        && // y축에 대해
        (diff.y <= PaddleHeight/2 + thickness/2 ) // 패들과 공의 높이 합의 절반 
    ){
        if(mBallVel.x < 0.0f ){
            mBallVel.x = -mBallVel.x;
        }
    }
    // 우측 벽과 충돌
    else if(mBallPos.x >= 1024 - thickness - thickness/2){
        if(mBallVel.x > 0.0f){
            mBallVel.x = -mBallVel.x;
        }
    }

    // 공이 화면 밖으로 나가면
    if(mBallPos.x <= -thickness/2){
        // 게임 종료
        mIsRunning=false;
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
    // 벽 그리기
    SDL_SetRenderDrawColor(mRenderer,
        255, 255, 255, // 하얀색으로
        255
    );
    SDL_Rect ceiling{
        0, 0, // position
        1024, thickness // size
    };
    SDL_Rect floor{
        0, 768-thickness, // position
        1024, thickness // size
    };
    SDL_Rect wall{
        1024-thickness, 0, // position
        thickness, 768 // size
    };
    SDL_RenderFillRect(mRenderer, &ceiling);
    SDL_RenderFillRect(mRenderer, &floor);
    SDL_RenderFillRect(mRenderer, &wall);

    // 공과 패들 그리기
    SDL_Rect ball{
        static_cast<int>(mBallPos.x - thickness/2), static_cast<int>(mBallPos.y - thickness/2),
        thickness, thickness
    };
    SDL_Rect paddle{
        static_cast<int>(mPaddlePos.x - thickness/2), static_cast<int>(mPaddlePos.y - PaddleHeight/2),
        thickness, PaddleHeight
    };
    SDL_RenderFillRect(mRenderer, &ball);
    SDL_RenderFillRect(mRenderer, &paddle);

    // 전면 버퍼와 후면 버퍼 교환
    SDL_RenderPresent(mRenderer);
}