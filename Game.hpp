#pragma once

#include <map>
#include <memory>
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

    void appendActor(std::shared_ptr<class Actor> actor);
    void removeActor(class Actor* actor);
    void ClearActors();

    SDL_Texture* GetTexture(const char* fileName);
    void appendDrawable(std::weak_ptr<class DrawComponent> drawable);
    void removeDrawable(class DrawComponent* drawable);
private:
    // 게임 루프를 위한 헬퍼 함수
    void ProcessInput();
    void UpdateGame();
    void GenerateOutput();

    // 이미지 로딩 과정 캡슐화
    std::weak_ptr<class SDL_Texture> LoadTexture(const char* fileName);
private:
    const Uint8* keyState;
    // SDL이 생성한 윈도우
    std::unique_ptr<class SDL_Window> mWindow;
    std::shared_ptr<class SDL_Renderer> mRenderer;

    // 게임이 계속 실행돼야 하는지를 판단
    bool mIsRunning=false;
    Uint32 mTicksCount=0;

    std::vector<std::shared_ptr<class Actor>> mActors;
    std::vector<std::shared_ptr<class Actor>> mPendingActors;
    bool mUpdatingActors=false;

    std::map<const std::string, std::shared_ptr<class SDL_Texture>> textures;
    std::vector<std::weak_ptr<class DrawComponent>> mDrawables;
};

class p1pingpong final: public Game{

};