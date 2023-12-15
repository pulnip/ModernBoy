#pragma once

#include <map>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Math.hpp"

class Game{
public:
    Game() noexcept=default;
    ~Game();
    // 게임 초기화
    bool Initialize();
    // 게임 세계의 액터를 로드
    void LoadData();
    // 게임이 끝나기 전까지 게임 루프를 실행
    void RunLoop();
    // 게임 종료
    void ShutDown();

    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);
    void ClearActors();

    SDL_Texture* GetTexture(const char* fileName);

    void appendDrawable(class DrawComponent* drawable);
    void removeDrawable(class DrawComponent* drawable);

    auto getKeyState() const{ return keyState; }
private:
    // 게임 루프를 위한 헬퍼 함수
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    // 이미지 로딩 과정 캡슐화
    SDL_Texture* LoadTexture(const char* fileName);
public:
    SDL_Event event;

    // 게임이 계속 실행돼야 하는지를 판단
    bool mIsRunning=false;
private:
    // SDL이 생성한 윈도우
    SDL_Window* mWindow=nullptr;

    const Uint8* keyState;

    SDL_Renderer* mRenderer;
    Uint32 mTicksCount=0;

    std::vector<class Actor*> mActors;
    std::vector<class Actor*> mPendingActors;
    bool mUpdatingActors=false;

    std::map<const std::string, SDL_Texture*> textures;
    std::vector<class DrawComponent*> mDrawables;
};

class p1pingpong final: public Game{

};