#pragma once

#include <vector>

#include "Vector2.hpp"

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
    virtual void UpdateActor(float deltaTime);

    // Getters/Setters
    State GetState() const{ return mState; }

    void AddComponent(class Component* component);
    void RemoveComponent(class Component* component);
protected:
    class Game* game;
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

    void UpdateActor(float deltaTime) override;
public:
    class CollisionComponent* cc;
private:
    class TransformComponent* tc;
    const Vector2 start_position, fixed_velocity, fixed_size;
};

class Wall final: public Actor{
public:
    Wall(class Game* game, int x, int y, int w, int h);
    ~Wall()=default;

    void UpdateActor(float deltaTime) override{}
public:
    class CollisionComponent* cc;
};

class Ball final: public Actor{
public:
    Ball(class Game* game, int x, int y, int w, int h);
    ~Ball()=default;

    void UpdateActor(float deltaTime) override;
public:
    class CollisionComponent* cc;
};
