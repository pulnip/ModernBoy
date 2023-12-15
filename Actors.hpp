#pragma once

#include <cstdint>
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
    void update(float deltaTime);
    // 액터에 부착된 모든 컴포넌트를 업데이트
    void updateComponents(float deltaTime);
    // 특정 액터에 특화된 업데이트 코드
    virtual void updateActorFirst(float deltaTime){}
    virtual void updateActorLast(float deltaTime){}

    // Game에서 호출하는 ProcessInput함수
    void processInput(const uint8_t* keyState);
    // 특정 액터를 위한 입력 코드
    virtual void actorInput(const uint8_t* keyState){}

    // Getters/Setters
    State getState() const{ return mState; }
    class Game* getGame(){ return mGame; }
    const Vector2& getPosition() const{ return mPosition; }
    void setPosition(const Vector2& other){ mPosition = other; }
    const Vector2& getVelocity() const{ return mVelocity; }
    void setVelocity(const Vector2& other){ mVelocity = other; }
    const Vector2& getSize() const{ return *mSize; }
    float getScale() const{ return mScale; }
    void setScale(float other){ mScale=other; }
    float getRotation() const{ return mRotation; }
    void setRotation(float other){ mRotation=other; }

    void appendComponent(class Component* component);
    void removeComponent(class Component* component);
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

    void updateActorFirst(float deltaTime) override;
    void updateActorLast(float deltaTime) override;
    void collideAllow(Actor* opponent);
private:
    const float fixed_velocity_y;
    class CollisionComponent* cc;
};

class Wall final: public Actor{
public:
    Wall(class Game* game, int x, int y, int w, int h);
    ~Wall()=default;

    void updateActorFirst(float deltaTime) override{}
    void updateActorLast(float deltaTime) override{}
};

class Ball final: public Actor{
public:
    Ball(class Game* game, int x, int y, int w, int h);
    ~Ball()=default;

    void updateActorLast(float deltaTime) override;
    void collideAllow(Actor* opponent);
private:
    class CollisionComponent* cc;
};

class Ship final: public Actor{
public:
    Ship(class Game* game);
};

class Asteroid final: public Actor{
public:
    Asteroid(class Game* game);
};
