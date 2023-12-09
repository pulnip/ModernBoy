#pragma once

#include <vector>

#include "Vector2.hpp"

// Component interface

class Component{
public:
    // 업데이트 순서값이 작을수록 더 빨리 갱신
    Component(class Actor* owner, int updateOrder=100);
    virtual ~Component();
    
    virtual void update(float deltaTime);
    int GetUpdateOrder() const{ return mUpdateOrder; }
protected:
    // 소유자 액터
    class Actor* mOwner;
    // 컴포넌트의 업데이트 순서
    int mUpdateOrder;
};

// Real Components

class TransformComponent final: public Component{
public:
    TransformComponent(class Actor* owner, float x, float y)
    :Component(owner, 4), position{x, y}{}
    ~TransformComponent()=default;

    void update(float deltaTime) override;
public:
    Vector2 position;
    Vector2 velocity;
};

class SimpleDrawComponent final: public Component{
public:
    SimpleDrawComponent(class Actor* owner, float width, float height, TransformComponent* _tc)
    :Component(owner, 2), size{width, height}, tc(_tc){}
    ~SimpleDrawComponent()=default;

    void update(float deltaTime) override{}
public:
    TransformComponent* tc;
    Vector2 size;
    short r=0, g=0, b=0, a=255;
};

class CollisionComponent final: public Component{
public:
    CollisionComponent(class Actor* owner, TransformComponent* _tc, SimpleDrawComponent* _sdc)
    :Component(owner, 3), tc(_tc), sdc(_sdc){}
    ~CollisionComponent()=default;

    void update(float deltaTime) override;

    void collideAllow(const CollisionComponent* opponent);
    void collideDisallow(const CollisionComponent* opponent);
public:
    TransformComponent* tc;
    const SimpleDrawComponent* sdc;
private:
    std::vector<const CollisionComponent*> opponents;
};

// 패들 조종 전용 컴포넌트
class ControlComponent final: public Component{
public:
    ControlComponent(class Actor* owner, TransformComponent* _tc, class Game* _game)
    :Component(owner, 1), tc(_tc), game(_game){};
    ~ControlComponent()=default;

    void update(float deltaTime) override;
private:
    TransformComponent* tc;
    class Game* game;
};

class SpriteComponent: public Component{
public:
    SpriteComponent(class Actor* owner, int drawOrder=100);
    ~SpriteComponent();
    virtual void Draw(SDL_Renderer* renderer);
    virtual void SetTexture(SDL_Texture* texture);

protected:
    // 그릴 텍스쳐
    SDL_Texture* mTexture;
    // 그리기 순서(화가 알고리즘)
    int mDrawOrder;
    // 텍스쳐의 너비/높이
    int mTexWidth;
    int mTexHeight;
};