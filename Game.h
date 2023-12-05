#pragma once

#include <vector>

#include <SDL.h>

struct Vector2{
    float x, y;
};

class Game{
public:
    Game();
    // 게임 초기화
    bool Initialize();
    // 게임이 끝나기 전까지 게임 루프를 실행
    void RunLoop();
    // 게임 종료
    void ShutDown();

    void AddActor(class Actor* actor);
    void ClearActors();
private:
    // 게임 루프를 위한 헬퍼 함수
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    // SDL이 생성한 윈도우
    SDL_Window* mWindow=nullptr;
    // 게임이 계속 실행돼야 하는지를 판단
    bool mIsRunning=false;

    SDL_Renderer* mRenderer;
    Uint32 mTicksCount;

    std::vector<class Actor*> mActors;
    std::vector<class Actor*> mPendingActors;
    bool mUpdatingActors=false;

    const int thickness=15;
    int mPaddleDir=0;
    const int PaddleHeight=8*thickness;
    Vector2 mPaddlePos, mBallPos;
    Vector2 mBallVel={-200.0f, 235.0f};
};