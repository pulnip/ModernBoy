#pragma once

#include <vector>

#include <SDL.h>

#include "Vector2.hpp"

// Component interface

class Component{
public:
    // 업데이트 순서값이 작을수록 더 빨리 갱신
    Component(class Actor* owner, int updateOrder=100);
    virtual ~Component();
    
    virtual void update(float deltaTime){}
    int GetUpdateOrder() const{ return mUpdateOrder; }
protected:
    // 소유자 액터
    class Actor* mOwner;
    // 컴포넌트의 업데이트 순서
    int mUpdateOrder;
};

// Real Components

// 패들 조종 전용 컴포넌트
class ControlComponent final: public Component{
public:
    ControlComponent(class Actor* owner)
    :Component(owner, 1){}
    ~ControlComponent()=default;

    void update(float deltaTime) override;
};

class CollisionComponent final: public Component{
public:
    CollisionComponent(class Actor* owner)
    :Component(owner, 2){}
    ~CollisionComponent()=default;

    void update(float deltaTime) override;

    void Allow(const class Actor* opponent);
    void Disallow(const class Actor* opponent);
private:
    std::vector<const class Actor*> opponents;
};

class DrawComponent: public Component{
public:
    DrawComponent(class Actor* owner, int drawOrder);
    virtual ~DrawComponent();

    virtual void Draw(SDL_Renderer* renderer)=0;

    int GetDrawOrder(){ return mDrawOrder; }
    const Vector2& GetSize() const{ return mSize; }
protected:
    // 그리기 순서(화가 알고리즘)
    const int mDrawOrder;
    // 텍스쳐의 너비/높이
    Vector2 mSize;
};

struct Color{ short r=0, g=0, b=0, a=255; };

class BoxComponent final: public DrawComponent{
public:
    BoxComponent(class Actor* owner, int drawOrder=1)
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

class SpriteComponent: public DrawComponent{
public:
    SpriteComponent(class Actor* owner, int drawOrder=2)
    :DrawComponent(owner, drawOrder){}
    ~SpriteComponent()=default;

    virtual void Draw(SDL_Renderer* renderer) override;
    virtual void SetTexture(SDL_Texture* texture);
private:
    SDL_Texture* mTexture;
};