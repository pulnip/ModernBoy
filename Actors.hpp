#pragma once

#include <vector>

#include "Math.hpp"

// Actor interface

class Actor{
public:
    // 액터의 상태를 추적하는 데 사용
    enum State{
        EActive, EPaused, EDead
    };

    Actor(class Game* game);
    virtual ~Actor();

    // Game에서 호출하는 Update함수
    void Update(float deltaTime);
    // 액터에 부착된 모든 컴포넌트를 업데이트
    void UpdateComponents(float deltaTime);
    // 특정 액터에 특화된 업데이트 코드
    virtual void UpdateActorFirst(float deltaTime)=0;
    virtual void UpdateActorLast(float deltaTime)=0;

    // Getters/Setters
    State GetState() const{ return mState; }
    class Game* GetGame(){ return mGame; }
    const Vector2& GetPosition() const{ return mPosition; }
    void SetPosition(const Vector2& other){ mPosition = other; }
    const Vector2& GetVelocity() const{ return mVelocity; }
    void SetVelocity(const Vector2& other){ mVelocity = other; }
    const Vector2& GetSize() const{ return *mSize; }
    float GetScale() const{ return mScale; }
    float GetRotation() const{ return mRotation; }

    void AddComponent(class Component* component);
    void RemoveComponent(class Component* component);
protected:
    Vector2 mPosition;
    Vector2 mVelocity;
    const Vector2* mSize;
    // 라디안
    float mScale=1.0f;
    float mRotation=0.0f;

    class Game* mGame;
private:
    State mState=EActive;

    // 액터가 보유한 컴포넌트들
    std::vector<class Component*> mComponents;
};

// Real Actors

class Paddle final: public Actor{
public:
    Paddle(class Game* game);
    ~Paddle()=default;

    void UpdateActorFirst(float deltaTime) override;
    void UpdateActorLast(float deltaTime) override;
    void CollideAllow(Actor* opponent);
private:
    const float fixed_velocity_y;
    class CollisionComponent* cc;
};

class Wall final: public Actor{
public:
    Wall(class Game* game, int x, int y, int w, int h);
    ~Wall()=default;

    void UpdateActorFirst(float deltaTime) override{}
    void UpdateActorLast(float deltaTime) override{}
};

class Ball final: public Actor{
public:
    Ball(class Game* game, int x, int y, int w, int h);
    ~Ball()=default;

    void UpdateActorFirst(float deltaTime) override{}
    void UpdateActorLast(float deltaTime) override;
    void CollideAllow(Actor* opponent);
private:
    class CollisionComponent* cc;
};
