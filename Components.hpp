#pragma once

#include <vector>

#include <SDL2/SDL.h>

#include "Math.hpp"

// Component interface
class Component{
public:
    // updateOrder이 작을수록 더 빨리 갱신
    // input 계열: 100 to 199
    // 연산 계열: 200 to 299
    // output계열: 300 to 399
    Component(class Actor* owner, int updateOrder);
    virtual ~Component();
    
    virtual void Update(float deltaTime)=0;
    virtual void ProcessInput(const uint8_t* keyState){}

    int GetUpdateOrder() const{ return mUpdateOrder; }
protected:
    // 소유자 액터
    class Actor* mOwner;
    // 컴포넌트의 업데이트 순서
    int mUpdateOrder;
};

// Real Components

// (패들 전용) 조종 컴포넌트
class ControlComponent final: public Component{
public:
    ControlComponent(class Actor* owner, int updateOrder=100)
    :Component(owner, updateOrder){}
    ~ControlComponent()=default;

    void Update(float deltaTime) override;
};

// 충돌 처리 컴포넌트
class CollisionComponent final: public Component{
public:
    CollisionComponent(class Actor* owner, int updateOrder=200)
    :Component(owner, updateOrder){}
    ~CollisionComponent()=default;

    void Update(float deltaTime) override;

    void Allow(const class Actor* opponent);
    void Disallow(const class Actor* opponent);
private:
    std::vector<const class Actor*> opponents;
};

// 2D Graphics interface
class DrawComponent: public Component{
public:
    // drawOrder이 작을수록 더 뒤에 위치
    // 배경 계열: 100 to 199
    // 일반 오브젝트 계열: 200 to 299
    // player계열: 300 to 399
    DrawComponent(class Actor* owner, int drawOrder);
    virtual ~DrawComponent();

    virtual void Update(float deltaTime) override{}
    virtual void Draw(SDL_Renderer* renderer)=0;

    int GetDrawOrder(){ return mDrawOrder; }
    const Vector2& GetSize() const{ return mSize; }
protected:
    // 그리기 순서(화가 알고리즘)
    const int mDrawOrder;
    // 텍스처의 너비/높이
    Vector2 mSize;
};

// Color Box 텍스처
struct Color{ short r=0, g=0, b=0, a=255; };

class BoxComponent final: public DrawComponent{
public:
    BoxComponent(class Actor* owner, int drawOrder=200)
    :DrawComponent(owner, drawOrder){}
    ~BoxComponent(){
        delete mColor;
    }

    void Draw(SDL_Renderer* renderer) override;
    void SetTexture(Color* color, int w, int h){
        mColor=color; mSize.x=w; mSize.y=h;
    }
private:
    Color* mColor;
};

// 단일 스프라이트 텍스처
class SpriteComponent: public DrawComponent{
public:
    SpriteComponent(class Actor* owner, int drawOrder=201)
    :DrawComponent(owner, drawOrder){}
    ~SpriteComponent()=default;

    virtual void Draw(SDL_Renderer* renderer) override;
    virtual void SetTexture(SDL_Texture* texture);
private:
    SDL_Texture* mTexture;
};

// 애니메이션 텍스처
class AnimSpriteComponent final: public SpriteComponent{
public:
    AnimSpriteComponent(class Actor* owner, int drawOrder=202)
    :SpriteComponent(owner, drawOrder){}
    // 애니메이션을 프레임마다 갱신
    void Update(float deltaTime) noexcept override;
    // 애니메이션에 사용되는 텍스처 설정
    void SetAnimTextures(const std::vector<SDL_Texture*>& textures){
        mAnimTextures=textures;
    }
    // 애니메이션 FPS
    float GetAnimFPS() const{ return mAnimFPS; }
    void SetAnimFPS(float fps){ mAnimFPS=fps; }
private:
    std::vector<SDL_Texture*> mAnimTextures;
    // 현재 프레임
    float mCurrFrame=0.0f;
    float mAnimFPS=8.0f;
};

// 스크롤되는 배경
class BGSpriteComponent final: public SpriteComponent{
public:
    BGSpriteComponent(class Actor* owner, int drawOrder=100)
    :SpriteComponent(owner, drawOrder){}
    
    void Update(float deltaTime) noexcept override;
    void Draw(SDL_Renderer* renderer) noexcept override;

    // 배경용 텍스처 설정
    void SetBGTextures(const std::vector<SDL_Texture*>& textures);
    void SetScreenSize(const Vector2& size){ mScreenSize=size; }
    void SetScrollSpeed(float speed){ mScrollSpeed=speed; }
    float GetScrollSpeed(){ return mScrollSpeed; }

private:
    struct BGTexture{
        SDL_Texture* mTexture=nullptr;
        // 화면이 시작하는 위치
        float mOffset_x;
    };
    std::vector<BGTexture> mBGTextures;
    Vector2 mScreenSize;
    float mScrollSpeed=0.0f;
};

class MoveComponent: public Component{
public:
    MoveComponent(class Actor* owner, int updateOrder=100)
    :Component(owner, updateOrder){}

    void Update(float deltaTime) override;

    float GetAngularSpeed() const{ return mAngularSpeed; }
    float GetForwardSpeed() const{ return mForwardSpeed; }
    void SetAngularSpeed(float speed) { mAngularSpeed=speed; }
    void SetForwardSpeed(float speed) { mForwardSpeed=speed; }
protected:
    // radian per second
    float mAngularSpeed;
    // unit per second
    float mForwardSpeed;
};

class InputComponent final: public MoveComponent{
public:
    InputComponent(class Actor* owner, int updateOrder=99)
    :MoveComponent(owner, updateOrder){}

    void ProcessInput(const uint8_t* keyState) noexcept override;

    void setForwardMoveSpeed(float speed){ mForwardMoveSpeed=speed; }
    float getForwardMoveSpeed(){ return mForwardMoveSpeed; }
    void setAngularMoveSpeed(float radian){ mAngularMoveSpeed=radian; }
    float getAngularMoveSpeed(){ return mAngularMoveSpeed; }
    void setForwardKey(unsigned short key){ mForwardKey=key; }
    void setBackwardKey(unsigned short key){ mBackwardKey=key; }
    void setClockwiseKey(unsigned short key){ mClockwiseKey=key; }
    void setCounterClockwiseKey(unsigned short key){ mCounterClockwiseKey=key; }
private:
    float mForwardMoveSpeed;
    float mAngularMoveSpeed;

    unsigned short mForwardKey;
    unsigned short mBackwardKey;
    unsigned short mClockwiseKey;
    unsigned short mCounterClockwiseKey;
};